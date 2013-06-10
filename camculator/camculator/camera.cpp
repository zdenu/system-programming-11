#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "gx.h"
#include "camera.h"

Camera::Camera(void)
: cameraFd(INVALID_DATA)
, dc_camera(NULL)
{
}

Camera::~Camera(void)
{
	if (dc_camera != NULL)
		gx_release_dc(dc_camera);
}

bool Camera::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	printf("camera init start.\n");
	State::init(dc_buffer, pFont, state);

	// camera doesn't use background png.
	if (back != NULL)
	{
		gx_png_close((dc_t*)back);
		back = NULL;
	}
	
	cameraFd = ::open(CAMERA_DEVICE, O_RDWR);
	if (cameraFd == INVALID_DATA)
		return false;
	
	title = (png_t*)gx_png_open((char*)"interface/title/camera.png");
	button = (png_t*)gx_png_open((char*)"interface/button/camera.png");
}

bool Camera::makeBackground(dc_t *dc_buffer, void* pParam)
{
	if (State::makeBackground(dc_buffer, pParam) == false)
		return false;
	
	read(cameraFd, dc_buffer->mapped, SCREEN_BUFFER_SIZE);
	
	return true;
}

bool Camera::makeScreen(dc_t *dc_buffer, dc_t *dc_screen, void* pParam)
{
	if (this->makeBackground(dc_buffer, pParam) == false)
		return false;
	
	if (State::makeScreen(dc_buffer, dc_screen, pParam) == false)
		return false;
	
	return true;
}

bool Camera::close(void)
{
	State::close();
	
	::close(cameraFd);
}

int Camera::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{	
	if (pTouchEvent->touchType == TOUCH_EVENT_MAIN_OK)
	{
		stCameraData* pCamData = new stCameraData;
		
		pCamData->dc_camera = gx_get_compatible_dc(dc_buffer);
		read(cameraFd, pCamData->dc_camera->mapped, SCREEN_BUFFER_SIZE);
		
		(*pParam) = pCamData;
	}
	
	return true;
}


