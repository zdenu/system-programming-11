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
	
	if((sweventFd = open(SW_EVENT_DEVICE, O_RDONLY)) < 0) {
			return false;
	 }
	printf("open : [%s] \n", SW_EVENT_DEVICE);

	(void)signal(SIGUSR1, this->keysignal);
	(void)signal(SIGUSR2, this->swsignal);
	return true;
}

void KeyHandler::keysignal(int sig)
	size_t read_bytes;
	static unsigned char vkey;
   read_bytes = read(eventFd,&vkey,1);

	stEvent* pEv = new stEvent;
	stKeyData* pKeyData = new stKeyData;
	pEv->eventType = EVENT_TYPE_KEY_PAD;
	pKeyData->key = vkey;
										
	pEv->pData = pKeyData;																
	Camculator::get().pushEvent(pEv);
}
void KeyHandler::swsignal(int sig)
	size_t read_bytes;
	static unsigned char vSw;
   read_bytes = read(sweventFd,&vSw,1);

	stEvent* pEv = new stEvent;
	stKeyData* pKeyData = new stKeyData;
	pEv->eventType = EVENT_TYPE_KEY_PAD;
	if( vSw&0x01 == 0 ){
		pKeyData->key = 17;
	}	
	else if( (vSw&0x02)>>1 == 0 ){
		pKeyData->key = 18;
	}			
	else if( (vSw&0x04)>>2 == 0 ){
		pKeyData->key = 19;
	}									
	pEv->pData = pKeyData;																
	Camculator::get().pushEvent(pEv);
}

void KeyHandler::close()
	close(fd_push);    
}




