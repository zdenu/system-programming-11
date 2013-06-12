#include <sys/ioctl.h>
#include <sys/types.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "define.h"
#include "keypad.h"
#include "camculator.h"

int	KeyHandler::eventFd = INVALID_DATA;
int KeyHandler::sweventFd = INVALID_DATA;

KeyHandler::KeyHandler()
{
}

KeyHandler::~KeyHandler()
{
}

bool KeyHandler::init()
{
	if((eventFd = open(KEY_EVENT_DEVICE, O_RDWR)) < 0) {
			return false;
	 }
	printf("open : [%s] \n", KEY_EVENT_DEVICE);
	
	if((sweventFd = open(SW_EVENT_DEVICE, O_RDWR)) < 0) {
			return false;
	 }
	printf("open : [%s] \n", SW_EVENT_DEVICE);


	signal(SIGUSR1, &KeyHandler::keysignal);
	signal(SIGUSR2, &KeyHandler::swsignal);
	
	pid_t pId = getpid();
	write(eventFd, &pId, sizeof(pid_t));
	write(sweventFd, &pId, sizeof(pid_t));

	return true;
}

void KeyHandler::keysignal(int sig)
{
	printf("KeySignal CALLED!!!!!!!!!!!!!(%d)\n", sig);
	size_t read_bytes;
	static unsigned char vkey;
   read_bytes = read(eventFd, &vkey, 1);

	stEvent* pEv = new stEvent;
	stKeyData* pKeyData = new stKeyData;
	pEv->eventType = EVENT_TYPE_KEY_PAD;
	pKeyData->key = vkey;
										
	pEv->pData = pKeyData;																
	Camculator::get().pushEvent(pEv);
}


void KeyHandler::swsignal(int sig)
{
	printf("SWSignal CALLED!!!!!!!!!!!!!(%d)\n", sig);
	size_t read_bytes;
	static unsigned char vSw;
   read_bytes = read(sweventFd, &vSw, 1);

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

void KeyHandler::close(void)
{
	::close(eventFd);
}




