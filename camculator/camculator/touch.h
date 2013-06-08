#pragma once

#include <linux/input.h>

#include "thread.h"
#include "gx.h"


//typedef struct input_event INPUT_EVENT;

typedef struct touchEventArea
{
	touchEventArea() : x(0), y(0), w(0), h(0), enable(0){}
	int x;
	int y;
	int w;
	int h;
	int enable;
} touchEventArea;

class TouchHandler
{
public:
	TouchHandler();
	~TouchHandler();
	
public:
	bool init(dc_t* dc_screen);
	int addTouchevent(int x, int y, int width, int height, int evType);
	int disableTouchEvent(int evType);
	int enableTouchEvent(int evType);
	
	void pauseTouchevent(void);
	void resumeTouchevent(void);
	
	
//	int touch(dc_t *dc_screen);
	void removeAllTouchevent();
	int touch_pause();
	
private:
	void* touchThread(Thread<TouchHandler>*, void* );
	
	bool dispatchTouchEvent(struct input_event& ev);
	
private:
	touchEventArea eventArray[TOUCH_EVENT_MAX];
	int touch_evnet_cnt;
	
	Thread<TouchHandler> touchThreadHandle;
	
	bool	isRunning;
	int		eventFd;
	int		pressed;
	
	int		beforeEvent;
	dc_t	*touch_before;
		
};

