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
	virtual bool init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam);
	virtual int dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent, void** pParam);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
};


#endif /* defined(__camculator__edit__) */
