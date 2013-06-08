//
//  state.cpp
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "state.h"
#include "touch.h"
#include "camculator.h"

State::State()
: top(NULL)
, bottom(NULL)
, button(NULL)
, button2(NULL)
, active(NULL)
{
	
}

State::~State()
{
	
}



bool State::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	top			= (png_t*)gx_png_open("interface/top.png");
	bottom		= (png_t*)gx_png_open("interface/menu.png");
	button2		= (png_t*)gx_png_open("interface/button/home.png");
	back		= (png_t*)gx_png_create(320, 240);
	active		= (png_t*)gx_png_create(63, 49);
	font		= pFont;
	this->state = state;

	gx_clear((dc_t *)active, gx_color(255, 255, 255, 100));
}

void State::setFont(dc_t* dc_buffer)
{
	dc_buffer->font	= font;
	dc_buffer->font_color = gx_color( 0, 0, 0, 255);
}

bool State::makeBackground(dc_t *dc_buffer, void* pParam)
{
	return true;
}

bool State::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	if ((top == NULL) ||
		(title == NULL) ||
		(button == NULL) ||
		(bottom == NULL) ||
		(button2 == NULL) ||
		(active == NULL) ||
		(dc_buffer == NULL) ||
		(dc_screen == NULL) ||
		(font == NULL) )
		gx_print_error(8, "layout pngs");
	else
	{
		setFont(dc_buffer);
		
		int x = 64 * (state - 1);
		gx_bitblt(dc_buffer, 0, 0, (dc_t *)top, 0, 0, top->width, top->height);
		gx_bitblt(dc_buffer, 98, 8, (dc_t *)title, 0, 0, title->width, title->height);
		gx_bitblt(dc_buffer, 281, 7, (dc_t *)button, 0, 0, button->width, button->height);
		gx_bitblt(dc_buffer, 4, 6, (dc_t *)button2, 0, 0, button->width, button->height);
		gx_bitblt(dc_buffer, 0, 191, (dc_t *)bottom, 0, 0, bottom->width, bottom->height);
		
		if(x >= 0)
			gx_bitblt( dc_buffer, x, 191, ( dc_t *)active, 0, 0, active->width, active->height);
	}
}

bool State::drawScreen(dc_t* dc_buffer, dc_t* dc_screen)
{
	if (dc_buffer == NULL || dc_screen == NULL)
		return false;
	
	gx_bitblt(dc_screen, 0, 0, (dc_t *)dc_buffer, 0, 0,320, 240);
}

bool State::close(void)
{
	if (top != NULL)
	{
		gx_png_close((dc_t*)top);
		top = NULL;
	}
	if (bottom != NULL)
	{
		gx_png_close((dc_t*)bottom);
		bottom = NULL;
	}
	if (title != NULL)
	{
		gx_png_close((dc_t*)title);
		title = NULL;
	}
	if (button != NULL)
	{
		gx_png_close((dc_t*)button);
		button = NULL;
	}
	if (button2 != NULL)
	{
		gx_png_close((dc_t*)button2);
		button2 = NULL;
	}
	if (active != NULL)
	{
		gx_png_close((dc_t*)active);
		active = NULL;
	}
	if (back != NULL)
	{
		gx_png_close((dc_t*)back);
		back = NULL;
	}
}

void State::disableTouchEvents(void)
{
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_HOME);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_CAMERA);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_CROP);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_LABELING);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_EDIT);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_RESULT);
	Camculator::get().getTouchHandler()->disableTouchEvent(TOUCH_EVENT_MAIN_OK);
}
void State::enableTouchEvents(void)
{
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_HOME);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_CAMERA);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_CROP);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_LABELING);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_EDIT);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_RESULT);
	Camculator::get().getTouchHandler()->enableTouchEvent(TOUCH_EVENT_MAIN_OK);
	
}
