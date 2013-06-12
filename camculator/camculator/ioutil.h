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
	void fnd(char* str);

public:
	static int  fd_mem;
	static int  fd_textlcd;
	static int  fd_dot;
	static int  fd_fnd;
	unsigned char *pled;
	unsigned short *pdsw;
	unsigned char *pbzr;
};

