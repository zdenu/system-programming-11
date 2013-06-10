#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "mp3.h"

//Sound
char madplay[30]="/usr/bin/madplay";
static int fd_mixer;
static int volume;
pid_t id, ret=-1;
static unsigned char status=0;


int MP3_initialize()
{
    if( (fd_mixer = open("/dev/mixer0", O_RDONLY)) < 0) {
    	perror("mixer open fails!!");
	return -1;
    }
    ioctl(fd_mixer, SOUND_MIXER_READ_VOLUME, &volume);
    volume=(volume >>8) & 0xff;               
}

void MP3_play(char *path)
{
	if(status == 0) {
		if(ret > 0) kill(ret, SIGKILL);
			sleep(2); 
		if((ret = fork()) == 0) {
			printf("sound_play\n");
			execl(madplay, "madplay", "-2", path, NULL);
			status = 1;
		}
	} else if(status == 2) {
			printf("sound_Replay\n");
			kill(ret,SIGCONT); 
	}
}

void MP3_stop(){
	if(status > 0) {
	printf("sound_Stop\n");
		if(ret > 0){
			kill(ret, SIGKILL);
			status = 0;
		}
	}
}

void MP3_pause(){
	if(status == 1) {
		status == 2;
		printf("sound_Pause\n"); 
		kill(ret, SIGSTOP);
	}
}

void soundSetVolume(int updown, int num)
{
	int left, right;

	ioctl(fd_mixer, SOUND_MIXER_READ_VOLUME, &volume);
	volume=(volume >>8) & 0xff;
	
	if (updown == 0) {	// vol : down
		if (volume != 0) {
			volume = num | ( num << 8);
			ioctl(fd_mixer, SOUND_MIXER_WRITE_VOLUME, &volume);
		}
	}
	else if (updown == 1) {	// vol : up 
		if (volume != 100) {
			volume = num | ( num << 8);
			ioctl(fd_mixer, SOUND_MIXER_WRITE_VOLUME, &volume);
		}
	}
	
	ioctl(fd_mixer, SOUND_MIXER_READ_VOLUME, &volume);
	volume=(volume >>8) & 0xff;

	printf("updown : %d, volume : %d\n", updown, volume);
}

int soundGetVolume()
{
	return volume;
}

void MP3_close() {
	if(ret > 0){
			kill(ret, SIGKILL);
			status = 0;
			ret = -1;
	}
	close(fd_mixer);
}
