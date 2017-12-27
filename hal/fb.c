/*
 * fb.c
 *
 *  Created on: Dec 26, 2017
 *      Author: sid
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cairo/cairo.h>

#include "../snes2010/snes9x.h"
#include "../snes2010/cpuexec.h"

int fb_fd = 0;
uint8_t *fb_p = 0;
struct fb_var_screeninfo fb_vinfo;
struct fb_fix_screeninfo fb_finfo;

void fb_init(void)
{


	// Open the file for reading and writing
	fb_fd = open("/dev/fb0", O_RDWR);
	if (fb_fd == -1)
	{
		perror("Error: cannot open framebuffer device");
		exit(1);
	}

	// Get fixed screen information
	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_finfo) == -1)
	{
		perror("Error reading fixed information");
		exit(2);
	}

	// Get variable screen information
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_vinfo) == -1)
	{
		perror("Error reading variable information");
		exit(3);
	}

	printf("Framebuffer: %dx%d, %dbpp\n", fb_vinfo.xres, fb_vinfo.yres, fb_vinfo.bits_per_pixel);

	// Figure out the size of the screen in bytes
	int screensize = fb_vinfo.xres * fb_vinfo.yres * fb_vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fb_p = (uint8_t *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if ((int) fb_p == -1)
	{
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
}

void fb_deinit(void)
{
	int screensize = fb_vinfo.xres * fb_vinfo.yres * fb_vinfo.bits_per_pixel / 8;
	munmap(fb_p, screensize);
	close(fb_fd);
}


/* screen */
void S9xDeinitUpdate(int width, int height)
{
#if 0
	static bool skip = false;
	if(skip)
	{
		skip = false;
		return;
	}
	skip = true;
#endif
	/* limit */
	int line_length = width*sizeof(uint16_t);
	if(line_length > fb_vinfo.xres*sizeof(uint16_t))
		line_length = fb_vinfo.xres;
	if(height > fb_vinfo.yres)
		height = fb_vinfo.yres;
	int gfx_pitch = GFX.Pitch/2;

	/* centering */
	int offset_x = 0, offset_y = 0;
	if(width < fb_vinfo.xres && height < fb_vinfo.yres)
	{
		offset_x = (fb_vinfo.xres-width)/2;
		offset_y = (fb_vinfo.yres-height)/2;
	}

	/* copy data into fb */
	for(int h=0; h<height; h++)
	{
		int location = (fb_vinfo.xoffset+offset_x) * (fb_vinfo.bits_per_pixel/8) +
				(h+fb_vinfo.yoffset+offset_y) * fb_finfo.line_length;
		memcpy(fb_p + location, &GFX.Screen[h*gfx_pitch], line_length);
	}
}
