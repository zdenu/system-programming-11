#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "define.h"
#include "ioutil.h"
#include "camculator.h"
#include "textlcd.h"

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

//	fd_dot = open("/dev/dotmatrix", O_WRONLY);
//	if(fd_dot < 0)  {
//			printf("Dotmatrix device open error\n");
//  			return false;
//	}
//	printf("open : [/dev/dotmatrix] \n");

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

void IOutil:maketextlcd(char* argv)
{
	int k, i;
	struct strcommand_varible strcommand;
	char remain[32];
	char remain2[32];
	strcommand.rows = 0;		
	strcommand.nfonts = 0;		
	strcommand.display_enable = 1;		
	strcommand.cursor_enable = 0;		
	strcommand.nblink = 0;		
	strcommand.set_screen = 0;		
	strcommand.set_rightshit = 1;		
	strcommand.increase = 1;		
	strcommand.nshift = 0;		
	strcommand.pos = 0;		
	strcommand.command = 1;		
	strcommand.strlength = 16;		

	ioctl(fd_textlcd,TEXTLCD_CLEAR,&strcommand,32);
	strcommand.pos = 0;
	
	for(i=0;i<32;i++)
	{
			remain[i]=' ';
			remain2[i]=' ';
	
	}	
	if(strlen(argv) > 16)
	{
		for(k=0;k<16;k++) {
			remain[k] = argv[k];
			remain2[k] = argv[k+8];
		}
	}	else
		{
		write(fd_textlcd,argv,strlen(argv));
		return 0;
	}
		
		write(fd_textlcd,remain,32);
	
		strcommand.pos = 40;
		write(fd_textlcd,remain2,40/*strlen(remain2)*/);
		strcommand.pos = 0;

		return 0;
}

void IOutil::textlcd(char* argv)
{
	write(fd_textlcd,argv,strlen(argv));
}

void IOutil::dotmatrix(int mode)
{
	if(mode == 0)
		write(fd_dot,"number",5);
	else if (mode == 1)
		write(fd_dot,"alphabet",5);
	else if(mode ==2)	
		write(fd_dot,"arith",5);
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
//	close(fd_dot);
	close(fd_fnd);
}



