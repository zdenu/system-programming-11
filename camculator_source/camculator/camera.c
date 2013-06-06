#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <gx.h>
#include "camera.h"

#pragma pack(1)
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
#pragma pack()
int camera_dev;

void init_camera(){
	if((camera_dev = open("/dev/camera",O_RDWR))==-1){
		printf("Can't open dev_cad\n");
		exit(1);
	}
		printf("open dev_camera\n");
}

void close_camera(){
	close(camera_dev);
}

int grayscale(stRGB input){
	return ((input.r + input.g + input.b)/3)*6.11;
}

void dc_camera(dc_t *target){
	int i=0;
	int tmp;
	stRGB rgbData[76800];
//	gx_clear( target, gx_color( 0, 0, 0, 255));
	read(camera_dev,rgbData,153600);
	for (i = 0 ;i<76800; i++) {
			tmp = grayscale(rgbData[i]);
			gx_set_pixel(target, i%320, i/320, gx_color( tmp,tmp,tmp, 255));
	} 
}



