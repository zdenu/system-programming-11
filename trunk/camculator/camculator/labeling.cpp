//
//  labeling.cpp
//  camculator
//
//  Created by Christopher Kim on 6/9/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "labeling.h"
#include "camculator.h"
#include "opencv.h"

Labeling::Labeling()
: labelingData(NULL)
{
	formula.clear();
}

Labeling::~Labeling()
{
}

bool Labeling::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	printf("labeling init start.\n");
	State::init(dc_buffer, pFont, state);
		
	title = (png_t*)gx_png_open( "interface/title/labeling.png");
	button = (png_t*)gx_png_open( "interface/button/check.png");
	
	return true;
}
bool Labeling::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	// TODO : Show On Loading.....
	
	if (pParam != NULL)
	{
		stCropData* pCropData = (stCropData*)pParam;
		Camculator::get().getOpenCV()->Labeling(pCropData->dc_crop,
												pCropData->dc_crop->width,
												pCropData->dc_crop->height,
												formula);
		
		printf("Labeling complete\n");
		
		labelingData = (bmp_t*)gx_bmp_open((char*)"lable_result.bmp");
	}
	
	
	
	this->makeBackground(dc_buffer, pParam);
	State::makeScreen(dc_buffer, dc_screen, pParam);
	if (labelingData != NULL)
	{
		printf("labeling is not NULL.\n");
		int destX = (USABLE_POINT_END_X - labelingData->width) / 2;
		int destY = (((USABLE_POINT_END_Y - USABLE_POINT_START_X) - labelingData->height) / 2) + USABLE_POINT_START_X;
	
		gx_bitblt(dc_buffer, 0, 50, (dc_t*)labelingData, 0, 0, labelingData->width, labelingData->height);
	}
	
	return true;
}
int Labeling::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{

	if (pTouchEvent->touchType == TOUCH_EVENT_MAIN_OK)
	{
		if (!formula.empty())
		{
			// send formula to edit.
			stFormulaData* pFormula = new stFormulaData;
			pFormula->formula.append(formula);
			(*pParam) = pFormula;
		}
	}
	
	
	return true;
}

bool Labeling::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);
	
	gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
	
	return true;
}