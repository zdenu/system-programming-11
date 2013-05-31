//
//  TouchListener.cpp
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/31/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "TouchListener.h"

#define EVENT_BUF_NUM 64
int event_fd = -1; /* the file descriptor for the device */


TouchListener::TouchListener()
{
	isThreadRunning.set(true);
}

TouchListener::~TouchListener()
{
}

bool TouchListener::initialize(void)
{
	const char* device = "/dev/input/event0";
	if (fd = fopen(device, O_RDONLY) < 0)
	{
		return false;
	}
	
	bool ret = thread.start(this, &TouchListener::touchThread, NULL);
	if (ret == false)
	{
		return false;
	}
	
	eventHandler[EV_SYN] = &TouchListener::handleEV_SYN;
	eventHandler[EV_KEY] = &TouchListener::handleEV_KEY;
	eventHandler[EV_ABS] = &TouchListener::handleEV_ABS;
	
	return true;
	
}

bool TouchListener::destroy(void)
{
	isThreadRunning.set(false);
	close(fd);
	fd = INVALID_SOCKET;
}

void* TouchListener::touchThread(Thread<TouchListener>*, void* )
{
	size_t ret = 0;
	struct input_event buf[EVENT_BUF_NUM];
	size_t stSize = sizeof(struct input_event);
	
	while (isThreadRunning.get())
	{
		ret = read(fd, buf, (stSize * EVENT_BUF_NUM) );
		if( read_bytes < stSize )
		{
			printf("%s: read error", device);
		}
		else
		{
			int eventCnt = ret / stSize;
			for (int i = 0 ; i < eventCnt ; ++i)
			{
				
				bool res = eventHandler[buf[i].type](buf[i]);
				if (res == false)
				{
					// process error.
				}
			}
		}
		sleep(0);
	}
}

bool TouchListener::handleEV_SYN(struct input_event& ev)
{
	printf("---------------------------------------\n");
	return true;
}
bool TouchListener::handleEV_KEY(struct input_event& ev)
{
	printf("Button code %d", buf[i].code);
	switch (buf[i].value)
	{
		case 1:
			printf(": pressed\n");
			break;
		case 0:
			printf(": released\n");
			break;
		default:
			printf("Unknown: type %d, code %d, value %d",
				   buf[i].type,
				   buf[i].code,
				   buf[i].value);
			break;
	}
	
	return true;
}
bool TouchListener::handleEV_ABS(struct input_event& ev)
{
	switch (buf[i].code)
	{
		case ABS_X:
			printf("X position: %d\n", buf[i].value);
			break;
		case ABS_Y:
			printf("Y position: %d\n", buf[i].value);
			break;
		case ABS_PRESSURE:
			printf("Pressure : %s\n", (buf[i].value > 1)? "yes" : "no" );
			break;
		default:
			printf("Touch Unknown: type %d, code %d, value %d\n",
				   buf[i].type,
				   buf[i].code,
				   buf[i].value);
			break;
	}

	return true;
}

