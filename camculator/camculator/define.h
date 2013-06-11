//
//  Define.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 6/5/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef WolframAlphaAPI_Define_h
#define WolframAlphaAPI_Define_h

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>

#include "gx.h"

typedef bool			BOOL;
typedef int16_t			SHORT;
typedef int32_t			INT;
typedef int64_t			INT64;
typedef uint16_t		USHORT;
typedef uint32_t		UINT;
typedef uint64_t		UINT64;
typedef char			CHAR;
typedef unsigned char	UCHAR;
typedef UCHAR			BYTE;

#define TRUE	1
#define FALSE	0
#define INVALID_DATA	-1
#define INVALID_HANDLE	0

#define MAX_TOUCH_EVENT		10
#define EVENT_BUF_NUM		64
#define SCREEN_BUFFER_SIZE	240 * 320 * 2

#define FRAME_BUFFER_DEVICE		"/dev/fb"
#define TOUCH_EVENT_DEVICE		"/dev/input/event0"
#define KEY_EVENT_DEVICE		"/dev/keypad"
#define SW_EVENT_DEVICE		"/dev/sw"
#define CAMERA_DEVICE			"/dev/camera"

#define FONT_MALGUN_14			"font/14pt/malgun14.bdf"
#define FONT_MALGUN_18			"font/18pt/malgun18.bdf"

#define WOLFRAM_ALPHA_APP_ID	"EUGLWW-4XTP6Q3GK2"
#define WOLFRAM_ALPHA_HOST		"api.wolframalpha.com"

#define MAKE_PIXEL( _r_, _g_, _b_ ) \
			(USHORT)( ((_r_ >> 3) << 11) |\
					  ((_g_ >> 2) << 5) |\
					  (_b_ >> 3))


#define HOME		0
#define CAMERA		1
#define CROP		2
#define LABELING	3
#define EDIT		4
#define RESULT		5
#define START		1
#define END			0

enum ENUM_SCREEN_TYPE
{
	SCREEN_TYPE_HOME = 0,
	SCREEN_TYPE_CAMERA,
	SCREEN_TYPE_CROP,
	SCREEN_TYPE_LABELING,
	SCREEN_TYPE_EDIT,
	SCREEN_TYPE_RESULT,
	SCREEN_TYPE_INFO,
	SCREEN_TYPE_SETTING,
	SCREEN_TYPE_ALERT,
	
	SCREEN_TYPE_MAX,
};

enum ENUM_TOUCH_EVENT
{
	// used in main.
	TOUCH_EVENT_MAIN_HOME	= 0,		// mode, state
	TOUCH_EVENT_MAIN_CAMERA,			// mode, state
	TOUCH_EVENT_MAIN_CROP,				// mode, state
	TOUCH_EVENT_MAIN_LABELING,			// mode, state
	TOUCH_EVENT_MAIN_EDIT,				// mode, state
	TOUCH_EVENT_MAIN_RESULT,			// mode, state
	TOUCH_EVENT_MAIN_OK,				// mode, state	//
	
	TOUCH_EVENT_CROP_CLICK,
	
	// used in setting.
	TOUCH_EVENT_SETTING_OPEN,			// mode, state
	TOUCH_EVENT_SETTING_CLOSE,			// mode
	TOUCH_EVENT_SETTING_FONT14,			// mode
	TOUCH_EVENT_SETTING_FONT18,			// mode
	TOUCH_EVENT_SETTING_NETWORK,		// mode
	TOUCH_EVENT_SETTING_VOLUME,			// mode
	TOUCH_EVENT_SETTING_OK,				// mode
	// used in alert
	
	TOUCH_EVENT_ALERT_ALL,
	
	TOUCH_EVENT_MAX,
};



enum ENUM_EVENT_TYPE
{
	EVENT_TYPE_NONE			= 0,
	EVENT_TYPE_HTTP_RESPONSE,
	EVENT_TYPE_TOUCH_PAD,
	EVENT_TYPE_KEY_PAD,
	EVENT_TYPE_MAX,
};

#pragma pack(1)

struct stEvent
{
	stEvent(void)
	: eventType(EVENT_TYPE_NONE)
	, pData(NULL)
	, dataSize(0)
	{}
	
	BYTE	eventType;
	void*	pData;			// type에 따라서 달라진다.
	int32_t	dataSize;
	
	
};

struct stTouchData
{
	stTouchData(void): x(0), y(0){}
	
	int touchType;
	int x;
	int y;
};

struct stKeyData
{
	stKeyData(void): code(0){}
	static unsigned char key;
};

struct stImageData
{
	stImageData(void) : pBuffer(NULL), size(NULL){}
	~stImageData(void) { if (pBuffer != NULL) delete [] pBuffer; }
	char*	pBuffer;
	int32_t size;
};

struct stCameraData
{
	stCameraData(void) : dc_camera(NULL){}
	~stCameraData(void) { if (dc_camera != NULL) gx_release_dc(dc_camera); }
	
	dc_t* dc_camera;
};

struct stCropData
{
	stCropData(void) : dc_crop(NULL){}
	~stCropData(void) { if (dc_crop != NULL) gx_png_close(dc_crop); }
	
	dc_t* dc_crop;
};

#pragma pack()

#endif
