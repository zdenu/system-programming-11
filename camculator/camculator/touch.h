#define MAX_TOUCH_EVENT 10
#define EVENT_BUF_NUM 64


void initTouchevent();
int addTouchevent(int x, int y, int width, int height);
void removeTouchevent(int num);
int touch();
void removeAllTouchevent();
int touch_pause();
