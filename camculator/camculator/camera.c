#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <gx.h>
#include "camera.h"

typedef struct stRGB
{
	union
	{
		unsigned short value;
		struct
		{
		unsigned char r : 5;
		unsigned char g : 6;
		unsigned char b : 5;
		};
	};
} stRGB;

void dc_camera(dc_t *target){
	int camera_dev;
	int i,j,k=0;
	stRGB rgbData[240*320];
	if((camera_dev = open("/dev/camera",O_RDWR))==-1){
		printf("Can't open dev_cad\n");
		exit(1);
	}
	gx_clear( target, gx_color( 0, 0, 0, 255));
	read(camera_dev,rgbData,153600);
	for (i = 0 ; i < 320 ; ++i)
	{
		for ( j = 0 ; j < 240 ; ++j)
		{
			gx_set_pixel( target, i, j,  gx_color(rgbData[k].r, rgbData[k].g, rgbData[k].b, 255));
			k++;
		}            
	}
	close(camera_dev);
}
