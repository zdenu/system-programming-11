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
#define ADDRESSOFBZ		0x14808000 // buzzer addr	


static int fd_mem;
unsigned char *pled;
unsigned char *pbz;

int initialize()
{
   if((fd_mem = open("/dev/mem",O_RDWR|O_SYNC)) < 0){
        perror("mem open fail\n");
		return -1;
    }        

   pled = mmap(NULL,1,PROT_WRITE, MAP_SHARED, fd_mem, ADDRESSOFLED);
	pbz = mmap(NULL, 1,PROT_WRITE,MAP_SHARED,fd_mem,ADDRESSOFBZ);

    if( (int)pled < 0 || (int)pbz<0){
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

void Buzzer(int time){
	*pbz = 0x01;
	usleep(time*1000);
	*pbz = 0x00;
}

void close(){
	munmap(pled,1);
	munmap(pbz,1);
	close(fd_mem);
}

int main(void) 
{ 
 initialize();

 Buzzer(100);
 LED_ON(0xFF);
 
 close();
 return 0; 
}
