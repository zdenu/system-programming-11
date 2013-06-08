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
{
}

Camera::~Camera(void)
{
}

bool Camera::init(dc_t* dc_buffer, font_t* pFont)
{
	State::init(dc_buffer, pFont);

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

bool Camera::makeBackground(dc_t *dc_buffer)
{
	if (State::makeBackground(dc_buffer) == false)
		return false;
	
	read(cameraFd, dc_buffer->mapped, 240 * 320 * 2);
	
	return true;
}

bool Camera::makeScreen(dc_t *dc_buffer, dc_t *dc_screen)
{
	if (this->makeBackground(dc_buffer) == false)
		return false;
	
	if (State::makeScreen(dc_buffer, dc_screen) == false)
		return false;
	
	return true;
}

bool Camera::close(void)
{
	State::close();
	
	::close(cameraFd);
}

int Camera::dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent)
{
	return true;
}


