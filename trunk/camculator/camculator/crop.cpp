//
//  crop.cpp
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "define.h"
#include "crop.h"
#include "gx.h"
#include "camculator.h"
#include "touch.h"
#include "mp3.h"
#include "ioutil.h"

Crop::Crop()
: isPhotoExist(false)
, touchCnt(0)
, firstX(0)
, firstY(0)
, secondX(0)
, secondY(0)
, pData(NULL)
{
}


Crop::~Crop()
{
}

bool Crop::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	printf("crop init start.\n");
	//MP3_play("/mnt/usb/sound/ko/crop.mp3");
	//Camculator::get().pIOutil->textlcd("Crop... please make area");
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
		stCameraData* pCamData = (stCameraData*)pParam;
		
		pData = gx_get_compatible_dc(pCamData->dc_camera);
		gx_bitblt(pData, 0, 0, pCamData->dc_camera, 0, 0, pCamData->dc_camera->width, pCamData->dc_camera->height);
		gx_bitblt(dc_buffer, 0, 0, pData, 0, 0, pData->width, pData->height);
		
	}
	else
	{
		if (pData == NULL)
		{
			gx_clear((dc_t *)back, gx_color( 255, 255, 255, 255));
			gx_rectangle( dc_buffer, 50, 50, 100, 100);
			gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		}
		else
			gx_bitblt(dc_buffer, 0, 0, pData, 0, 0, pData->width, pData->height);

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
		
		
		if (touchCnt == 1)
		{
			// set pixel to firstX, firstY.
			dc_buffer->pen_color     = gx_color( 255, 0, 0, 255);
			dc_buffer->brush_color   = gx_color( 255, 0, 0, 255);

			gx_set_pixel(dc_buffer, firstX, firstY, dc_buffer->pen_color);
			gx_circle(dc_buffer, firstX, firstY, 3);
		}
		else if (touchCnt == 2)
		{
			dc_buffer->pen_color     = gx_color( 0, 0, 255, 255);
			dc_buffer->brush_color   = gx_color( 255, 0, 0, 0);

			//gx_circle(dc_buffer, secondX, secondY, 3);
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
			MP3_play("/mnt/usb/sound/ko/crop2.mp3");
		}
		else if(touchCnt == 1)
		{
			secondX = pTouchEvent->x;
			secondY = pTouchEvent->y;
			touchCnt = 2;
		}
		else if (touchCnt == 2)
		{
			//MP3_play("/mnt/usb/sound/ko/crop0.mp3");
			resetCropPoints();
			firstX = pTouchEvent->x;
			firstY = pTouchEvent->y;
			touchCnt = 1;
		}
		
		printf("CROP::TouchCount- firstX: %d, firstY: %d, secondX: %d, secondY: %d, touchCnt: %d\n",
			   firstX,
			   firstY,
			   secondX,
			   secondY);
	}
	else if (pTouchEvent->touchType == TOUCH_EVENT_MAIN_OK)
	{
		// crop and go to labeling.
		Camculator::get().pIOutil->textlcd("Edit... Please use keypad");
		stCropData* pCropData = new stCropData;
		pCropData->dc_crop = gx_png_create(secondX - firstX, secondY - firstY);
		// crop data.
		gx_bitblt(pCropData->dc_crop, 0, 0, dc_buffer, firstX, firstY, secondX - firstX, secondY - firstY);
		
		(*pParam) = pCropData;
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

void Crop::disableTouchEvents(void)
{
	State::disableTouchEvents();
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_CROP_CLICK);
}
void Crop::enableTouchEvents(void)
{
	State::enableTouchEvents();
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_CROP_CLICK);
}
