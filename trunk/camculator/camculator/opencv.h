//
//  opencv.h
//  camculator
//
//  Created by Christopher Kim on 6/10/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#ifndef camculator_opencv_h
#define camculator_opencv_h

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "BlobLabeling.h"
#include "gx.h"

#define RED_MASK 0xF800
#define GREEN_MASK 0x07E0
#define BLUE_MASK 0x001F

#define TEMPLATE_NUM 39
#define TEMPLATE_FONT 2

using namespace cv;
using namespace std;

class OpenCV
{
public:
	OpenCV():definedIntStartFlg(0), definedIntEndFlg(0){}
	~OpenCV(){}
	
public:
	bool init(void);
	bool Labeling(dc_t* pData, int width, int height, std::string& strData);
	
protected:
	IplImage* rgb565to888(dc_t *rgb565Data ,int width, int height);
	IplImage* img_resize(IplImage* src_img, int height);
	IplImage* img_resizeto_screen(IplImage* src_img);
	int matching(IplImage *srcImage, double* error);
	
private:
	IplImage *tImage[TEMPLATE_NUM][2];
	int objCenterPosition[TEMPLATE_NUM][2];
	string definedInt[2];
	int definedIntStartFlg;
	int definedIntEndFlg;
	string definedIntVariable;

};


#endif
