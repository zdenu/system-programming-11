#pragma once

#include "thread.h"

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
	bool init(void);
	int addTouchevent(int x, int y, int width, int height);
	
	void removeTouchevent(int num);
	int touch();
	void removeAllTouchevent();
	int touch_pause();
	
private:
	void* touchThread(Thread<TouchHandler>*, void* );
	
private:
	touchEventArea eventArray[MAX_TOUCH_EVENT];
	int touch_evnet_cnt;
	
	Thread<TouchHandler> touchThreadHandle;
	
	bool	isRunning;
	int		eventFd;
	bool	isPressed;

};
