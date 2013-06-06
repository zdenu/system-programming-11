//
//  Camculator.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 6/5/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __WolframAlphaAPI__Camculator__
#define __WolframAlphaAPI__Camculator__

#include <iostream>
#include <queue>
#include "singleton.h"
#include "Define.h"
#include "lock.h"
#include "thread.h"

class Camculator : public Singleton<Camculator>
{
public:
	Camculator(void);
	~Camculator(void);
	
public:
	bool init(void);
	bool start(void);
	
	bool destroy(void);
	
	void pushEvent(stEvent& ev);
	stEvent popEvent(void);
	
private:
	void* httpThread(Thread<Camculator>*, void* );
	
private:
	bool isRunning;
	
	Lock queueLock;
	std::queue<stEvent> eventQueue;
	
	Thread<Camculator> httpThreadHandler;
};

#endif /* defined(__WolframAlphaAPI__Camculator__) */
