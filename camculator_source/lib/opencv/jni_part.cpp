#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <android/log.h>
#include <string>
#include "BlobLabeling.h"

using namespace std;
using namespace cv;

#define TEMPLATE_NUM 39
#define TEMPLATE_FONT 1

IplImage *tImage[TEMPLATE_NUM][2];
int objCenterPosition[TEMPLATE_NUM][2];
string definedInt[2];
int definedIntStartFlg = 0;
int definedIntEndFlg = 0;
string definedIntVariable;

extern "C" {

double PhaseCorrelation(const CvMat *pMatA, const CvMat *pTempl, CvPoint *pMaxLoc)
{
	// real part
	CvMat *pMatRe = cvCreateMat(pMatA->rows + pTempl->rows + 1, pMatA->cols + pTempl->cols + 1, CV_32FC1);

	// imaginary part#define TEMPLATE_NUM 23

	IplImage *tImage[20];

	CvMat *pMatIm = cvCreateMat(pMatRe->rows, pMatRe->cols, pMatRe->type);
	CvMat *pDFT_A = cvCreateMat(pMatRe->rows, pMatRe->cols, CV_32FC2);
	CvMat *pDFT_B = cvCreateMat(pMatRe->rows, pMatRe->cols, pDFT_A->type);

	//step 1: Calculate pDFT_A of the input, pMatA
	//copy and 0-padding
	cvZero(pMatRe);
	CvMat *pMatSubReA, subMatA;
	pMatSubReA = cvGetSubRect(pMatRe, &subMatA, cvRect(0, 0, pMatA->cols, pMatA->rows));
	cvScale(pMatA, pMatSubReA);

	cvZero(pMatIm);
	cvMerge(pMatRe, pMatIm, NULL, NULL, pDFT_A);
	cvDFT(pDFT_A, pDFT_A, CV_DXT_FORWARD, pMatA->rows);

	//step 2: Calculate pDFT_B of the input, pMatB
	cvZero(pMatRe);
	CvMat *pMatSubReB, subMatB;
	pMatSubReB = cvGetSubRect(pMatRe, &subMatB, cvRect(0, 0, pTempl->cols, pTempl->rows));
	cvScale(pTempl, pMatSubReB);

	cvMerge(pMatRe, pMatIm, NULL, NULL, pDFT_B);
	cvDFT(pDFT_B, pDFT_B, CV_DXT_FORWARD, pTempl->rows);

	//step 3: pDFT_A = pDFT_A * CONJ(pDFT_B)
	cvMulSpectrums(pDFT_A, pDFT_B, pDFT_A, CV_DXT_MUL_CONJ);

	//step 4: Normalize by spectrum
	cvSplit(pDFT_A, pMatRe, pMatIm, NULL, NULL);

	CvMat *pMatT1 = cvCreateMat(pMatRe->rows, pMatRe->cols, pMatRe->type);
	CvMat *pMatT2 = cvCreateMat(pMatRe->rows, pMatRe->cols, pMatRe->type);

	// Calcilate spectrum
	cvPow(pMatRe, pMatT1, 2.0);
	cvPow(pMatIm, pMatT2, 2.0);
	cvAdd(pMatT1, pMatT2, pMatT1);
	cvPow(pMatT1, pMatT2, 0.5); //root

	cvDiv(pMatRe, pMatT2, pMatRe);
	cvDiv(pMatIm, pMatT2, pMatIm);

	cvReleaseMat(&pMatT1);
	cvReleaseMat(&pMatT2);

	cvMerge(pMatRe, pMatIm, NULL, NULL, pDFT_A);

	//step 5:
	//A = IDFT(pDFT_A* conj(pDFT_B)/(|pDFT_A * conj(pDFT_B)|))
	cvDFT(pDFT_A, pDFT_A, CV_DXT_INV_SCALE, 0);
	cvSplit(pDFT_A, pMatRe, pMatIm, NULL, NULL);
	double min_val, max_val;
	cvMinMaxLoc(pMatRe, &min_val, &max_val, NULL, pMaxLoc);

	/*
	//for showing the phase correlation
	IplImage *corrImage;
	corrImage = cvCreateImage(cvGetSize(pMatA), IPL_DEPTH_8U, 1);
	// Color Image
	cvNamedWindow("corrImage",CV_WINDOW_AUTOSIZE);
	cvShowImage("corrImage",corrImage);

	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&corrImage);
	////
	*/
	cvReleaseMat(&pMatRe);
	cvReleaseMat(&pMatIm);
	cvReleaseMat(&pDFT_A);
	cvReleaseMat(&pDFT_B);

	return max_val;


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

//  if((tImage = cvLoadImage("img/2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL){
//    return  -1;
//  }


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

			/*
				if(srcImageResize->width < tImage[i]->width)
					break;
				if(srcImageResize->height < tImage[i]->height)
					break;
			*/

			CvPoint min_loc, max_loc;
			CvPoint left_top;
		/*
			//주파수영역 매칭
			phaseCorr = PhaseCorrelation(pMatA, pMatT, &max_loc);
		//	printf("phaseCorr[%d] = %f\n",i, phaseCorr);
		//	printf("max_loc[%d] = (%d,%d)\n",i,  max_loc.x, max_loc.y);

			if(phaseCorr>fMax){
				fMax = phaseCorr;
				matched = i;
			}

		 */
			//템플릿 매칭
			temp = cvCreateImage(cvSize(srcImageResize->width - tImage[i][j]->width +1, srcImageResize->height - tImage[i][j]->height +1), IPL_DEPTH_32F, 1);    // 상관계수를 구할 이
			cvMatchTemplate(srcImageResize, tImage[i][j], temp, CV_TM_CCOEFF_NORMED);
			cvMinMaxLoc(temp, &min, &val, NULL, &left_top);   // blob의 좌표는 &left_top

			//	printf("phaseCorr[%d] = %f\n",i, phaseCorr);
		//	printf("max_loc[%d] = (%d,%d)\n",i,  max_loc.x, max_loc.y);



			printf("phaseCorr[%d] = %f\n",i, val);
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


JNIEXPORT jstring JNICALL Java_kr_ac_ajou_ddozzip_calmera_OpencvActivity_FindFeatures(JNIEnv* env, jobject obj, jlong addrGray, jlong addrRgba)
{
	int g_switch_value = 0;
	jstring outFormula;

    Mat* pMatRgb=(Mat*)addrRgba;
    Mat* pMatGr=(Mat*)addrGray;



    Mat MatRgb = *pMatRgb;
    Mat MatGr = *pMatGr;


    IplImage rgbImg = MatRgb;
    IplImage grayImg = MatGr;
    IplImage* pGrayImg = &grayImg;
    IplImage* pRgbImg = &rgbImg;

    IplImage labeledImg;
    Mat* pMatLabeled;
    Mat MatLabeled;
    vector<KeyPoint> v;

	string formula;
	string strTemp = "";

    //binary Process 반전
    cvtColor(*pMatRgb, *pMatGr, CV_BGR2GRAY);
    threshold( *pMatGr, *pMatGr, 127.0, 255.0, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

    //IplImage로 변환
    MatGr = *pMatGr;
    grayImg = Mat(MatGr);
    pGrayImg = &grayImg;

    IplImage* pLabeledImg = cvCreateImage(cvSize(pGrayImg->width, pGrayImg->height), 8, 3);
    IplImage* pGrayImg_inv = cvCreateImage(cvSize(pGrayImg->width, pGrayImg->height), 8, pGrayImg->nChannels);


    IplConvKernel *element1;
    element1 = cvCreateStructuringElementEx (3, 3, 2, 2, CV_SHAPE_RECT, NULL);
    IplConvKernel *element2;
    element2 = cvCreateStructuringElementEx (3, 3, 2, 2, CV_SHAPE_RECT, NULL); // 필터의 크기를 5x5로 설정

    cvDilate( pGrayImg, pGrayImg, element1, 1); // 팽창

    //스무스
    cvSmooth(pGrayImg,pGrayImg, CV_MEDIAN, 3);
    cvThreshold( pGrayImg, pGrayImg, 127.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);

    cvErode(pGrayImg, pGrayImg, element2, 1);  // 침식



//    cvSmooth(pGrayImg,pGrayImg, CV_GAUSSIAN, 3);
//    cvThreshold( pGrayImg, pGrayImg, 127.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);

    //반전에 반전 (원본)
    cvNot(pGrayImg,pGrayImg_inv);
    cvCvtColor( pGrayImg_inv, pLabeledImg, CV_GRAY2RGB);

    CBlobLabeling blob;
    blob.SetParam( pGrayImg, 20 );

    blob.DoLabeling();

	CvScalar color = cvScalar(255, 0, 0);

	if((tImage[0][0] = cvLoadImage("/sdcard/calmera_img/0_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[0][1] = cvLoadImage("/sdcard/calmera_img/0_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[1][0] = cvLoadImage("/sdcard/calmera_img/1_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[1][1] = cvLoadImage("/sdcard/calmera_img/1_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[2][0] = cvLoadImage("/sdcard/calmera_img/2_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[2][1] = cvLoadImage("/sdcard/calmera_img/2_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[3][0] = cvLoadImage("/sdcard/calmera_img/3_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[3][1] = cvLoadImage("/sdcard/calmera_img/3_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[4][0] = cvLoadImage("/sdcard/calmera_img/4_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[4][1] = cvLoadImage("/sdcard/calmera_img/4_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[5][0] = cvLoadImage("/sdcard/calmera_img/5_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[5][1] = cvLoadImage("/sdcard/calmera_img/5_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[6][0] = cvLoadImage("/sdcard/calmera_img/6_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[6][1] = cvLoadImage("/sdcard/calmera_img/6_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[7][0] = cvLoadImage("/sdcard/calmera_img/7_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[7][1] = cvLoadImage("/sdcard/calmera_img/7_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[8][0] = cvLoadImage("/sdcard/calmera_img/8_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[8][1] = cvLoadImage("/sdcard/calmera_img/8_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[9][0] = cvLoadImage("/sdcard/calmera_img/9_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[9][1] = cvLoadImage("/sdcard/calmera_img/9_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[10][0] = cvLoadImage("/sdcard/calmera_img/plus_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[10][1] = cvLoadImage("/sdcard/calmera_img/plus_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[11][0] = cvLoadImage("/sdcard/calmera_img/minus_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;
//	if((tImage[11][1] = cvLoadImage("/sdcard/calmera_img/minus_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL) ;

	if((tImage[12][0] = cvLoadImage("/sdcard/calmera_img/divide_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[12][1] = cvLoadImage("/sdcard/calmera_img/divide_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[13][0] = cvLoadImage("/sdcard/calmera_img/pi_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
	if((tImage[13][1] = cvLoadImage("/sdcard/calmera_img/pi_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[14][0] = cvLoadImage("/sdcard/calmera_img/bracket_l_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[14][1] = cvLoadImage("/sdcard/calmera_img/bracket_l_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[15][0] = cvLoadImage("/sdcard/calmera_img/bracket_r_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[15][1] = cvLoadImage("/sdcard/calmera_img/bracket_r_2.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[16][0] = cvLoadImage("/sdcard/calmera_img/A.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[16][1] = cvLoadImage("/sdcard/calmera_img/A.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[17][0] = cvLoadImage("/sdcard/calmera_img/L.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[17][1] = cvLoadImage("/sdcard/calmera_img/L.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[18][0] = cvLoadImage("/sdcard/calmera_img/t_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[18][1] = cvLoadImage("/sdcard/calmera_img/t.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[19][0] = cvLoadImage("/sdcard/calmera_img/i_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[19][1] = cvLoadImage("/sdcard/calmera_img/I.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[20][0] = cvLoadImage("/sdcard/calmera_img/c_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[20][1] = cvLoadImage("/sdcard/calmera_img/C.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[21][0] = cvLoadImage("/sdcard/calmera_img/s_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[21][1] = cvLoadImage("/sdcard/calmera_img/S.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[22][0] = cvLoadImage("/sdcard/calmera_img/o_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[22][1] = cvLoadImage("/sdcard/calmera_img/O.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[23][0] = cvLoadImage("/sdcard/calmera_img/dot.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);
//	if((tImage[23][1] = cvLoadImage("/sdcard/calmera_img/dot.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[24][0] = cvLoadImage("/sdcard/calmera_img/m_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[25][0] = cvLoadImage("/sdcard/calmera_img/n_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[26][0] = cvLoadImage("/sdcard/calmera_img/d_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[27][0] = cvLoadImage("/sdcard/calmera_img/e_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[28][0] = cvLoadImage("/sdcard/calmera_img/f_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[29][0] = cvLoadImage("/sdcard/calmera_img/dx_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[30][0] = cvLoadImage("/sdcard/calmera_img/diff_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[31][0] = cvLoadImage("/sdcard/calmera_img/Large_c_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[32][0] = cvLoadImage("/sdcard/calmera_img/arrow_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[33][0] = cvLoadImage("/sdcard/calmera_img/y_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[34][0] = cvLoadImage("/sdcard/calmera_img/x_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[35][0] = cvLoadImage("/sdcard/calmera_img/a_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[36][0] = cvLoadImage("/sdcard/calmera_img/integral_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[37][0] = cvLoadImage("/sdcard/calmera_img/sigma_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);

	if((tImage[37][0] = cvLoadImage("/sdcard/calmera_img/root_1.jpg" , CV_LOAD_IMAGE_GRAYSCALE )) == NULL);


	for(int i = 0; i < TEMPLATE_NUM; i++)  {
		for(int j = 0; j < TEMPLATE_FONT; j++){
			if(tImage[i][j] != NULL){
				cvThreshold(tImage[i][j], tImage[i][j], 127.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
			}
		}
	}

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

			/*
			__android_log_print(ANDROID_LOG_INFO,"pLabeledImg_obj->height:", pLabeledImg_obj->height);
			__android_log_print(ANDROID_LOG_INFO,"pLabeledImg_obj->width:", pLabeledImg_obj->width);

			__android_log_print(ANDROID_LOG_INFO,"pt1.x:", pt1.x);
			__android_log_print(ANDROID_LOG_INFO,"pt1.y:", pt1.y);
			__android_log_print(ANDROID_LOG_INFO,"pt2.x:", pt2.x);
			__android_log_print(ANDROID_LOG_INFO,"pt2.y:", pt2.y);


			__android_log_print(ANDROID_LOG_INFO,"jni","pGrayImg_inv->nChannels:" + 123);
			__android_log_print(ANDROID_LOG_INFO,"jni","pGrayImg_inv->nChannels:" + 1232);
	*/

			cvSetImageROI(pGrayImg_inv,cvRect(pt1.x,pt1.y,pt2.x-pt1.x,pt2.y-pt1.y));

			cvCopy(pGrayImg_inv, pLabeledImg_obj);
			cvResetImageROI(pGrayImg_inv);
/*
			string makeTamplate = "/sdcard/calmera_template/";
			//makeTamplate += i;
			makeTamplate += i;
			makeTamplate += "_3.jpg";

			__android_log_print(ANDROID_LOG_INFO,"jni","saving...");
			cvSaveImage(makeTamplate.c_str(), pLabeledImg_obj);
			__android_log_print(ANDROID_LOG_INFO,"jni","saved!!!");
*/

		  	  //여기서 위치정보 판별
			objCenterPosition[i][0] = (pt2.x-pt1.x)/2 + pt1.x;
			objCenterPosition[i][1] = (pt2.y-pt1.y)/2 + pt1.y;
			nMatchValue = matching(pLabeledImg_obj, &error);

			cvDrawRect(pLabeledImg, pt1, pt2, color);

			CvFont point_font;
			cvInitFont(&point_font,CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, 8);
			char text[20];
			char letter[5];

			if((objCenterPosition[0][1]-objCenterPosition[i][1])>40){ // 자승 처리
				formula += "^";
			}


			switch(nMatchValue){

			case 0:
					strcpy(letter, "0");
					formula +=  "0";
					break;
			case 1:
					strcpy(letter, "1");
					formula +=  "1";
					break;
			case 2:
					strcpy(letter, "2");
					formula +=  "2";
					break;
			case 3:
					strcpy(letter, "3");
					formula +=  "3";
					break;
			case 4:
					strcpy(letter, "4");
					formula +=  "4";
					break;
			case 5:
					strcpy(letter, "5");
					formula +=  "5";
					break;
			case 6:
					strcpy(letter, "6");
					formula +=  "6";
					break;
			case 7:
					strcpy(letter, "7");
					formula +=  "7";
					break;
			case 8:
					strcpy(letter, "8");
					formula +=  "8";
					break;
			case 9:
					strcpy(letter, "9");
					formula +=  "9";
					break;
			case 10:
					strcpy(letter, "+");
					formula +=  "+";
					break;
			case 11:
					strcpy(letter, "-");
					if(((objCenterPosition[i][0] - objCenterPosition[i-1][0])<5)&&((objCenterPosition[i][0] - objCenterPosition[i-1][0])>-5)){
						formula.erase(formula.length() -1 ,1 );
						formula +=  "=";
					}
					else
						formula +=  "-";
					break;
			case 12:
					strcpy(letter, "/");
					formula +=  "/";
					break;
			case 13:
					strcpy(letter, "pi");
					formula +=  "pi";
					break;
			case 14:
					strcpy(letter, "(");
					formula +=  "(";
					break;
			case 15:
					strcpy(letter, ")");
					formula +=  ")";
					break;

			case 16:
					strcpy(letter, "A");
					formula +=  "A";
					break;
			case 17:
					strcpy(letter, "L");
					formula +=  "L";
					break;
			case 18:
					strcpy(letter, "t");
					formula +=  "t";
					strTemp = formula.substr(formula.length()-1,formula.length());
					//if(strTemp == "d")
						//definedIntEndFlg = 1;
						definedIntVariable = "t";
					break;
			case 19:
					strcpy(letter, "I");
					formula +=  "I";
					break;
			case 20:
					strcpy(letter, "C");
					strTemp = formula.substr(formula.length()-2,formula.length());
					if((strTemp=="Se")||(strTemp=="SC")){
						formula.erase(formula.length() -2 ,2);
						formula +=  "sec";
					}
					else
					formula +=  "C";
					break;
			case 21:
					strcpy(letter, "S");
					strTemp = formula.substr(formula.length()-2,formula.length());
					if((strTemp=="CO")||(strTemp=="C0")||(strTemp=="cO")||(strTemp=="c0")){
						formula.erase(formula.length() -2 ,2);
						formula +=  "cos";
					}
					else
						formula +=  "S";
					break;
			case 22:
					strcpy(letter, "O");
					formula +=  "O";
					break;
			case 23:
					strcpy(letter, ".");

					if(((objCenterPosition[i][0] - objCenterPosition[i-1][0])<10)&&((objCenterPosition[i][0] - objCenterPosition[i-1][0])>-10)){
						formula.erase(formula.length() - 2, 2);
						formula +=  "i";
					}
					else
						formula +=  ".";

					if(formula.length()>=6){
						strTemp = formula.substr(formula.length()-6,formula.length());
						if(strTemp == ".^.^.^"){
							formula.erase(formula.length() -6 ,6);
							formula +=  "integral";
							objCenterPosition[0][0] = objCenterPosition[i-1][0];
						}

					}

					break;
			case 24:
					strcpy(letter, "m");
					formula +=  "m";
					break;
			case 25:
					strcpy(letter, "n");
					strTemp = formula.substr(formula.length()-2,formula.length());
					if((strTemp=="SI")||(strTemp=="Si")||(strTemp=="sI")||(strTemp=="si")||(strTemp=="S1")||(strTemp=="s1")){
						formula.erase(formula.length() -2 ,2);
						formula +=  "sin";
					}
					else
					formula +=  "n";
					break;
			case 26:
					strcpy(letter, "d");
					formula +=  "d";
					break;
			case 27:
					strcpy(letter, "e");
					formula +=  "e";
					break;
			case 28:
					strcpy(letter, "f");
					formula +=  "f";
					break;
			case 29:
					strcpy(letter, "dx");
					formula +=  "dx";
					//definedIntEndFlg = 1;
					break;
			case 30:
					strcpy(letter, "'");
					formula.erase(formula.length() -1 ,1 );
					formula +=  "'";
					break;
			case 31:
					strcpy(letter, "C");
					strTemp = formula.substr(formula.length()-1,formula.length());
					if((strTemp=="SC")||(strTemp=="Se")){
						formula.erase(formula.length() -2 ,2);
						formula +=  "sec";
					}
					else
						formula +=  "C";
					break;
			case 32:
					strcpy(letter, "->");
					formula +=  "->";
					break;
			case 33:
					strcpy(letter, "y");
					formula +=  "y";
					strTemp = formula.substr(formula.length()-1,formula.length());
					//if(strTemp == "d")
						//definedIntEndFlg = 1;
						definedIntVariable = "y";
					break;
			case 34:
					strcpy(letter, "x");
					formula +=  "x";
					strTemp = formula.substr(formula.length()-1,formula.length());
					//if(strTemp == "d")
						//definedIntEndFlg = 1;
						definedIntVariable = "x";
					break;
			case 35:
					strcpy(letter, "a");
					formula +=  "a";
					break;
			case 36:
					strcpy(letter, "integral");
					formula +=  "integral ";
					definedIntStartFlg = 1;
					break;
			case 37:
					strcpy(letter, "sigma");
					formula +=  "sigma ";
					break;
			case 38:
					strcpy(letter, "root");
					formula +=  "root ";
					break;
			}

			if(((objCenterPosition[0][1]-objCenterPosition[i][1])>40)&&definedIntStartFlg){ // 정적분
				strTemp = formula.substr(formula.length()-10,formula.length()-1);
				formula.erase(formula.length() - 2, 2);
				//if(strTemp == "integral ")
					definedInt[0] = letter;
					definedIntStartFlg = 0;
					definedIntEndFlg = 1;
			}else if(((objCenterPosition[0][1]-objCenterPosition[i][1])<-40)&&definedIntStartFlg){ // 정적분
				strTemp = formula.substr(formula.length()-10,formula.length()-1);
				formula.erase(formula.length() - 1, 1);
				//if(strTemp == "integral ")
					definedInt[1] = letter;
			}else if(definedIntStartFlg){
				if(nMatchValue != 36) definedIntStartFlg = 0;
				definedIntEndFlg = 0;
			}


			sprintf(text,"[%s]",letter);
			pt1.y = pt1.y-5;
			cvPutText(pLabeledImg,text, pt1, &point_font, CV_RGB(255,0,255));

			cvInitFont(&point_font,CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, 8);
			sprintf(text,"[%.1f]",error*100);
			pt1.y = pt1.y-20;
			cvPutText(pLabeledImg,text, pt1, &point_font, CV_RGB(0,255,0));

/*
			cvInitFont(&point_font,CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, 8);
			sprintf(text,"[%d, %d]",objCenterPosition[i][0],objCenterPosition[i][1]);
			pt1.y = pt1.y-40;
			cvPutText(pLabeledImg,text, pt1, &point_font, CV_RGB(0,0,255));
*/
    	}
		//cvWaitKey ( 0 );

    }



    if(definedIntEndFlg){
    	formula += " from " + definedIntVariable + "=" + definedInt[1] + " to " + definedInt[0];
    }

	outFormula = env->NewStringUTF(formula.c_str());
	//env->ReleaseStringUTFChars(outFormula, "hell...");
	*pMatGr = pLabeledImg;


	return outFormula;

}

}

