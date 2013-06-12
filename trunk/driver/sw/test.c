#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

static int dev;
static unsigned char vkey;

void usrsignal(int sig)
{
	read(dev, &vkey, 1);
	printf("SW input: %2d\n", vkey);
}

int main(void)
{
	pid_t id;
	int quit = 1;
	
	dev = open("/dev/sw", O_RDWR);
	if(dev<0) {
		printf("application : sw driver open fail!.\n");
		exit(1);
	}
	
	(void)signal(SIGUSR2, usrsignal);
	id = getpid();
	write(dev, &id, 4);
	printf("press the sw button!\n");
	
	while(quit) {
	//	printf("Exit Program!! (key = %d)\n", vkey);
	}
	close(dev);
	return 0;
}
