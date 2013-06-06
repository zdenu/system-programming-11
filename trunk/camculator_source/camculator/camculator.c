#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gx.h>
#include <gxjpg.h>
#include <gxpng.h>
#include "touch.h"
#include "camera.h"

/** ----------------------------------------------------------------------------
@brief  버퍼 크기 정의
@remark
 -----------------------------------------------------------------------------*/
#define MAX_BUFF_SIZE   1024
#define MAX_DEV_NANE    100


/** ----------------------------------------------------------------------------
@brief  지역 변수 정의
@remark
 -----------------------------------------------------------------------------*/
static dc_t         *dc_screen;                                        
static dc_t         *dc_buffer;                                         
static dc_t			*before_screen;								
static font_t 		*font14;
static font_t 		*font18;

static char  buff[MAX_BUFF_SIZE];                                       
static char  fb_dev_name[MAX_DEV_NANE];

#define HOME		0
#define CAMERA		1
#define CROP		2
#define LABELING	3
#define EDIT		4
#define RESULT		5
#define START		1
#define END			0


int fontloader14(char* file){
	char buff[30];
	sprintf(buff,"font/14pt/%s",file);
	printf( "font loading\n");
	if ( NULL == ( font14 = gx_open_font(buff) )  ) 
		return 1;
}
int fontloader18(char* file){
	char buff[30];
	sprintf(buff,"font/18pt/%s",file);
	printf( "font loading\n");
	if ( NULL == ( font18 = gx_open_font(buff) )   )
		return 1;
}


static void interface_Background(int mode){
	png_t   *back;
	back = gx_png_create( 320, 240);
	switch(mode){
	case HOME : 
		back = gx_png_open( "interface/background/home.png");
		//display history//
		gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		dc_buffer->font	= font14;
		dc_buffer->font_color	= gx_color( 0, 0, 0, 255);
		//sprintf( buff, "%s", msg);
		gx_text_out( dc_buffer, 58, 115, "lim (15x^2/621x)");
		gx_text_out( dc_buffer, 58, 146, "integral(4x+2)");
		gx_text_out( dc_buffer, 58, 175, "sum(4x*8)");
		break;
	case CAMERA :
		dc_camera(dc_buffer);
		break;
	case CROP :
		gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
		gx_rectangle( dc_buffer, 50, 50, 100, 100);
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		break;
	case LABELING :
		gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		break;
	case EDIT :
		back = gx_png_open( "interface/background/edit.png");
		//display edit//
		gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		dc_buffer->font	= font14;
		dc_buffer->font_color	= gx_color( 0, 0, 0, 255);
		//sprintf( buff, "%s", msg);
		gx_text_out( dc_buffer, 9, 75 , "lim (15x^2/621x)");
		gx_text_out( dc_buffer, 9, 108, "integral(4x+2)");
		gx_text_out( dc_buffer, 9, 145, "sum(4x*8)");
		gx_text_out( dc_buffer, 9, 178, "sum(4x*8)");
		break;
	case RESULT :
		gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		break;
	}
	if ( back == NULL){
		gx_print_error(8,"background");                                       // 실행 중 에러 내용을 출력
	}else
	{
		//gx_bitblt( dc_screen, 0, 0, ( dc_t *)dc_buffer, 0, 0,320, 48);
		gx_png_close( back);
	}
}

static void interface_layout(int mode){
	png_t   *top;
	png_t   *title;
	png_t   *button;
	png_t   *button2;
	png_t   *bottom;
	png_t   *active;
	int x;
	top = gx_png_open( "interface/top.png");
	bottom = gx_png_open( "interface/menu.png");
	button2 = gx_png_open( "interface/button/home.png");
	active = gx_png_create( 63, 49);
	gx_clear( ( dc_t *)active, gx_color( 255, 255, 255, 100));
	x = 64*(mode-1);
	interface_Background(mode);
	switch(mode){
	case HOME : 
		title = gx_png_open( "interface/title/home.png");
		button = gx_png_open( "interface/button/info.png");
		button2 = gx_png_open( "interface/button/setting.png");
		break;
	case CAMERA :
		title = gx_png_open( "interface/title/camera.png");
		button = gx_png_open( "interface/button/camera.png");
		break;
	case CROP :
		title = gx_png_open( "interface/title/crop.png");
		button = gx_png_open( "interface/button/check.png");
		break;
	case LABELING :
		title = gx_png_open( "interface/title/labeling.png");
		button = gx_png_open( "interface/button/check.png");
		break;
	case EDIT :
		title = gx_png_open( "interface/title/edit.png");
		button = gx_png_open( "interface/button/check.png");
		break;
	case RESULT :
		title = gx_png_open( "interface/title/result.png");
		button = gx_png_open( "interface/button/send.png");
		break;
	}
	if ( top == NULL || title== NULL || button==NULL || bottom == NULL )
		gx_print_error(8, "layout pngs");                                       // 실행 중 에러 내용을 출력
	else
	{
		//gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 0));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)top, 0, 0, top->width, top->height);
		gx_bitblt( dc_buffer, 98, 8, ( dc_t *)title, 0, 0, title->width, title->height);
		gx_bitblt( dc_buffer, 281, 7, ( dc_t *)button, 0, 0, button->width, button->height);
		gx_bitblt( dc_buffer, 4, 6, ( dc_t *)button2, 0, 0, button->width, button->height);
		gx_bitblt( dc_buffer, 0, 191, ( dc_t *)bottom, 0, 0, bottom->width, bottom->height);
		if(x>0){
			gx_bitblt( dc_buffer, x, 191, ( dc_t *)active, 0, 0, active->width, active->height);
			gx_png_close( bottom);
			gx_png_close( active);
		}

		gx_bitblt( dc_screen, 0, 0, ( dc_t *)dc_buffer, 0, 0,320, 240);
		gx_png_close( button2);
		gx_png_close( top);
		gx_png_close( title);
		gx_png_close( button);
	}
}

