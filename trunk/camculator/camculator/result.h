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
#include "WolframAlphaManager.h"
#include <vector>
#include <fstream>

class Result : public State
{
public:
	Result(void);
	virtual ~Result(void);
	
public:
	virtual bool init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state);
	virtual bool makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam);
	virtual int dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam);

	bool parseGifImages(TImageVector* pVector);
	
	virtual void disableTouchEvents(void);
	virtual void enableTouchEvents(void);
	
protected:
	virtual bool makeBackground(dc_t* dc_buffer, void* pParam);
	
private:
	void removeImageList(void);

private:
	int loadingState;
	bool isResultExist;
	std::vector<dc_t*> pImgList;
	int totalWidth;
	int totalHeight;
	dc_t* pResultDc;
	
	int currentCursor;

};


#endif /* defined(__camculator__result__) */
