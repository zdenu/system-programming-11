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


class TouchListener
{
public:
	TouchListener();
	~TouchListener();
	
public:
	bool initialize(void);
	
	
private:
	void* touchThread(Thread<TouchListener>*, void* );
	
private:
	AtomicValue<bool>	 isThreadRunning;
	Thread<TouchListener> thread;
};