//
//  labeling.cpp
//  camculator
//
//  Created by Christopher Kim on 6/9/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "labeling.h"

Labeling::Labeling()
{
}

Labeling::~Labeling()
{
}

bool Labeling::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	printf("labeling init start.\n");
	State::init(dc_buffer, pFont, state);
		
	title = (png_t*)gx_png_open( "interface/title/labeling.png");
	button = (png_t*)gx_png_open( "interface/button/check.png");
	
	return true;
}
bool Labeling::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	this->makeBackground(dc_buffer, pParam);
	State::makeScreen(dc_buffer, dc_screen, pParam);
	
	if (pParam != NULL)
	{
		stCameraData* pCamData = (stCameraData*)pParam;
		int startX = (320 - (pCamData->dc_camera->width)) / 2;
		int startY = (240 - (pCamData->dc_camera->height)) / 2;
		
		gx_bitblt(dc_buffer, startX, startY, pCamData->dc_camera, 0, 0, pCamData->dc_camera->width, pCamData->dc_camera->height);
	}
	
	return true;
}
int Labeling::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{
	return true;
}

bool Labeling::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);
	
	gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
	
	return true;
}