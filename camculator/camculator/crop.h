//
//  crop.h
//  camculator
//
//  Created by Christopher Kim on 6/7/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__crop__
#define __camculator__crop__

#include "define.h"
#include "state.h"

class Crop : public State
{
public:
	Crop(void);
	virtual ~Crop(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen);
	virtual int dispatchTouchEvent(ENUM_TOUCH_EVENT touchEvent);
	
	void setPhotoData(char* pBuffer, int size){}
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer);
	
	bool isPhotoExist;
};


#endif /* defined(__camculator__crop__) */
