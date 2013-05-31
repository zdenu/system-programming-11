//
//  TouchListener.cpp
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/31/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "TouchListener.h"

TouchListener::TouchListener()
{
	isThreadRunning.set(true);
}

TouchListener::~TouchListener()
{
}

bool TouchListener::initialize(void)
{
	
}

void* TouchListener::touchThread(Thread<TouchListener>*, void* )
{
	while (isThreadRunning.get())
	{
		
	}
}

