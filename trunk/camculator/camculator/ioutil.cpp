#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "define.h"
#include "ioutil.h"
#include "camculator.h"

int		IOutil::fd_mem = INVALID_DATA;
int		IOutil::fd_textlcd = INVALID_DATA;
int		IOutil::fd_dot = INVALID_DATA;
int		IOutil::fd_fnd = INVALID_DATA;
char		IOutil::fndstr[7] = {"      "};

IOutil::IOutil()
{
}

IOutil::~IOutil()
{
}


bool IOutil::init()
{
	if((fd_mem = open("/dev/mem",O_RDWR|O_SYNC)) < 0){
        perror("Mem open fail\n");
		return false;
  	  }        
	printf("open : [/dev/mem] \n");

	fd_textlcd = open("/dev/textlcd",O_WRONLY);
	if(fd_textlcd < 0) {
			printf("Textlcd device open error\n");
			return false;
	}
	printf("open : [/dev/textlcd] \n");

	fd_dot = open("/dev/dotmatrix", O_WRONLY);
	if(fd_dot < 0)  {
			printf("Dotmatrix device open error\n");
  			return false;
	}

	fd_fnd = open("/dev/7segment", O_WRONLY);
	if(fd_fnd < 0)  {
			printf("7segment device open error\n");
  			return false;
	}

	 
   pled = (unsigned char*)mmap(NULL, 1, PROT_WRITE, MAP_SHARED,  fd_mem, ADDRESSOFLED);
	pdsw = (unsigned short*)mmap(NULL, 2, PROT_WRITE, MAP_SHARED,	fd_mem, ADDRESSOFSW);
	pbzr = (unsigned char*)mmap(NULL, 1, PROT_WRITE, MAP_SHARED,	fd_mem, ADDRESSOFBZ);

    if( (int)pled < 0 || (int)pdsw<0 || (int)pbzr<0){
		close(fd_mem);
      	perror("mmap error\n");
       	return -1;
    }
	 *pled = 0x00;
	 *pbzr  = 0x00;

	return true;
}

void IOutil::LED_ON(unsigned char val){
	*pled = val;
}

void IOutil::LED_OFF(){
	*pled = 0x00;
}

unsigned char IOutil::SW_read(){
	printf("Dip sw : %016X\n",*pdsw);
	return (*pdsw);
}

void IOutil::Buzzer(int time){
	*pbzr = 0x01;
	usleep(time*1000);
	*pbzr = 0x00;
}

void IOutil::textlcd(char* argv)
{
	write(fd_textlcd,argv,strlen(argv));
}

void IOutil::dotmatrix(int mode)
{
	write(fd_dot,&mode,1);
}

void IOutil::fnd(const char* str)
{
	strcpy(fndstr,str);
}

void IOutil::fnd_init(const char* str)
{
	printf("7 segment Thread is running...\n");
	 strcpy(fndstr,str);
	 pthread_create(&tid, NULL, &IOutil::t_fnd, NULL);
}

void IOutil::fnd_kill()
{
	pthread_cancel(tid);
}

void* IOutil::t_fnd(void *pram)
{
	while(true){
		write(fd_fnd, fndstr ,6);
		usleep(10000);
	}
}

void IOutil::closed(){
	munmap(pled,1);
	munmap(pbzr,1);
	munmap(pdsw,2);
	close(fd_mem);
	close(fd_textlcd);
	close(fd_dot);
	close(fd_fnd);
}



