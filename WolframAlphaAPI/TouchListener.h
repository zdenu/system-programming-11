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


typedef bool (*TOUCH_EVENT)(struct input_event& ev);

class TouchListener
{
public:
	TouchListener();
	~TouchListener();
	
public:
	bool initialize(void);
	bool destroy(void);
	
private:
	void* touchThread(Thread<TouchListener>*, void* );
	
	
	static bool handleEV_SYN(struct input_event& ev);
	static bool handleEV_KEY(struct input_event& ev);
	static bool handleEV_ABS(struct input_event& ev);
	
private:
	AtomicValue<bool>	 isThreadRunning;
	Thread<TouchListener> thread;
	
	TOUCH_EVENT	eventHandler[EV_MAX];
	
	
	int fd;
};