#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <termios.h>
#include <asm/fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>


#define ADDRESSOFLED		0x14805000	// led addr.
#define ADDRESSOFSW		0x14802000	//  sw addr. 3bit
#define ADDRESSOFBZ		0x14808000 // buzzer addr	


static int fd_mem;
unsigned char *pled;
unsigned char *psw;
unsigned char *pbz;

int initialize()
{
   if((fd_mem = open("/dev/mem",O_RDWR|O_SYNC)) < 0){
        perror("mem open fail\n");
		return -1;
    }        

    pled = mmap(NULL,1,PROT_WRITE, MAP_SHARED, fd_mem, ADDRESSOFLED);
	psw = mmap(NULL, 1,PROT_WRITE,MAP_SHARED,fd_mem,ADDRESSOFSW);
	pbz = mmap(NULL, 1,PROT_WRITE,MAP_SHARED,fd_mem,ADDRESSOFBZ);

    if( (int)pled < 0 || (int)psw<0 || (int)pbz<0){
		close(fd_mem);
      	perror("mmap error\n");
       	return -1;
    }
	 *pled = 0x00;
	 *pbz  = 0x00;
}

void LED_ON(unsigned char val){
	*pled = val;
}

void LED_OFF(unsigned char val){
	*pled = 0x00;
}

void LED_close(){
	munmap(pled,1);
}

unsigned char SW_read(){
	return (*psw);
}

void SW_close(){
	munmap(psw,1);
}

void Buzzer(int time){
	*pbz = 0x01;
	usleep(time*1000);
	*pbz = 0x00;
}

void BZ_close(){
	munmap(pbz,1);
}

void MEM_close(){
	close(fd_mem);
}

static int dev; 
int main(void) 
{ 
 initialize();

 Buzzer(100);
 LED_ON(0xFF);

 while(1) {
		printf("%d %d %d\n",SW_read()&0x01,(SW_read()&0x02)>>1,(SW_read()&0x04)>>2);
		LED_ON(SW_read());
  }
 BZ_close();
 MEM_close();
 return 0; 
}
