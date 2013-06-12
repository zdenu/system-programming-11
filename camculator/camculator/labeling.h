//
//  labeling.h
//  camculator
//
//  Created by Christopher Kim on 6/9/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__labeling__
#define __camculator__labeling__

#include "define.h"
#include "state.h"
#include <string>

class Labeling : public State
{
public:
	Labeling(void);
	virtual ~Labeling(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam);
	virtual int dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);

	bmp_t* labelingData;
	std::string formula;
};


#endif /* defined(__camculator__labeling__) */
