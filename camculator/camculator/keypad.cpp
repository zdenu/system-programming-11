#include <sys/ioctl.h>
#include <sys/types.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "define.h"
#include "keypad.h"
#include "camculator.h"

KeyHandler::KeyHandler()
	: key_evnet_cnt(0)
	, eventFd(INVALID_DATA)
{
}

KeyHandler::~KeyHandler()
{
}

bool KeyHandler::init()
{
	if((eventFd = open(KEY_EVENT_DEVICE, O_RDONLY)) < 0) {
			return false;
	 }
	printf("open : [%s] \n", KEY_EVENT_DEVICE);

	(void)signal(SIGUSR1, keysignal);
		
	return true;
}

void KeyHandler::keysignal(int sig)
	size_t read_bytes;
	static unsigned char vkey;
   read_bytes = read(eventFd,&vkey,1);

	stEvent* pEv = new stEvent;
	stKeyData* pKeyData = new stKeyData;
	pEv->eventType = EVENT_TYPE_KEY_PAD;
	pTouchData->key = vkey;
										
	pEv->pData = pKeyData;																
	Camculator::get().pushEvent(pEv);
}

void KeyHandler::close()
	close(fd_push);    
}




