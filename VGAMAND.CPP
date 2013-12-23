#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define VIDEO_INT 0x10
#define SET_MODE 0x00
#define VGA_256_COLOR_MODE 0x13
#define TEXT_MODE 0x03
#define NUM_COLORS 256

#define SCREEN_HORIZ 320
#define SCREEN_VERT 200

#define GRAPH_YMIN 1
#define GRAPH_YMAX -1
#define GRAPH_XMIN -2
#define GRAPH_XMAX 1

#define ESCAPE_MAX 50

/* Most of the VGA pixel plotting code comes
*  from http://www.brackeen.com/vga/
*  
*  Big thanks to David Brackeen for making the
*  best VGA programming tutorial on the entire
*  internet!
*/

typedef unsigned char byte;
typedef unsigned short word;

byte *VGA=(byte *)0xA0000000L;
word *my_clock=(word *) 0x0000046c;

void set_mode(byte mode)
{
	union REGS regs;
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
}

void plot_pixel_fast(int x, int y, byte color)
{
	VGA[y * SCREEN_HORIZ + x] = color;
}

float pixel_to_graph(int pixel, int graph_width, int screen_width, int graph_min)
{
	return ((float)pixel * (graph_width / (float)screen_width)) + graph_min;
}

int mandelbrot(float real, float imaginary, int escape_max)
{
    float zReal, zImag;
    float z0Real, z0Imag;
    int n, escape;
    zReal = zImag = 0;
    z0Real = z0Imag = 0;
 
    for (n = 0; n <= escape_max; n++)
    {
        z0Real = ((zReal * zReal) - (zImag * zImag) + real);
        z0Imag = (2 * (zReal * zImag)) + imaginary;
 
        if (((z0Real * z0Real) + (z0Imag * z0Imag)) >= 4)
        {
            escape = n;
            return escape;
        }
 
        zReal = z0Real;
        zImag = z0Imag;
        escape = ESCAPE_MAX;
    }
     
    return escape;
}

void main()
{
	int x, y, escape;
	float graph_x, graph_y;
	float t1;
	word start;
	
	set_mode(VGA_256_COLOR_MODE);

	start = *my_clock;

	for (y = 0; y <= SCREEN_VERT - 1; y++)
		{
			graph_y = pixel_to_graph(y, GRAPH_YMAX - GRAPH_YMIN, SCREEN_VERT, GRAPH_YMIN);
			for (x = 0; x <= SCREEN_HORIZ - 1; x++)
				{
					graph_x = pixel_to_graph(x, GRAPH_XMAX - GRAPH_XMIN, SCREEN_HORIZ, GRAPH_XMIN);
					escape = mandelbrot(graph_x, graph_y, ESCAPE_MAX);
					if (escape == ESCAPE_MAX)
						{
							plot_pixel_fast(x, y, 14);
						}
					else
						{
							//plot_pixel_fast(x, y, escape % 256);
						}
				}
		}

	t1 = (*my_clock - start) / 18.2;
	
	getchar();
	
	set_mode(TEXT_MODE);

	printf("Plotting took %f seconds.\n", t1);

	return;
}
