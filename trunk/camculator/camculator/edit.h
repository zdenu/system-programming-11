//
//  edit.h
//  camculator
//
//  Created by Christopher Kim on 6/8/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__edit__
#define __camculator__edit__

#include "define.h"
#include "state.h"


class Edit : public State
{
public:
	Edit(void);
	virtual ~Edit(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen);
	virtual int dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer);
};


#endif /* defined(__camculator__edit__) */
