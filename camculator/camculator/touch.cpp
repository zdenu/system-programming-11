

#include <sys/ioctl.h>
#include <sys/types.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "define.h"
#include "touch.h"
#include "camculator.h"

TouchHandler::TouchHandler()
: touch_evnet_cnt(0)
, isRunning(false)
, eventFd(INVALID_DATA)
{
}

TouchHandler::~TouchHandler()
{
}

bool TouchHandler::init(void)
{
	if ((eventFd = open(TOUCH_EVENT_DEVICE, O_RDONLY)) < 0)
	{
		printf("%s: open error", TOUCH_EVENT_DEVICE);
		return false;
	}
	printf("open : [%s] \n", TOUCH_EVENT_DEVICE);
	
	// run thread.
	
	bool ret = touchThreadHandle.start(this, &TouchHandler::touchThread, NULL);
	if (ret == false)
		return false;
	
	return true;
}

int TouchHandler::addTouchevent(int x, int y, int width, int height, int evType)
{
	if (evType < TOUCH_EVENT_MAIN_HOME || evType >= TOUCH_EVENT_MAX)
		return INVALID_DATA;
	
	
	eventArray[evType].x = x;
	eventArray[evType].y = y;
	eventArray[evType].w = width;
	eventArray[evType].h = height;
	eventArray[evType].enable = 1;
	touch_evnet_cnt++;

	return evType;
}
int TouchHandler::disableTouchEvent(int evType)
{
	if (evType < TOUCH_EVENT_MAIN_HOME || evType >= TOUCH_EVENT_MAX)
		return INVALID_DATA;
	
	eventArray[evType].enable = 0;
}

int TouchHandler::enableTouchEvent(int evType)
{
	if (evType < TOUCH_EVENT_MAIN_HOME || evType >= TOUCH_EVENT_MAX)
		return INVALID_DATA;
	
	eventArray[evType].enable = 1;
}

void TouchHandler::pauseTouchevent()
{
	int i;
	beforeEvent=-1;
	for(i=0;i<touch_evnet_cnt ;i++) {
		if(eventArray[i].enable > 0)
			eventArray[i].enable++;
	}
}

void TouchHandler::resumeTouchevent()
{
	int i;
	for(i=0;i<touch_evnet_cnt ;i++) {
		if(eventArray[i].enable > 0)
			eventArray[i].enable--;
	}
}


void TouchHandler::removeAllTouchevent()
{
	memset(eventArray, 0x00, sizeof(eventArray));
}

void* TouchHandler::touchThread(Thread<TouchHandler>*, void* )
{
	size_t read_bytes;
	int bx = 0, by = 0;
	int x = 0, y = 0;
	int eventCnt = 0;
	INPUT_EVENT buf[EVENT_BUF_NUM];

	while (isRunning)
	{
		read_bytes = read(eventFd, buf, (sizeof(INPUT_EVENT) * EVENT_BUF_NUM));
		if( read_bytes < sizeof(INPUT_EVENT) )
		{
			printf("%s: read error", TOUCH_EVENT_DEVICE);
		}
		else
		{
			for (int i = 0 ; i < read_bytes/sizeof(INPUT_EVENT) ; ++i)
			{
				dispatchTouchEvent(buf[i]);
			}
		}
	}
}

