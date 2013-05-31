//
//  TouchListener.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/31/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//
#pragma once

#include "atomic_value.h"
#include "thread.h"

#define INVALID_SOCKET	-1

//#define EV_SYN 0
//#define EV_KEY 1
//#define EV_ABS 4
//#define EV_MAX 16


typedef bool (*TOUCH_EVENT)(struct input_event& ev);

class TouchListener
{
public:
	TouchListener();
	~TouchListener();
	
public:
	bool initialize(void);
	bool destroy(void);
	
	static bool handleEV_SYN(struct input_event& ev);
	static bool handleEV_KEY(struct input_event& ev);
	static bool handleEV_ABS(struct input_event& ev);
	
private:
	void* touchThread(Thread<TouchListener>*, void* );
	
	
	
private:
	AtomicValue<bool>	 isThreadRunning;
	Thread<TouchListener> thread;
	
	TOUCH_EVENT	eventHandler[EV_MAX];

	int fd;
};