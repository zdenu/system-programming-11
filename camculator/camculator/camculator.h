//
//  camculator.h
//  camculator
//
//  Created by Christopher Kim on 6/6/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef camculator_camculator_h
#define camculator_camculator_h

#include <queue>
#include "singleton.h"
#include "gx.h"
#include "lock.h"
#include "state.h"

typedef std::queue<stEvent*>	TEventQueue;

class TouchHandler;

class Camculator : public Singleton<Camculator>
{
public:
	Camculator();
	~Camculator();
	
public:
	bool init(void);
	void main(void);
	
	int fontloader14(char* file);
	int fontloader18(char* file);
	
	
	void drawPartScreen(int x, int y, int w, int h, color_t color);
	void drawBeforeScreen(void);
	
	void pushEvent(stEvent* pEv);
	stEvent* popEvent(void);
	
private:
	void initSettingLayout(void);
	
private:
	
	void interfaceDispatcher(stEvent* pEv);
	
	void interface_Background(int mode);
	void interface_layout(int mode, int state);
	void interface_loading(int mode);
	void interface_splash(void);
	void interface_alert(char* msg);
	void interface_info(void);
	
	void initTouchEvents(int state);
	
	int getCurrentState(int mode);
	
	void disableSettingEvent(void);
	
private:
	
	State* pState[SCREEN_TYPE_MAX];
	
	TouchHandler* pTouchHandler;
	

	Lock		queueLock;
	TEventQueue	eventQueue;
	
private:
	bool	isSettingInitialized;
	
	int		beforeState;
	int		currentState;
	
	dc_t*	dc_screen;
	dc_t*	dc_buffer;
	dc_t*	before_screen;
	dc_t*	setting_frame;
	
	font_t* font14;
	font_t* font18;
	
	
	bool isRunning;

};

#endif
