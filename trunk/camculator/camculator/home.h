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


class Home : public State
{
public:
	Home(void);
	virtual ~Home(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer);
}

#endif /* defined(__camculator__home__) */
