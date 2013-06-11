#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{
        
	int fd;
	int value;

        if((fd=open("/dev/7segment",O_RDWR|O_SYNC)) < 0) {
                printf("FND open fail\n");
                exit(1);
        }

        while(value != 0) {
                write(fd,"123456",12);
        }

	close(fd);
        return 0;
           
} 
