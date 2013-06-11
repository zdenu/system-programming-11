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
, pressed(0)
, touch_before(NULL)
{
}

TouchHandler::~TouchHandler()
{
}

bool TouchHandler::init(dc_t* dc_screen)
{
	if ((eventFd = open(TOUCH_EVENT_DEVICE, O_RDONLY)) < 0)
		return false;

	printf("open : [%s] \n", TOUCH_EVENT_DEVICE);
	
	// run thread.
	
	isRunning = true;
	
	bool ret = touchThreadHandle.start(this, &TouchHandler::touchThread, NULL);
	if (ret == false)
	{
		printf("Touch thread start false.\n");
		return false;
	}
		
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
	eventArray[evType].enable = 0;
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

void TouchHandler::removeAllTouchevent()
{
	memset(eventArray, 0x00, sizeof(eventArray));
}

void* TouchHandler::touchThread(Thread<TouchHandler>*, void* )
{
	size_t read_bytes;
	int x = 0, y = 0;
	int bx = 0, by = 0;
	struct input_event event_buf[EVENT_BUF_NUM];
	
	printf("Touch thread start.\n");

	while (isRunning)
	{
		read_bytes = read(eventFd,
						  event_buf,
						  (sizeof(struct input_event) * EVENT_BUF_NUM));
		
		if( read_bytes < sizeof(struct input_event) )
		{
			printf("%s: read error", TOUCH_EVENT_DEVICE);
		}
		else
		{
			for (int i = 0 ; i < (read_bytes/sizeof(struct input_event)) ; ++i)
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
							if(event_buf[i].value == 0) { // Release
//								gx_bitblt(Camculator::get().getScreen(), 0, 0, touch_before,0,0,320,240);
								beforeEvent=-1;
								pressed = 0;
								for(int j=0;j<TOUCH_EVENT_MAX;j++){
									if(eventArray[j].enable == 1 && x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
									{
										printf("Touch Release event : %d\n", j);
										//gx_bitblt(dc_screen ,eventArray[j].x, eventArray[j].y, ( dc_t *)touch_before, 0, 0, eventArray[j].w, eventArray[j].h);
										stEvent* pEv = new stEvent;
										stTouchData* pTouchData = new stTouchData;
										pEv->eventType = EVENT_TYPE_TOUCH_PAD;
										pTouchData->x = x;
										pTouchData->y = y;
										pTouchData->touchType = j;
										
										pEv->pData = pTouchData;
																				
										Camculator::get().pushEvent(pEv);

									}
								}
								
							} else if(event_buf[i].value > 50){
								for(int j=0;j<TOUCH_EVENT_MAX;j++){
									if(pressed != 1){
										pressed = 1;
//										gx_bitblt(touch_before,0,0,Camculator::get().getScreen(),0,0,320,240);
									}
									if(eventArray[j].enable == 1 && x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
									{
										if(beforeEvent!=j) {
//											gx_bitblt(Camculator::get().getScreen(),0,0,touch_before,0,0,320,240);
//											printf("Touch Press event : %d\n", j);
//											dc_t* active = gx_png_create( eventArray[j].w, eventArray[j].h);
//											gx_clear( ( dc_t *)active, gx_color( 0, 0, 0, 80));
//											gx_bitblt( Camculator::get().getScreen(), eventArray[j].x, eventArray[j].y, ( dc_t *)active, 0, 0, active->width, active->height);
//											gx_png_close( active);
//											beforeEvent=j;
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
		
		sleep(0);
	}
	
	return NULL;
}




