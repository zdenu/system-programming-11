#pragma once

#include <signal.h>

#include "gx.h"

class KeyHandler
{
public:
	KeyHandler();
	~KeyHandler();
	
public:
	bool init();
	void close();
	static void keysignal(int sig);
	static void swsignal(int sig);
	
public:
	static int	eventFd;
	static int 	sweventFd;
};

