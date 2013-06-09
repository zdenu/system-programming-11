//
//  result.cpp
//  camculator
//
//  Created by Christopher Kim on 6/8/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "result.h"

Result::Result()
{
}

Result::~Result()
{
}

bool Result::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	State::init(dc_buffer, pFont, state);
	
	title = (png_t*)gx_png_open( "interface/title/result.png");
	button = (png_t*)gx_png_open( "interface/button/send.png");

	return true;
}
bool Result::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	this->makeBackground(dc_buffer, pParam);
	State::makeScreen(dc_buffer, dc_screen, pParam);
	
	
	if (pParam != NULL)
	{
		// draw now loading....
	}
	return true;
}
int Result::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{
	return true;
}

bool Result::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);
	
	gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);

	
	return true;
}