//두가지 방법이 있는데 속도 테스트 해봐야함.
/*
static void interface_home( void){
    dc_t       *dc_comp;
    dc_comp = gx_get_compatible_dc(dc_screen);             // dc_screen의 클론을 만듦

    gx_open_file( dc_comp, "interface/home.png");                   // 파일을 open
    gx_bitblt( dc_screen, 0, 0, dc_comp, 0, 0, dc_screen->width, dc_screen->height);
}
 */


static void interface_splash( void){
	bmp_t   *bmp;
	bmp = gx_bmp_open( "interface/background/splash.bmp");
	if ( NULL == bmp)
		gx_print_error(8, "interface/background/splash.bmp");                                         // 실행 중 에러 내용을 출력
	else
	{
		gx_bitblt( dc_screen, 0, 0, ( dc_t *)bmp, 0, 0, bmp->width, bmp->height);
		gx_bmp_close( bmp);
	}
}
static void interface_loading(int mode){
	png_t   *png;
	switch(mode) {
	case START :
		gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
		png = gx_png_open( "interface/background/loading.png");
		if ( NULL == png)
			gx_print_error(8, "interface/background/loading.png");                                         // 실행 중 에러 내용을 출력
		else
		{
			gx_bitblt( dc_screen, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
			gx_png_close( png);
		}
	break;
	case END :  
		gx_bitblt( dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
	break;
	}
}

static void interface_setting(){
		png_t   *png;
		int touch_font14,touch_font18,touch_network,touch_volume,touch_ok,touch_close;
		int t_event;
		dc_t *setting_frame;
		setting_frame = gx_get_compatible_dc( dc_screen);
		gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
		gx_bitblt( setting_frame, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
		png = gx_png_open( "interface/setting.png");
		pauseTouchevent();
		touch_close = addTouchevent(268, 30, 30, 30);
		touch_font14 = addTouchevent(136, 69, 170, 30);
		touch_font18 = addTouchevent(136,103, 170, 30);
		touch_network = addTouchevent(136, 140, 170, 30);
		touch_volume = addTouchevent(136, 178, 170, 30);
		touch_ok = addTouchevent(231, 30, 30, 30);
		
		if ( NULL == png)
			gx_print_error(8, "interface/setting.png");                                         // 실행 중 에러 내용을 출력
		else
		{
		gx_bitblt( setting_frame, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
			while(1){
				gx_bitblt( dc_buffer, 0, 0, setting_frame, 0, 0, 320, 240);
				dc_buffer->font	= font14;
				dc_buffer->font_color	= gx_color( 90, 90, 90, 255);
				gx_text_out( dc_buffer, 149, 89, "font14");
				gx_text_out( dc_buffer, 149, 124, "font18");
				gx_text_out( dc_buffer, 149, 158, "WIFI");
				gx_text_out( dc_buffer, 149, 198, "20");
				gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
			
				t_event = touch(dc_screen);
				if(		   t_event == touch_font14) {
								
				} else if (t_event == touch_font18) {
					
				} else if (t_event == touch_network) {
					
				} else if (t_event == touch_volume) {
					
				} else if (t_event == touch_ok) {
					resumeTouchevent();
					gx_png_close( png);
					gx_bitblt( dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
					return;
				} else if (t_event == touch_close) {
					resumeTouchevent();
					gx_png_close( png);
					gx_bitblt( dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
					return;
				}
			}
		}
		
}

static void interface_alert( char* msg){
	png_t   *png;
	int touch_all;
	pauseTouchevent();
	touch_all = addTouchevent(0, 0, 320, 240);
	gx_bitblt( dc_buffer, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	//gx_to_screen_dc(dc_buffer,dc_screen);
	//gx_to_screen_dc(before_screen,dc_screen);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	png = gx_png_open( "interface/alert.png");
	if ( NULL == png)
		gx_print_error(8, "interface/alert.png");                                         // 실행 중 에러 내용을 출력
	else
	{
		//100,120
		gx_bitblt( dc_buffer, 0, 0, (dc_t*)png, 0, 0, 320, 240);
		sprintf( buff, "%s", msg);
		dc_buffer->font	= font14;
		dc_buffer->font_color	= gx_color( 0,    0, 0, 255);
		gx_text_out( dc_buffer, 100, 120, buff);
		gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
		gx_png_close( png);
	}
	touch(dc_screen);
	resumeTouchevent();
	//gx_to_screen_dc(dc_screen,before_screen);
	gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}
static void interface_info(void){
	png_t   *png;
	gx_bitblt( dc_buffer, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	addTouchevent(0, 0, 320, 240);
	png = gx_png_open( "interface/info.png");
	if ( NULL == png)
		gx_print_error(8, "interface/info.png");                                         // 실행 중 에러 내용을 출력
	else
	{
		gx_bitblt( dc_buffer, 0, 0, (dc_t*)png, 0, 0, 320, 240);
		dc_buffer->font	= font18;
		dc_buffer->font_color	= gx_color( 0, 0, 0, 255);
		gx_text_out( dc_buffer, 263, 68, "1.0");
		dc_buffer->font	= font14;
		dc_buffer->font_color	= gx_color( 90, 90, 90, 255);
		gx_text_out( dc_buffer, 175, 121, "JYG");
		gx_text_out( dc_buffer, 175, 147, "KHJ");
		gx_text_out( dc_buffer, 175, 173, "PSY");
		gx_text_out( dc_buffer, 175, 199, "JYJ");
		gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
		gx_png_close( png);
	}
	touch(dc_screen);
	resumeTouchevent();
	gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}


/** ----------------------------------------------------------------------------
@brief  main()
@remark -
@param  -
@return -
 -----------------------------------------------------------------------------*/
int   main  ( int argc, char *argv[]){
	int h_touch_home, h_touch_camera, h_touch_crop, h_touch_labeling, h_touch_edit, h_touch_result,h_touch_ok;
	int e_touch;
	int now_mode=0;
	if( 2 > argc){                                                      //  인수가 없다면 Frame Buffer의 기본 Device Name은 /dev/fb
		strcpy( fb_dev_name, "/dev/fb");
	} else {
		strcpy( fb_dev_name, argv[1]);
	}

	if  ( GX_SUCCESS != gx_open( fb_dev_name)           )   return 1;   // gxLib 초기화
	if  ( NULL == ( dc_screen = gx_get_screen_dc() ) 	)   return 1;   // 화면 출력을 위한 screen DC 구함
	if  ( NULL == ( dc_buffer
			= gx_get_compatible_dc( dc_screen) ))   return 1;   // 화면 깜빡임을 없애기 위한 버퍼 DC
	if  ( NULL == ( before_screen
			= gx_get_compatible_dc( dc_screen) ))   return 1; 

	gx_clear( dc_screen, gx_color( 0, 0, 0, 255));
	gx_clear( dc_buffer, gx_color( 0, 0, 0, 255));

	if  ( NULL == fontloader14("malgun14.bdf") ) return 1;
	if  ( NULL == fontloader18("malgun18.bdf") ) return 1;

	printf( "running....\n");
	printf( "screen widht= %d\n"      , dc_screen->width);              // 화면 폭과 넓이를 출력
	printf( "screen color depth= %d\n", dc_screen->colors);
	interface_splash();
	initTouchevent("/dev/input/event0",dc_screen);
	h_touch_home = addTouchevent(0, 0, 44, 44);
	h_touch_camera = addTouchevent(0, 191, 63, 49);
	h_touch_crop = addTouchevent(64, 191, 63, 49);
	h_touch_labeling = addTouchevent(127, 191, 63, 49);
	h_touch_edit = addTouchevent(192, 191, 63, 49);
	h_touch_result = addTouchevent(254, 191, 63, 49);
	h_touch_ok = addTouchevent(276, 0, 44, 44);
	//sleep(1);
	interface_layout(HOME);
	now_mode = 0;
	//usleep(1000);
	while(1){
		e_touch = touch(dc_screen);
			if(e_touch == h_touch_home ) {
				if(now_mode == 0){
					interface_setting();				
				} else {
				now_mode = 0;
				interface_layout(HOME);
				}
			} else if(e_touch == h_touch_camera ){
				now_mode = 1;
				interface_layout(CAMERA);
			} else if(e_touch == h_touch_crop) {
				now_mode = 2;
				interface_alert("Please take photo");
				//sleep(1);
				interface_layout(CROP);
				dc_screen->pen_color     = gx_color( 255, 0, 0, 255);
				dc_screen->brush_color   = gx_color( 0, 0, 0, 0);
				gx_rectangle( dc_screen, 100, 100, 200, 100);
			} else if(e_touch ==  h_touch_labeling ){
				now_mode = 3;
				interface_loading(START);
				sleep(1);
				interface_loading(END); 
				interface_layout(LABELING);
			} else if(e_touch ==  h_touch_edit ){
				now_mode = 4;
				interface_layout(EDIT);
			} else if(e_touch ==  h_touch_result ){
				now_mode = 5;
				interface_layout(RESULT);
			} else if(e_touch ==  h_touch_ok ){ 		
				now_mode = 6;
				interface_info();
				//sleep(1);
		}
		usleep(1);
	}
	gx_release_dc( dc_buffer);
	gx_release_dc( dc_screen);
	gx_close();
	return   0;
}
