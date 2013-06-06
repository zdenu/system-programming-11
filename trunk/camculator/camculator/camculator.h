//
//  camculator.h
//  camculator
//
//  Created by Christopher Kim on 6/6/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef camculator_camculator_h
#define camculator_camculator_h

#include "singleton.h"
#include "../include/gxlib/gx.h"

class TouchHandler;

class Camculator : public Singleton<Camculator>
{
public:
	Camculator();
	~Camculator();
	
public:
	bool init(void);
	void main(void);
	
private:
	void interface_Background(int mode);
	void interface_layout(int mode);
	void interface_loading(int mode);
	void interface_splash(void);
	void interface_alert(char* msg);
	void interface_info(void);
	
	
	void setTouchEvents(void);
	
private:
	TouchHandler* pTouchHandler;
	
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
	
	font_t* malgun14;
	font_t* malgun18;
	
	bool isRunning;

};

#endif
