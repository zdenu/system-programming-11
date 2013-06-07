

#ifndef __camculator__camera__
#define __camculator__camera__

#include "define.h"
#include "state.h"

class Camera : public State
{
public:
	Camera(void);
	virtual ~Camera(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen);
	virtual bool close(void);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer);
	
private:
	int cameraFd;
};

void dc_camera(dc_t *target);

#endif /* defined(__camculator__camera__) */