bool TouchHandler::dispatchTouchEvent(INPUT_EVENT& ev)
{
	int bx;
	switch( ev.type )
	{
		case EV_ABS:
		{
			switch( ev.code )
			{
				case ABS_X:
				{
					if (abs(ev.value - beforeX) > 10)
					{
						currentX = (float)ev.value * 0.32;
						beforeX = ev.value;
					}
					break;
				}
				case ABS_Y:
				{
					if (abs(ev.value - beforeY) > 10)
					{
						currentY = (float)ev.value * 0.24;
						currentY -= 240;
						beforeY = ev.value;
					}
					break;
				}
				case ABS_PRESSURE:
				{
					if (ev.value == 0)
					{
						// Release.
						Camculator::get().drawBeforeScreen();
						beforeEvent = INVALID_DATA;
						pressed = 0;
						for( int j = 0; j < TOUCH_EVENT_MAX ; ++j)
						{
							if((eventArray[j].enable == 1) &&
							   (currentX > eventArray[j].x) &&
							   (currentX < eventArray[j].x + eventArray[j].w) &&
							   (currentY > eventArray[j].y) &&
							   (currentY < eventArray[j].y + eventArray[j].h))
							{
								printf("Touch Release event : %d\n", j);
								
								stEvent* pEv = new stEvent;
								pEv->eventType = EVENT_TYPE_TOUCH_PAD;
								char* buffer = new char[sizeof(int)];
								memcpy(buffer, &j, sizeof(int));
								pEv->pData = buffer;
								
								Camculator::get().pushEvent(pEv);
							}
						}
					}
					else if (ev.value > 50)
					{
						for(int j = 0; j < TOUCH_EVENT_MAX; ++j)
						{
							if(preesed != 1)
							{
								preesed = 1;
								Camculator::get().drawBeforeScreen();
							}
							if((eventArray[j].enable == 1) &&
							   (currentX > eventArray[j].x) &&
							   (currentX < eventArray[j].x + eventArray[j].w) &&
							   (currentY > eventArray[j].y) &&
							   (currentY < eventArray[j].y + eventArray[j].h))
							{
								if(beforeEvent != j)
								{
									printf("Touch Press event : %d\n", j);
									beforeEvent = j;
									
									Camculator::get().drawBeforeScreen();
									Camculator::get().drawPartScreen(eventArray[j].x,
																	 eventArray[j].y,
																	 eventArray[j].w,
																	 eventArray[j].h,
																	 gx_color(0, 0, 0, 80));
								}
							}
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case EV_SYN:
			break;
		default:
			break;
	}
}

int TouchHandler::touch(dc_t *dc_screen)
{
//	int i,j;
//	size_t read_bytes;
//	int bx = 0, by = 0;
//	int x=0,y=0;
//	struct input_event event_buf[EVENT_BUF_NUM];
//	png_t   *active;
//	
//	while (1)
//	{
//		read_bytes = read(eventFd, event_buf, (sizeof(struct input_event)*EVENT_BUF_NUM) );
//		if( read_bytes < sizeof(struct input_event) )
//		{
//			printf("read error");
//			exit(1);
//		}
//		for( i=0; i<(read_bytes/sizeof(struct input_event)); i++ )
//		{
//			switch( event_buf[i].type )
//			{
//				case EV_SYN:
//					break;
//				case EV_ABS:
//					switch (event_buf[i].code)
//				{
//					case ABS_X:
//						if(abs(event_buf[i].value-bx) > 10 ) {
//							x = (float)event_buf[i].value*0.32;
//							bx = event_buf[i].value;
//							//x = x;
//						}
//						break;
//					case ABS_Y:
//						if(abs(event_buf[i].value-by) > 10) {
//							y = (float)event_buf[i].value*0.24;
//							y = 240 - y;
//							by=event_buf[i].value;
//						}
//						break;
//					case ABS_PRESSURE:
//						//printf("touch : %d, %d\n", x,y);
//						if(event_buf[i].value == 0) { // Release
//							gx_bitblt(dc_screen,0,0,touch_before,0,0,320,240);
//							//gx_to_screen_dc(dc_screen,touch_before);
//							beforeEvent=-1;
//							preesed = 0;
//							for(j=0;j<touch_evnet_cnt;j++){
//								if(eventArray[j].enable == 1 && x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
//								{
//									printf("Touch Release event : %d\n", j);
//									//gx_bitblt(dc_screen ,eventArray[j].x, eventArray[j].y, ( dc_t *)touch_before, 0, 0, eventArray[j].w, eventArray[j].h);
//									return j;
//								}
//							}
//							
//						} else if(event_buf[i].value > 50){
//							for(j=0;j<touch_evnet_cnt;j++){
//								if(preesed != 1){
//									preesed = 1;
//									gx_bitblt(touch_before,0,0,dc_screen,0,0,320,240);
//								}
//								if(eventArray[j].enable == 1 && x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
//								{
//									if(beforeEvent!=j) {
//										gx_bitblt(dc_screen,0,0,touch_before,0,0,320,240);
//										printf("Touch Press event : %d\n", j);
//										active = (png_t*)gx_png_create( eventArray[j].w, eventArray[j].h);
//										gx_clear( ( dc_t *)active, gx_color( 0, 0, 0, 80));
//										gx_bitblt( dc_screen, eventArray[j].x, eventArray[j].y, ( dc_t *)active, 0, 0, active->width, active->height);
//										gx_png_close((dc_t*)active);
//										beforeEvent=j;
//									}
//									//
//								}
//							}
//						}
//						break;
//					default:
//						printf("Touch Unknown: type %d, code %d, value %d\n",
//							   event_buf[i].type,
//							   event_buf[i].code,
//							   event_buf[i].value);
//						break;
//				}
//					break;
//				default:
//					printf("Unknown: type %d, code %d, value %d\n",
//						   event_buf[i].type,
//						   event_buf[i].code,
//						   event_buf[i].value);
//					break;
//			}
//			
//		}
//	}
	
}



