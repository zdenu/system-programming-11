//
//  home.h
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__home__
#define __camculator__home__

#include "define.h"
#include "state.h"
#include <fstream>

class Home : public State
{
public:
	Home(void);
	virtual ~Home(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam);
	virtual int dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam);
	std::string history[3];

protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
	bool readHistory();
};

#endif /* defined(__camculator__home__) */
