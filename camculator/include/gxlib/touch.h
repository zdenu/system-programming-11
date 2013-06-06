#define MAX_TOUCH_EVENT 10

struct touchEventArea
{
	int x;
	int y;
	int w;
	int h;
};

struct touchEventList
{
	int x;
	int y;
	int w;
	int h;
	int enable;
};

int addTouchevent(int x, int y, int width, int height);
void removeTouchevent(int num);
static int touch();
