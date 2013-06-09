//
//  home.cpp
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "home.h"


Home::Home()
{
	
}

Home::~Home()
{
	
}
bool Home::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	State::init(dc_buffer, pFont, state);
	
	if (back != NULL)
		gx_png_close((dc_t*)back);
	
	back = (png_t*)gx_png_open((char*)"interface/background/home.png");
	title = (png_t*)gx_png_open( "interface/title/home.png");
	button = (png_t*)gx_png_open( "interface/button/info.png");
	
	if (button2 != NULL)
		gx_png_close((dc_t*)button2);
		
	button2 = (png_t*)gx_png_open((char*)"interface/button/setting.png");
}

bool Home::makeBackground(dc_t *dc_buffer, void* pParam)
{
	if (State::makeBackground(dc_buffer, pParam) == false)
		return false;
	
	if (dc_buffer == NULL || back == NULL)
		return false;
	
	gx_clear((dc_t *)dc_buffer, gx_color(0, 0, 0, 255));
	gx_bitblt(dc_buffer, 0, 0, (dc_t *)back, 0, 0, back->width, back->height);	
}

bool Home::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	if (this->makeBackground(dc_buffer, pParam) == false)
		return false;
	
	if (State::makeScreen(dc_buffer, dc_screen, pParam) == false)
		return false;
	
	gx_text_out( dc_buffer, 58, 115, (char*)"lim (15x^2/621x)");
	gx_text_out( dc_buffer, 58, 146, (char*)"integral(4x+2)");
	gx_text_out( dc_buffer, 58, 175, (char*)"sum(4x*8)");
	
}


int Home::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{
	// 
	return true;
}