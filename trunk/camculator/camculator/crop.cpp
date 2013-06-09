//
//  crop.cpp
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "crop.h"
#include "gx.h"

Crop::Crop()
: isPhotoExist(false)
, touchCnt(0)
, firstX(0)
, firstY(0)
, secondX(0)
, secondY(0)
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
		// clear black.
		gx_clear(dc_buffer, gx_color(0, 0, 0, 255));
		
		printf("crop data is exist(pParam : %x).\n", pParam);
		stCameraData* pCamData = (stCameraData*)pParam;
		
		printf("crop dc_camera : width: %d, height: %d, bytes: %d, bytes_per_line: %d \n",
			   pCamData->dc_camera->width,
			   pCamData->dc_camera->height,
			   pCamData->dc_camera->bytes,
			   pCamData->dc_camera->bytes_per_line);
		
		gx_bitblt(dc_buffer, 0, 0, pCamData->dc_camera, 0, 0, pCamData->dc_camera->width, pCamData->dc_camera->height);
		
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
//	if (!isPhotoExist)
//	{
////		interface_alert(const_cast<char*>("Please take photo"));
//	}
//	else
	{
		dc_buffer->pen_color     = gx_color( 255, 0, 0, 255);
		dc_buffer->brush_color   = gx_color( 0, 0, 0, 0);
		
		if (touchCnt == 1)
		{
			// set pixel to firstX, firstY.
			gx_set_pixel(dc_buffer, firstX, firstY, dc_buffer->pen_color);
		}
		else if (touchCnt == 2)
		{
			gx_rectangle(dc_buffer, firstX, firstY, secondX, secondY);
		}
	}
	
	return true;
}

int Crop::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{
	if (pTouchEvent->touchType == TOUCH_EVENT_CROP_CLICK)
	{
		if (touchCnt == 0)
		{
			firstX = pTouchEvent->x;
			firstY = pTouchEvent->y;
			touchCnt = 1;
		}
		else if(touchCnt == 1)
		{
			secondX = pTouchEvent->x;
			secondY = pTouchEvent->y;
			touchCnt = 2;
		}
		else if (touchCnt == 2)
		{
			resetCropPoints();
			firstX = pTouchEvent->x;
			firstY = pTouchEvent->y;
			touchCnt = 1;
		}
	}
	else if (pTouchEvent->touchType == TOUCH_EVENT_MAIN_OK)
	{
		// crop and go to labeling.
		stCameraData* pCamData = new stCameraData;
		pCamData->dc_camera = gx_get_compatible_dc(dc_buffer);
		
		// crop data.
		gx_bitblt(pCamData->dc_camera, 0, 0, dc_buffer, firstX, firstY, secondX - firstX, secondY - firstY);
	}
	
	return true;
}

void Crop::resetCropPoints(void)
{
	touchCnt = 0;
	firstX = 0;
	firstY = 0;
	secondX = 0;
	secondY = 0;
}
