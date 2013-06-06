

#include <sys/ioctl.h>
#include <sys/types.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "define.h"
#include "touch.h"

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
		printf("%s: open error", device);
		return false;
	}
	printf("open : [%s] \n", TOUCH_EVENT_DEVICE);
	
	// run thread.
	
	bool ret = touchThreadHandle.start(this, TouchHandler::touchThread, NULL);
	if (ret == false)
		return false;
	
	return true;
}

int TouchHandler::addTouchevent(int x, int y, int width, int height)
{
	for(int i = 0 ; i < MAX_TOUCH_EVENT ; ++i)
	{
		if(eventArray[i].enable == 0)
		{
			eventArray[i].x = x;
			eventArray[i].y = y;
			eventArray[i].w = width;
			eventArray[i].h = height;
			eventArray[i].enable = 1;
			
			if(touch_evnet_cnt < i + 1)
			{
				touch_evnet_cnt = i + 1;
			}
			return i;
		}
	}
}

void TouchHandler::removeTouchevent(int num)
{
	eventArray[num].enable = 1;
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
	for(int i = 0 ; i < touch_evnet_cnt ; ++i)
	{
		eventArray[i].enable = 1;
	}
	touch_evnet_cnt = 0;
}

void* TouchHandler::touchThread(Thread<TouchHandler>*, void* )
{
	size_t read_bytes;
	int bx = 0, by = 0;
	int x = 0, y = 0;
	int eventCnt = 0;
	struct input_event event_buf[EVENT_BUF_NUM];

	while (isRunning)
	{
		read_bytes = read(eventFd, event_buf, (sizeof(struct input_event)*EVENT_BUF_NUM) );
		if( read_bytes < sizeof(struct input_event) )
		{
			printf("%s: read error", device);
		}
		else
		{
			for (int i = 0 ; i < read_bytes/sizeof(struct input_event) ; ++i)
			{
				switch( event_buf[i].type )
				{
					case EV_SYN:
						break;
					case EV_ABS:
						switch (event_buf[i].code)
						{
						case ABS_X:
						{
							if(abs(event_buf[i].value - bx) > 30 )
							{
								x = (float)event_buf[i].value * 0.32;
								bx = event_buf[i].value;
								//x = x;
							}
							break;
						}
						case ABS_Y:
						{
							if(abs(event_buf[i].value - by) > 30)
							{
								y = (float)event_buf[i].value * 0.24;
								y = 240 - y;
								by=event_buf[i].value;
							}
							break;
						}
						case ABS_PRESSURE:
						{
							for(j = 0; j < touch_evnet_cnt; ++j)
							{
								if( (x > eventArray[j].x) &&
									(x < eventArray[j].x + eventArray[j].w) &&
									(y > eventArray[j].y) &&
									(y < eventArray[j].y + eventArray[j].h) )
								{
									printf("event occar! : %d\n", j);
									// TODO : Push to UI Thread.
									return j;
								}
							}
							break;
						}
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
}

int TouchHandler::touch()
{
	size_t read_bytes;
	int bx = 0,by = 0;
	int x = 0,y = 0;
	struct input_event event_buf[EVENT_BUF_NUM];
	// input device open
		while (1)
	{
		read_bytes = read(event_fd, event_buf, (sizeof(struct input_event)*EVENT_BUF_NUM) );
		if( read_bytes < sizeof(struct input_event) )
		{
			printf("%s: read error", device);
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
					if(abs(event_buf[i].value-bx) > 30 ) {
						x = (float)event_buf[i].value*0.32;
						bx = event_buf[i].value;
						//x = x;
					}
					break;
				case ABS_Y:
					if(abs(event_buf[i].value-by) > 30) {
						y = (float)event_buf[i].value*0.24;
						y = 240 - y;
						by=event_buf[i].value;
					}
					break;
				case ABS_PRESSURE:
					//printf("touch : %d, %d\n", x,y);
					for(j=0;j<touch_evnet_cnt;j++){
						//printf("%d~%d\n",eventArray[j].x,eventArray[j].x+eventArray[j].w);
						if(x > eventArray[j].x && x < eventArray[j].x+eventArray[j].w && y > eventArray[j].y && y < eventArray[j].y+eventArray[j].h)
						{
							printf("event occar! : %d\n", j);
							return j;
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

int TouchHandler::touch_pause()
{
	size_t read_bytes;
	struct input_event event_buf[EVENT_BUF_NUM];
	char *device;
	int event_fd = -1;
	int i;
	device = "/dev/input/event0";

	// input device open
	if ((event_fd = open(device, O_RDONLY)) < 0)
	{
		printf("%s: open error", device);
		exit(1);
	}
	printf("open : [%s] \n",device);   // raoudi
	while (1)
	{
		read_bytes = read(event_fd, event_buf, (sizeof(struct input_event)*EVENT_BUF_NUM) );
		if( read_bytes < sizeof(struct input_event) )
		{
			printf("%s: read error", device);
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
					break;
				case ABS_Y:
					break;
				case ABS_PRESSURE:
					return 1;
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


