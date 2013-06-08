//
//  edit.cpp
//  camculator
//
//  Created by Christopher Kim on 6/8/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "edit.h"

Edit::Edit()
{
}

Edit::~Edit()
{
}

bool Edit::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	State::init(dc_buffer, pFont, state);
	
	if (back != NULL)
		gx_png_close((dc_t*)back);
	
	back = (png_t*)gx_png_open((char*)"interface/background/edit.png");
	title = (png_t*)gx_png_open( "interface/title/edit.png");
	button = (png_t*)gx_png_open( "interface/button/check.png");
	
	return true;
}
bool Edit::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	this->makeBackground(dc_buffer, pParam);
	State::makeScreen(dc_buffer, dc_screen, pParam);
	
	gx_text_out( dc_buffer, 9, 75 , "lim (15x^2/621x)");
	gx_text_out( dc_buffer, 9, 108, "integral(4x+2)");
	gx_text_out( dc_buffer, 9, 145, "sum(4x*8)");
	gx_text_out( dc_buffer, 9, 178, "sum(4x*8)");

	return true;
}
int Edit::dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent, void** pParam)
{
	return true;
}

bool Edit::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);

	gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);	
	
	return true;
}