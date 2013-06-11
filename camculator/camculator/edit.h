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
	virtual int dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam);
	virtual int dispatchKeyEvent(dc_t* dc_buffer, stKeyData* pKeyEvent, void** pParam)
	
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
	int inputKey(int keymode, string n1 ,string n2,string n3, string a1, string a2, string a3 ,string s1,string s2,string s3)

private:
	int	keymode;
	int	step;
	bool  shift;
	string txt;
	int cursor;
	unsigned char bkey;
};


#endif /* defined(__camculator__edit__) */
