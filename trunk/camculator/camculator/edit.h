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
	virtual int dispatchKeyEvent(dc_t* dc_buffer, stKeyData* pKeyEvent, void** pParam);
	std::string replaceAll(const std::string &str, const std::string &pattern, const std::string &replace);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
	int inputKey(std::string n1,
				 std::string n2,
				 std::string n3,
				 std::string a1,
				 std::string a2,
				 std::string a3,
				 std::string s1,
				 std::string s2,
				 std::string s3);
	
	char* stringToUpper( std::string& str );

private:
	int	keymode;
	int	step;
	bool  shift;
	std::string txt;
	int cursor;
	unsigned char bkey;
};


#endif /* defined(__camculator__edit__) */
