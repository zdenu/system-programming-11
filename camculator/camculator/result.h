//
//  result.h
//  camculator
//
//  Created by Christopher Kim on 6/8/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__result__
#define __camculator__result__


#include "define.h"
#include "state.h"


class Result : public State
{
public:
	Result(void);
	virtual ~Result(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam);
	virtual int dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent, void** pParam);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
};


#endif /* defined(__camculator__result__) */
