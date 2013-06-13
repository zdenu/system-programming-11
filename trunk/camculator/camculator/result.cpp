//
//  result.cpp
//  camculator
//
//  Created by Christopher Kim on 6/8/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "result.h"
#include "camculator.h"
#include "touch.h"
#include "mp3.h"

using namespace std;

Result::Result()
:isResultExist(false)
, currentCursor(0)
, loadingState(0)
{
}

Result::~Result()
{
}

bool Result::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	printf("result init start.\n");
	MP3_play("/mnt/usb/sound/ko/result2.mp3");
	State::init(dc_buffer, pFont, state);
	
	title = (png_t*)gx_png_open( "interface/title/result.png");
	button = (png_t*)gx_png_open( "interface/button/send.png");
	loadingState = START;

	

	return true;
}
bool Result::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	this->makeBackground(dc_buffer, pParam);
	State::makeScreen(dc_buffer, dc_screen, pParam);
	
	if (!isResultExist)
	{
		Camculator::get().interface_loading(loadingState);
		loadingState++;
		
		if (loadingState == STEP7)
			loadingState = STEP0;
	}
	else
	{
		Camculator::get().interface_loading(END);
		
		gx_bitblt(dc_buffer,
				  USABLE_POINT_START_X,
				  USABLE_POINT_START_Y,
				  pResultDc,
				  0,
				  currentCursor,
				  320,
				  USABLE_POINT_END_Y - USABLE_POINT_START_Y);
		// if result exist.
	}
	
	return true;
}


int Result::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{
	if (pTouchEvent->touchType == TOUCH_EVENT_RESULT_NEXT)
	{
		currentCursor += RESULT_TOUCH_CURSOR_VALUE;
		if (currentCursor > pResultDc->height - (USABLE_POINT_END_Y - USABLE_POINT_START_Y))
			currentCursor = pResultDc->height - (USABLE_POINT_END_Y - USABLE_POINT_START_Y);
	}
	else if (pTouchEvent->touchType == TOUCH_EVENT_RESULT_PREV)
	{
		currentCursor -= RESULT_TOUCH_CURSOR_VALUE;
		if (currentCursor < 0)
			currentCursor = 0;
	}
	return true;
}

bool Result::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);
	
	gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);

	
	return true;
}

bool Result::parseGifImages(TImageVector* pVector)
{
	if (pVector == NULL)
		return false;
	
	removeImageList();
	totalHeight = 0;
	totalWidth = 0;
	
	for (int i = 0 ; i < pVector->size() ; ++i)
	{
		stImageData* pData = pVector->at(i);
		
		// copy to dc_t.
		dc_t* pImg = gx_png_create(pData->width, pData->height);
		int idx = 0;
		for (int j = 0 ; j < pData->height; ++j)
		{
			for (int k = 0 ; k < pData->width ; ++k)
			{
				gx_set_pixel(pImg, k, j, gx_color(pData->pVector->at(idx).r,
												  pData->pVector->at(idx).g,
												  pData->pVector->at(idx).b,
												  255));
				
				idx++;
			}
		}
		pImgList.push_back(pImg);
		
		totalHeight += pData->height;
		
		// delete pData.
		delete pData;
	}
	
	pResultDc = gx_png_create(320, totalHeight);
	int x = 0, y = 0;
	int width = 0;
	int height = 0;
	
	for (int i = 0 ; i < pImgList.size() ; ++i)
	{
		x = (320 - pImgList[i]->width) / 2;
		gx_bitblt(pResultDc, x, y, pImgList[i], 0, 0, pImgList[i]->width, pImgList[i]->height);
		
		y += pImgList[i]->height;
	}
	
	isResultExist = true;
	
	return true;
}

void Result::removeImageList(void)
{
	if (!pImgList.empty())
	{
		for (int i = 0 ; i < pImgList.size() ; ++i)
			gx_png_close((dc_t*)pImgList[i]);
	}
}

void Result::disableTouchEvents(void)
{
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_RESULT_PREV);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_RESULT_NEXT);
}
void Result::enableTouchEvents(void)
{
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_RESULT_PREV);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_RESULT_NEXT);
}

