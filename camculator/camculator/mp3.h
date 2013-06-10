
#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void soundSetVolume(int updown, int num);
int MP3_initialize();
void MP3_play(char *path);
void MP3_stop();
void MP3_pause();
void soundSetVolume(int updown, int num);
int soundGetVolume();
	
#ifdef __cplusplus
}
#endif /* __cplusplus */


