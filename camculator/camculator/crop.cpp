//
//  crop.cpp
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "crop.h"


Crop::Crop()
: isPhotoExist(false)
{
}


Crop::~Crop()
{
}

bool Crop::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	State::init(dc_buffer, pFont, state);
	
	title = (png_t*)gx_png_open( "interface/title/crop.png");
	button = (png_t*)gx_png_open( "interface/button/check.png");
	
	return true;
}

bool Crop::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);
	
	
	if (pParam != NULL)
	{
		printf("crop data is exist(pParam : %x).\n", pParam);
		stCameraData* pData = (stCameraData*)pParam;
		
		gx_bitblt(dc_buffer, 0, 0, pData->dc_camera, 0, 0, pData->dc_camera->width, pData->dc_camera->height);
	}
	else
	{
		gx_clear((dc_t *)back, gx_color( 255, 255, 255, 255));
		gx_rectangle( dc_buffer, 50, 50, 100, 100);
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);		
	}
	return true;
}

bool Crop::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	this->makeBackground(dc_buffer, pParam);

	State::makeScreen(dc_buffer, dc_screen, pParam);

	// photo가 없을 경우 alert.
	if (!isPhotoExist)
	{
//		interface_alert(const_cast<char*>("Please take photo"));
	}
	else
	{
		dc_buffer->pen_color     = gx_color( 255, 0, 0, 255);
		dc_buffer->brush_color   = gx_color( 0, 0, 0, 0);
		gx_rectangle(dc_buffer, 100, 100, 200, 200);
	}
	
	return true;
}

int Crop::dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent, void** pParam)
{
	return true;
}
