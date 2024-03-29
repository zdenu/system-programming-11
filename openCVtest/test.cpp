#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "BlobLabeling.h"

#define RED_MASK 0xF800
#define GREEN_MASK 0x07E0
#define BLUE_MASK 0x001F

#define TEMPLATE_NUM 39
#define TEMPLATE_FONT 2

using namespace cv;
using namespace std;

IplImage *tImage[TEMPLATE_NUM][2];
int objCenterPosition[TEMPLATE_NUM][2];


IplImage* rgb565to888(unsigned short *rgb565Data ,int width, int height)
{
    IplImage *rgb888Image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    int rgb565Step = width;

    uchar* rgb888Data = (uchar*)rgb888Image->imageData;

    float factor5Bit = 255.0 / 31.0;
    float factor6Bit = 255.0 / 63.0;
    for(int i = 0; i < height; i++)
	    {
		for(int j = 0; j < width; j++)
		{
		    unsigned short rgb565 = rgb565Data[i*rgb565Step + j];
		    uchar r5 = (rgb565 & RED_MASK)   >> 11;
		    uchar g6 = (rgb565 & GREEN_MASK) >> 5;
		    uchar b5 = (rgb565 & BLUE_MASK);
		    // round answer to closest intensity in 8-bit space...
		    uchar r8 = floor((r5 * factor5Bit) + 0.5);
		    uchar g8 = floor((g6 * factor6Bit) + 0.5);
		    uchar b8 = floor((b5 * factor5Bit) + 0.5);
			
		    rgb888Data[i*rgb888Image->widthStep + j]       = r8;
		    rgb888Data[i*rgb888Image->widthStep + (j + 1)] = g8;
		    rgb888Data[i*rgb888Image->widthStep + (j + 2)] = b8;
		}
	    }
   return rgb888Image;
}

int matching(IplImage *srcImage, double* error)
{
	////////이미지에서 검출///////////
  char text [16];
  int i ;
  double result[3];
  CvFont font ;
  IplImage  *src_img1, * src_img2;
  IplImage  *dst_img[3];

  CvMat *pMatA, subMatA;
  CvMat *pMatT, subMatT;
  double phaseCorr;

  double fMax = 0;
  int matched = 0;



  src_img1 = cvCreateImage(cvSize(srcImage->width, srcImage->height), 8, 1);
  cvCopy(srcImage, src_img1);

  phaseCorr = 0;

  //템플릿 매칭
  double min=0, max=0, val;
//  CvPoint min_loc, max_loc;
  IplImage *temp;



	if((srcImage->width / srcImage->height) >= 3){
		fMax = 0.7;
		matched = 11;
	}

  for(int i = 0; i < TEMPLATE_NUM ; i++){
	for(int j = 0; j < TEMPLATE_FONT; j++){
		if(tImage[i][j] != NULL){
			IplImage *srcImageResize = cvCreateImage(cvSize(tImage[i][j]->width+1,tImage[i][j]->height+1), 8, 1);
			cvResize(srcImage,srcImageResize);

			pMatA = cvGetSubRect(srcImageResize, &subMatA, cvRect(0,0, srcImageResize->width, srcImageResize->height));
			pMatT = cvGetSubRect(tImage[i][j], &subMatT, cvRect(0,0, tImage[i][j]->width, tImage[i][j]->height));
			CvPoint min_loc, max_loc;
			CvPoint left_top;
			//템플릿 매칭
			temp = cvCreateImage(cvSize(srcImageResize->width - tImage[i][j]->width +1, srcImageResize->height - tImage[i][j]->height +1), IPL_DEPTH_32F, 1);    // 상관계수를 구할 이
			cvMatchTemplate(srcImageResize, tImage[i][j], temp, CV_TM_CCOEFF_NORMED);
			cvMinMaxLoc(temp, &min, &val, NULL, &left_top);   // blob의 좌표는 &left_top

		//	printf("phaseCorr[%d] = %f\n",i, val);
			if(val>fMax){
				fMax = val;
				matched = i;
			}

		}
	}
  }
  *error = fMax;
  return matched;

}

IplImage* img_resize(IplImage* src_img, int height)
{
    IplImage* des_img;
    float scale;
         //세로  맞춤
	 scale = height/float(src_img->height);
    des_img=cvCreateImage(cvSize((src_img->width)*scale,(src_img->height)*scale),src_img->depth,src_img->nChannels);
    cvResize(src_img,des_img,CV_INTER_LINEAR);
    return des_img;
} 

