//
//  Camculator.cpp
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 6/5/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//
#include "Define.h"
#include "Camculator.h"

template<> Camculator* Singleton<Camculator>::ms_Singleton = 0;

Camculator::Camculator(void)
: isRunning(false)
{
}

Camculator::~Camculator(void)
{
}

bool Camculator::init(void)
{
	// create http request thread.
	
	return true;
}
bool Camculator::start(void)
{
	while (isRunning)
	{
		
		sleep(0);
	}

	return true;
}

bool Camculator::destroy(void)
{
	return true;
}

void Camculator::pushEvent(stEvent& ev)
{
	queueLock.lock();
	eventQueue.push(ev);
	queueLock.unlock();
}
stEvent Camculator::popEvent(void)
{
	stEvent ev;
	queueLock.lock();
	if (!eventQueue.empty())
	{
		ev = eventQueue.front();
		eventQueue.pop();
	}
	queueLock.unlock();
	
	return ev;
}


