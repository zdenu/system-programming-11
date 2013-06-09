

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
	virtual bool init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam);
	virtual bool close(void);
	
	virtual int dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
	
private:
	dc_t* dc_camera;
	int cameraFd;
};

#endif /* defined(__camculator__camera__) */


