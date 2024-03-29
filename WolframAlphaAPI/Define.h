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

#define FRAME_BUFFER_DEVICE		"/dev/fb"
#define TOUCH_EVENT_DEVICE		"/dev/input/event0"

#define WOLFRAM_ALPHA_APP_ID	"EUGLWW-4XTP6Q3GK2"
#define WOLFRAM_ALPHA_HOST		"api.wolframalpha.com"

#define MAKE_PIXEL( _r_, _g_, _b_ ) \
			(USHORT)( ((_r_ >> 3) << 11) |\
					  ((_g_ >> 2) << 5) |\
					  (_b_ >> 3))


enum ENUM_EVENT_TYPE
{
	EVENT_TYPE_NONE			= 0,
	EVENT_TYPE_HTTP_RESPONSE,
	EVENT_TYPE_TOUCH_PAD,
	EVENT_TYPE_MAX,
};

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

struct stImageData
{
	char*	pBuffer;
	int32_t size;
};

#endif
