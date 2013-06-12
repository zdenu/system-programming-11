//
//  util.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 6/4/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef __camculator__util__
#define __camculator__util__

#include "define.h"
#include "giflib/gif_lib.h"
#include "giflib/gif_lib_private.h"
#include <stdarg.h>
#include "WolframAlphaManager.h"

namespace Util
{
	void dumpMemory(void *data, int dataSize );
	
	void GifQprintf(char *Format, ...);
	void PrintGifError(int ErrorCode);
	
	void DumpScreen2RGB(
						int& outSize,
						TRGBVector* pRGBBuffer,
						ColorMapObject *ColorMap,
						GifRowType *ScreenBuffer,
						int ScreenWidth,
						int ScreenHeight);

	
	bool GIF2RGB(char *FileName,
				 TRGBVector* pRGBBuffer,
				 int& outSize,
				 int& width,
				 int& height)
;
}

#endif // __camculator__util__