IplImage* img_resizeto_screen(IplImage* src_img)
{
    IplImage* des_img;
    float scale;
         //가로 320에 맞춤
	 scale = 320/float(src_img->width);
    des_img=cvCreateImage(cvSize((src_img->width)*scale,(src_img->height)*scale),src_img->depth,src_img->nChannels);
    cvResize(src_img,des_img,CV_INTER_LINEAR);
    return des_img;
} 


int main( int argc, char** argv )
{
	 string definedInt1,definedInt2;
	 int definedIntStartFlg = 0;
	 int definedIntEndFlg = 0;
	 string definedIntVariable;
	 IplImage* pRgbImg = cvLoadImage(argv[1], 1 );
    IplImage* pGrayImg = cvCreateImage(cvGetSize(pRgbImg),IPL_DEPTH_8U,1);
	 bool bsquare=false;
	  

    IplImage labeledImg;
    //Mat* pMatLabeled;
    //Mat MatLabeled;
    vector<KeyPoint> v;

	 string formula;
	 string strTemp = "";
	 string outFormula;
	 imshow( "Original", pRgbImg );
	
	//밝게
	//cvAddS(pRgbImg, CV_RGB(100,100,100), pRgbImg, NULL);
	
 	imshow( "Light", pRgbImg );
	 //흑백 변환 
	printf( "Gray scale Process\n" );
	cvCvtColor(pRgbImg,pGrayImg,CV_BGR2GRAY);
	imshow( "Gray", pGrayImg );
	
//히스토그램
	// cvEqualizeHist(pGrayImg, pGrayImg);
//	imshow( "Histogram", pRgbImg );

	printf( "Image resize Process\n" );
	//300 , 400, 500 ,700 ,800 ~ height resize for templete
	 pGrayImg = img_resize(pGrayImg, 400);
	 imshow( "RESIZE", pGrayImg ); //arm에선 작동안함..

	printf ("Binary Process\n" );
	 //binary Process 반전
	 cvAdaptiveThreshold(pGrayImg, pGrayImg, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 10);
    //cvThreshold(pGrayImg, pGrayImg, 10.0, 255.0, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
 	 //threshold(pGrayImg, pGrayImg, 127, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	 imshow( "Binary", pGrayImg );

	  //여백 넣기
    IplImage* pGrayImg2 = cvCreateImage(cvSize(pGrayImg->width+20, pGrayImg->height+20), 8, pGrayImg->nChannels);
	 //cvSet(pGrayImg2,CV_RGB(255,255,255));
	 cvSetImageROI(pGrayImg2, cvRect(20,20,pGrayImg->width-20,pGrayImg->height-20));
	 cvSetImageROI(pGrayImg, cvRect(10,10,pGrayImg->width-20,pGrayImg->height-20));
	 cvCopy(pGrayImg,pGrayImg2);
	 cvResetImageROI(pGrayImg2);
	 imshow( "ADD ", pGrayImg2 );
	 cvReleaseImage(&pGrayImg);
	 

    IplImage* pLabeledImg = cvCreateImage(cvSize(pGrayImg2->width, pGrayImg2->height), 8, 3);
    IplImage* pGrayImg_inv = cvCreateImage(cvSize(pGrayImg2->width, pGrayImg2->height), 8, pGrayImg2->nChannels);

		
    IplConvKernel *element1;
    element1 = cvCreateStructuringElementEx (3, 3, 2, 2, CV_SHAPE_RECT, NULL);
    IplConvKernel *element2;
    element2 = cvCreateStructuringElementEx (3, 3, 2, 2, CV_SHAPE_RECT, NULL); // 필터의 크기를 5x5로 설정
	
	printf( "Filter Process\n" );
	// 팽창/
   cvDilate( pGrayImg2, pGrayImg2, element1, 1); 
	imshow( "Dilate", pGrayImg2 );


    	  //스무스
    cvSmooth(pGrayImg2,pGrayImg2, CV_MEDIAN, 3);
    cvThreshold( pGrayImg2, pGrayImg2, 127, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
	imshow( "cvSmooth", pGrayImg2 );
	// 침식
    cvErode(pGrayImg2, pGrayImg2, element2, 1);  
	imshow( "Erode", pGrayImg2 );
    	//반전에 반전 (원본)
   cvNot(pGrayImg2, pGrayImg_inv);
   cvCvtColor( pGrayImg_inv, pLabeledImg, CV_GRAY2RGB);
	
	imshow( "Filtered", pGrayImg_inv );

	printf ("Labeling Process\n" );
	CBlobLabeling blob;
   blob.SetParam( pGrayImg2, 5 );

   blob.DoLabeling();
	CvScalar color = cvScalar(255, 0, 0);

	printf( "Load templete\n" );
	if((tImage[0][0] = cvLoadImage("ocr_templete/0_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[1][0] = cvLoadImage("ocr_templete/1_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[2][0] = cvLoadImage("ocr_templete/2_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[3][0] = cvLoadImage("ocr_templete/3_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[4][0] = cvLoadImage("ocr_templete/4_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[5][0] = cvLoadImage("ocr_templete/5_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[6][0] = cvLoadImage("ocr_templete/6_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[7][0] = cvLoadImage("ocr_templete/7_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[8][0] = cvLoadImage("ocr_templete/8_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[9][0] = cvLoadImage("ocr_templete/9_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[10][0] = cvLoadImage("ocr_templete/plus_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[11][0] = cvLoadImage("ocr_templete/minus_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
	if((tImage[12][0] = cvLoadImage("ocr_templete/divide_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[13][0] = cvLoadImage("ocr_templete/pi_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[13][1] = cvLoadImage("ocr_templete/pi_2.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[14][0] = cvLoadImage("ocr_templete/bracket_l_2.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[14][1] = cvLoadImage("ocr_templete/bracket_l_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[15][0] = cvLoadImage("ocr_templete/bracket_r_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[16][0] = cvLoadImage("ocr_templete/A.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[17][0] = cvLoadImage("ocr_templete/L.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[18][0] = cvLoadImage("ocr_templete/t_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[18][1] = cvLoadImage("ocr_templete/t_2.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[19][0] = cvLoadImage("ocr_templete/i_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[20][0] = cvLoadImage("ocr_templete/c_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[21][0] = cvLoadImage("ocr_templete/s_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[22][0] = cvLoadImage("ocr_templete/o_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[23][0] = cvLoadImage("ocr_templete/dot.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[24][0] = cvLoadImage("ocr_templete/m_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[25][0] = cvLoadImage("ocr_templete/n_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[26][0] = cvLoadImage("ocr_templete/d_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[27][0] = cvLoadImage("ocr_templete/e_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[28][0] = cvLoadImage("ocr_templete/f_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[29][0] = cvLoadImage("ocr_templete/dx_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	//if((tImage[30][0] = cvLoadImage("ocr_templete/diff_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[31][0] = cvLoadImage("ocr_templete/Large_c_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[32][0] = cvLoadImage("ocr_templete/arrow_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[33][0] = cvLoadImage("ocr_templete/y_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[34][0] = cvLoadImage("ocr_templete/x_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[35][0] = cvLoadImage("ocr_templete/a_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[36][0] = cvLoadImage("ocr_templete/integral_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[37][0] = cvLoadImage("ocr_templete/sigma_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[38][0] = cvLoadImage("ocr_templete/root_1.bmp" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	//템플릿 이진화.

	for(int i = 0; i < TEMPLATE_NUM; i++)  {
		for(int j = 0; j < TEMPLATE_FONT; j++){
			if(tImage[i][j] != NULL){
				cvThreshold(tImage[i][j], tImage[i][j], 127.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
			}
		}
	}

	printf( "Templete matching...\n" );

    for(int i = 0; i < blob.m_nBlobs; i++)
    {
    	CvPoint pt1 = cvPoint(blob.m_recBlobs[i].x-5, blob.m_recBlobs[i].y-5);
    	CvPoint pt2 = cvPoint(pt1.x + blob.m_recBlobs[i].width + 10, pt1.y + blob.m_recBlobs[i].height + 10);
    	if((blob.m_recBlobs[i].x-5<= 0)||(blob.m_recBlobs[i].y-5<= 0)){
    		//size error
    	} else{
			//각 레이블 표시
			// 이미지 매칭
			IplImage* pLabeledImg_obj = cvCreateImage(cvSize(pt2.x-pt1.x, pt2.y-pt1.y), 8, pGrayImg_inv->nChannels);
			
			int nMatchValue;
			double error;
			cvSetImageROI(pGrayImg_inv,cvRect(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y));
			
			cvCopy(pGrayImg_inv, pLabeledImg_obj);
			cvResetImageROI(pGrayImg_inv);

		  	  //여기서 위치정보 판별
			objCenterPosition[i][0] = (pt2.x-pt1.x)/2 + pt1.x;
			objCenterPosition[i][1] = (pt2.y-pt1.y)/2 + pt1.y;
			nMatchValue = matching(pLabeledImg_obj, &error);
			
			cvDrawRect(pLabeledImg, pt1, pt2, color ,6);
			//CvFont point_font;
			//cvInitFont(&point_font,CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, 8);
			char text[20];
			string letter = "";
			
			if((objCenterPosition[0][1]-objCenterPosition[i][1])>30){ // 자승 처리
				if(!bsquare){				
				formula += "^";
				bsquare = true;
				}
			} else if(bsquare){
				bsquare = false;
				formula += " ";
			}
			printf("%d ",objCenterPosition[i][1]);
			
			switch(nMatchValue){

			case 0:
						
					letter =  "0";
					formula +=  "0";
					break;
			case 1:
					letter =  "1";
					formula +=  "1";
					break;
			case 2:
					letter =  "2";
					formula +=  "2";
					break;
			case 3:
					letter =  "3";
					formula +=  "3";
					break;
			case 4:
					letter =  "4";
					formula +=  "4";
					break;
			case 5:
					letter =  "5";
					formula +=  "5";
					break;
			case 6:
					letter =  "6";
					formula +=  "6";
					break;
			case 7:
					letter =  "7";
					formula +=  "7";
					break;
			case 8:
					letter =  "8";
					formula +=  "8";
					break;
			case 9:
					letter =  "9";
					formula +=  "9";
					break;
			case 10:
					letter =  "+";
					formula +=  "+";
					break;
			case 11:
					letter =  "-";
					if(((objCenterPosition[i][0] - objCenterPosition[i-1][0])<5)&&((objCenterPosition[i][0] - objCenterPosition[i-1][0])>-5)){
						if(formula.length()>0){
						formula.erase(formula.length() -1 ,1 );
						}
						formula +=  "=";
					}
					else
						formula +=  "-";
					break;
			case 12:
					letter =  "/";
					formula +=  "/";
					break;
			case 13:
					letter =  "pi";
					formula +=  "pi";
					break;
			case 14:
					letter =  "(";
					formula +=  "(";
					break;
			case 15:
					letter =  ")";
					formula +=  ")";
					break;

			case 16:
					letter =  "A";
					formula +=  "A";
					break;
			case 17:
					letter =  "L";
					formula +=  "L";
					break;
			case 18:
					letter =  "t";
					formula +=  "t";
					if(formula.length()>0){
					strTemp = formula.substr(formula.length()-1,formula.length());
					}
					//if(strTemp == "d")
						//definedIntEndFlg = 1;
						definedIntVariable = "t";
					break;
			case 19:
					letter =  "I";
					formula +=  "I";
					break;
			case 20:
					letter =  "C";
					if(formula.length()>1){
						strTemp = formula.substr(formula.length()-2,formula.length());
						if((strTemp=="Se")||(strTemp=="SC")){
							if(formula.length()>1){
							formula.erase(formula.length() -2 ,2 );
							}
							formula +=  "sec";
						}
						else
						formula +=  "C";
						} else {
							formula +=  "C";
						}
					break;
			case 21:
					letter =  "S";
					if(formula.length()>1){
					strTemp = formula.substr(formula.length()-2,formula.length());
					if((strTemp=="CO")||(strTemp=="C0")||(strTemp=="cO")||(strTemp=="c0")){
						if(formula.length()>1){
						formula.erase(formula.length() -2 ,2 );
						}
						formula +=  "cos";
					}
					else
						formula +=  "S";
					} else {
					formula +=  "S";
					}
					break;
			case 22:
					letter =  "O";
					formula +=  "O";
					break;
			case 23:
					letter =  ".";
					if(((objCenterPosition[i][0] - objCenterPosition[i-1][0])<10)&&((objCenterPosition[i][0] - objCenterPosition[i-1][0])>-10)){
						if(formula.length()>1){
						formula.erase(formula.length() -2 ,2 );
						}
						formula +=  "i";
						bsquare = false;
					}
					else
						formula +=  ".";
					if(formula.length()>=6){
						strTemp = formula.substr(formula.length()-6,formula.length());
						if(strTemp == ".^.^.^"){
							if(formula.length()>5){
						formula.erase(formula.length() -6 ,6 );
						}
							formula +=  "integral";
							objCenterPosition[0][0] = objCenterPosition[i-1][0];
						}

					}
					break;
			case 24:
					letter =  "m";
					formula +=  "m";
					break;
			case 25:
					letter =  "n";
					if(formula.length()>1){
						strTemp = formula.substr(formula.length()-2,formula.length());
						if((strTemp=="SI")||(strTemp=="Si")||(strTemp=="sI")||(strTemp=="si")||(strTemp=="S1")||(strTemp=="s1")){
							if(formula.length()>1){
							formula.erase(formula.length() -2 ,2 );
							}
							formula +=  "sin";
						}
						else
						formula +=  "n";
					} else {
						formula +=  "n";					
					}
					break;
			case 26:
					letter =  "d";
					formula +=  "d";
					break;
			case 27:
					letter =  "e";
					formula +=  "e";
					break;
			case 28:
					letter =  "f";
					formula +=  "f";
					break;
			case 29:
					letter =  "dx";
					formula +=  "dx";
					//definedIntEndFlg = 1;
					break;
			case 30:
					letter =  "'";
					/*if(formula.length()>0){
						formula.erase(formula.length() -1 ,1 );
						}*/
					formula +=  "'";
					break;
			case 31:
					letter =  "C";
					if(formula.length()>0){
						strTemp = formula.substr(formula.length()-1,formula.length());
						if((strTemp=="SC")||(strTemp=="Se")){
							if(formula.length()>1){
							formula.erase(formula.length() -2 ,2 );
							}
							formula +=  "sec";
						}
						else
							formula +=  "C";
					} else {
						formula +=  "C";
					}
					break;
			case 32:
					letter =  "->";
					formula +=  "->";
					break;
			case 33:
					letter =  "y";
					formula +=  "y";
					if(formula.length()>0){
					strTemp = formula.substr(formula.length()-1,formula.length());
					}
					//if(strTemp == "d")
						//definedIntEndFlg = 1;
						definedIntVariable = "y";
					break;
			case 34:
					letter =  "x";
					formula +=  "x";
					if(formula.length()>0){
					strTemp = formula.substr(formula.length()-1,formula.length());
					}
					//if(strTemp == "d")
						//definedIntEndFlg = 1;
						definedIntVariable = "x";
					break;
			case 35:
					letter =  "a";
					formula +=  "a";
					break;
			case 36:
					letter =  "integral";
					formula +=  "integral ";
					definedIntStartFlg = 1;
					break;
			case 37:
					letter =  "sigma";
					formula +=  "sigma ";
					break;
			case 38:
					letter =  "root";
					formula +=  "root ";
					break;
			}
			//printf("%s",letter);
////////////////////////////////////////////////////////////////////////////////
			if(((objCenterPosition[0][1]-objCenterPosition[i][1])>40)&&definedIntStartFlg){ // 정적분
				if(formula.length()>10){
					strTemp = formula.substr(formula.length()-10,formula.length()-1);
				}
				if(formula.length()>1){
						formula.erase(formula.length() -2 ,2 );
				}
				//if(strTemp == "integral ")
						cout<<letter<<endl;
						definedInt1 = letter;
						cout<<definedInt1<<endl;
					//letter =  ")");
					definedIntStartFlg = 0;
					definedIntEndFlg = 1;
			}else if(((objCenterPosition[0][1]-objCenterPosition[i][1])<-40)&&definedIntStartFlg){ // 정적분
				if(formula.length()>10){
					
					strTemp = formula.substr(formula.length()-10,formula.length()-1);
				}
				if(formula.length()>0){
						formula.erase(formula.length() -1 ,1 );
					}
				//if(strTemp == "integral ")
				definedInt2 = letter;
			}else if(definedIntStartFlg){
				
				if(nMatchValue != 36) definedIntStartFlg = 0;
				definedIntEndFlg = 0;
			}
/////////////////////////////////////////////////////////////////////////////////
			sprintf(text,"[%s]",letter.c_str());
			pt1.y = pt1.y-5;
			//cvPutText(pLabeledImg,text, pt1, &point_font, CV_RGB(255,0,255));

			//cvInitFont(&point_font,CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, 8);
			sprintf(text,"[%.1f]",error*100);
			pt1.y = pt1.y-20;
			//cvPutText(pLabeledImg,text, pt1, &point_font, CV_RGB(0,255,0));

    	}
    }

    if(definedIntEndFlg){
    	formula += " from " + definedIntVariable + "=" + definedInt1 + " to " + definedInt2;
    }


	//printf("\n");
	//*pMatGr = pLabeledImg;
	imshow( "Labeled", pLabeledImg );
	pLabeledImg = img_resizeto_screen(pLabeledImg);
	imshow( "Result", pLabeledImg );
	//레이블 결과 저장
	cvSaveImage("lable_result.bmp", pLabeledImg);

	cout << formula << endl;
  waitKey(0);
	cvReleaseImage(&pGrayImg2);
	cvReleaseImage(&pLabeledImg);
  return 0;
}
