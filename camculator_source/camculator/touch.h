#define MAX_TOUCH_EVENT 50
#define EVENT_BUF_NUM 64


void initTouchevent(char *device ,dc_t *dc_screen);
void closeTouchevent();
int addTouchevent(int x, int y, int width, int height);
void removeTouchevent(int num);
int touch(dc_t *dc_screen);
void removeAllTouchevent();
void resumeTouchevent();
void pauseTouchevent();
