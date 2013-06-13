#pragma once

#include <sys/mman.h>
#include <sys/mount.h>

class IOutil
{
public:
	IOutil();
	~IOutil();
	
public:
	bool init();
	void closed();
	void LED_ON(unsigned char val);
	void LED_OFF();
	unsigned char SW_read();
	void Buzzer(int time);
	void textlcd(char* argv);
	void dotmatrix(int mode);
	void fnd(const char* str);
	void fnd_init(const char* str);
	void fnd_kill();

public:
	static int  fd_mem;
	static int  fd_textlcd;
	static int  fd_dot;
	static int  fd_fnd;
	unsigned char *pled;
	unsigned short *pdsw;
	unsigned char *pbzr;
	static char fndstr[7];
	pthread_t tid;

private :
	static void* t_fnd(void *param);
};

