//
//  state.h
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__state__
#define __camculator__state__
#include "define.h"
#include "gx.h"
#include "gxpng.h"

class State
{
public:
	State(void);
	virtual ~State(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont);
	
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen);
	virtual bool close(void);
	
	

	bool dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent);
	
	ENUM_SCREEN_TYPE getScreenType(void)		{ return state; }
	void setScreenType(ENUM_SCREEN_TYPE state)	{ this->state = state; }
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer);
	bool drawScreen(dc_t* dc_buffer, dc_t* dc_screen);
	void setFont(dc_t* dc_buffer);
	
protected:
	ENUM_SCREEN_TYPE state;
	ENUM_TOUCH_EVENT touchEvent;
	
	font_t*	font;
	png_t*	top;
	png_t*	title;
	png_t*	button;
	png_t*	button2;
	png_t*	bottom;
	png_t*	active;
	png_t*	back;
};
#endif /* defined(__camculator__state__) */
