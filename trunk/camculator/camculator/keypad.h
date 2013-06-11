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
	void keysignal(int sig);
	
private:

private:
	int		eventFd;
		
};

