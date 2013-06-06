#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <gx.h>
#include <gxpng.h>
#include "touch.h"

typedef struct touchEventArea
{
	int x;
	int y;
	int w;
	int h;
	int enable;
} touchEventArea;

touchEventArea eventArray[MAX_TOUCH_EVENT];
int touch_evnet_cnt = 0;
int event_fd = -1;
int bx=0,by=0;
int beforeEvent=-1;
dc_t *touch_before;
int preesed=0;

void initTouchevent(char *device ,dc_t *dc_screen){
	int i;
	for(i=0;i<MAX_TOUCH_EVENT;i++)
			eventArray[i].enable == 0;

	if ((event_fd = open("/dev/input/event0", O_RDONLY)) < 0)
	{
		printf("%s: open error", "/dev/input/event0");
		exit(1);
	}
	printf("open : [%s] \n","/dev/input/event0");   // raoudi

	if  ( NULL == ( touch_before
		= gx_get_compatible_dc( dc_screen) ))   return; 

}
void closeTouchevent(){
	close(event_fd);
}
void pauseTouchevent() {
	int i;
	beforeEvent=-1;
	for(i=0;i<touch_evnet_cnt ;i++) {
		if(eventArray[i].enable > 0)
			eventArray[i].enable++;
	}
}

void resumeTouchevent() {
	int i;
	for(i=0;i<touch_evnet_cnt ;i++) {
		if(eventArray[i].enable > 0)
			eventArray[i].enable--;
	}
}

int addTouchevent(int x, int y, int width, int height){
	int i=0;
	for(i=0;i<MAX_TOUCH_EVENT;i++){
		if(eventArray[i].enable == 0) {
			eventArray[i].x = x;
			eventArray[i].y = y;
			eventArray[i].w = width;
			eventArray[i].h = height;
			eventArray[i].enable = 1;
			if(touch_evnet_cnt<i+1){
				touch_evnet_cnt = i+1;
			}
			return i;
		}
	}
}

void removeTouchevent(int num){
	eventArray[num].enable = 0;
}

void removeAllTouchevent(){
	int i;	
	for(i=0;i<touch_evnet_cnt;i++){
		eventArray[i].enable = 0;
	}
	touch_evnet_cnt = 0;
}

int touch(dc_t *dc_screen){
	int i,j;
	size_t read_bytes;
	int x=0,y=0;
	struct input_event event_buf[EVENT_BUF_NUM];
	png_t   *active;
	


	while (1)
	{
		read_bytes = read(event_fd, event_buf, (sizeof(struct input_event)*EVENT_BUF_NUM) );
		if( read_bytes < sizeof(struct input_event) )
		{
			printf("read error");
			exit(1);
		}
		for( i=0; i<(read_bytes/sizeof(struct input_event)); i++ )
		{
			switch( event_buf[i].type )
			{
			case EV_SYN:
				break;
			case EV_ABS:
				switch (event_buf[i].code)
				{
				case ABS_X:
					if(abs(event_buf[i].value-bx) > 10 ) {
						x = (float)event_buf[i].value*0.32;
						bx = event_buf[i].value;
						//x = x;
					}
					break;
				case ABS_Y:
					if(abs(event_buf[i].value-by) > 10) {
						y = (float)event_buf[i].value*0.24;
						y = 240 - y;
						by=event_buf[i].value;
					}
					break;
				case ABS_PRESSURE:
					//printf("touch : %d, %d\n", x,y);
					if(event_buf[i].value == 0) { // Release
						gx_bitblt(dc_screen,0,0,touch_before,0,0,320,240);
						//gx_to_screen_dc(dc_screen,touch_before);	
						beforeEvent=-1;
						preesed = 0;
						for(j=0;j<touch_evnet_cnt;j++){
							if(eventArray[j].enable == 1 && x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
							{
								printf("Touch Release event : %d\n", j);
								//gx_bitblt(dc_screen ,eventArray[j].x, eventArray[j].y, ( dc_t *)touch_before, 0, 0, eventArray[j].w, eventArray[j].h); 
								return j;
							}
						}
					
					} else if(event_buf[i].value > 50){
						for(j=0;j<touch_evnet_cnt;j++){
							if(preesed != 1){
								preesed = 1;
								gx_bitblt(touch_before,0,0,dc_screen,0,0,320,240);						
							}
							if(eventArray[j].enable == 1 && x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
							{
								if(beforeEvent!=j) {
								gx_bitblt(dc_screen,0,0,touch_before,0,0,320,240);
								printf("Touch Press event : %d\n", j);												
								active = gx_png_create( eventArray[j].w, eventArray[j].h);
								gx_clear( ( dc_t *)active, gx_color( 0, 0, 0, 80));
								gx_bitblt( dc_screen, eventArray[j].x, eventArray[j].y, ( dc_t *)active, 0, 0, active->width, active->height);
								gx_png_close( active);
								beforeEvent=j;
								}
								//
							} 
						}
					}
					break;
				default:
					printf("Touch Unknown: type %d, code %d, value %d\n",
							event_buf[i].type,
							event_buf[i].code,
							event_buf[i].value);
					break;
				}
				break;
			default:
				printf("Unknown: type %d, code %d, value %d\n",
						event_buf[i].type,
						event_buf[i].code,
						event_buf[i].value);
				break;
			}

		}
	}

}
