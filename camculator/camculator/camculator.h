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
	
	void interfaceDispatcher(stEvent* pEv);
	
	void interface_Background(int mode);
	void interface_layout(int mode);
	void interface_loading(int mode);
	void interface_splash(void);
	void interface_alert(char* msg);
	void interface_info(void);
	void interface_setting(void);
	
	
	void setTouchEvents(void);
	
private:
	TouchHandler* pTouchHandler;
	

	Lock		queueLock;
	TEventQueue	eventQueue;
	
private:
	int h_touch_home;
	int h_touch_camera;
	int h_touch_crop;
	int h_touch_labeling;
	int h_touch_edit;
	int h_touch_result;
	int h_touch_ok;
	
	dc_t*	dc_screen;
	dc_t*	dc_buffer;
	dc_t*	before_screen;
	
	font_t* font14;
	font_t* font18;
	
	int screenMode;
	
	bool isRunning;

};

#endif
