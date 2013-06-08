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

bool Crop::init(dc_t* dc_buffer, font_t* pFont)
{
	State::init(dc_buffer, pFont);
	
	title = (png_t*)gx_png_open( "interface/title/crop.png");
	button = (png_t*)gx_png_open( "interface/button/check.png");
	
	return true;
}

bool Crop::makeBackground(dc_t* dc_buffer)
{
	State::makeBackground(dc_buffer);
	
	gx_clear((dc_t *)back, gx_color( 255, 255, 255, 255));
	gx_rectangle( dc_buffer, 50, 50, 100, 100);
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);

	
	return true;
}

bool Crop::makeScreen(dc_t* dc_buffer, dc_t* dc_screen)
{
	this->makeBackground(dc_buffer);
	State::makeScreen(dc_buffer, dc_screen);

	// photo가 없을 경우 alert.
	if (!isPhotoExist)
	{
//		interface_alert(const_cast<char*>("Please take photo"));
	}

	
	dc_screen->pen_color     = gx_color( 255, 0, 0, 255);
	dc_screen->brush_color   = gx_color( 0, 0, 0, 0);
	gx_rectangle(dc_screen, 100, 100, 200, 200);
	
	drawScreen(dc_buffer, dc_screen);
	return true;
}

int Crop::dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent)
{
	return true;
}
