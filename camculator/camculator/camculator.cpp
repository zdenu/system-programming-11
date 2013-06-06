#include "define.h"
#include "camculator.h"

#include "touch.h"
#include "camera.h"

#include "../include/gxlib/gxjpg.h"
#include "../include/gxlib/gxpng.h"


/** ----------------------------------------------------------------------------
@brief  ���� ũ�� ����
@remark
 -----------------------------------------------------------------------------*/
#define MAX_BUFF_SIZE   1024
#define MAX_DEV_NANE    100


/** ----------------------------------------------------------------------------
@brief  ���� ���� ����
@remark
 -----------------------------------------------------------------------------*/
static char  buff[MAX_BUFF_SIZE];

template<> Camculator* Singleton<Camculator>::ms_Singleton = 0;

Camculator::Camculator()
: isRunning(false)
, h_touch_home(INVALID_HANDLE)
, h_touch_camera(INVALID_HANDLE)
, h_touch_crop(INVALID_HANDLE)
, h_touch_labeling(INVALID_HANDLE)
, h_touch_edit(INVALID_HANDLE)
, h_touch_result(INVALID_HANDLE)
, h_touch_ok(INVALID_HANDLE)
, dc_screen(NULL)
, dc_buffer(NULL)
, before_screen(NULL)
, malgun14(NULL)
, malgun18(NULL)
{
}

Camculator::~Camculator()
{
}

bool Camculator::init(void)
{
	if  ( GX_SUCCESS != gx_open(const_cast<char*>(FRAME_BUFFER_DEVICE)) )
		return false;
	if  ( NULL == (dc_screen = gx_get_screen_dc()) )
		return false;
	if  ( NULL == (dc_buffer = gx_get_compatible_dc(dc_screen)) )
		return false;
	if  ( NULL == (before_screen = gx_get_compatible_dc(dc_screen)) )
		return false;
	
	gx_clear( dc_screen, gx_color( 0, 0, 0, 255));
	gx_clear( dc_buffer, gx_color( 0, 0, 0, 255));
	
	printf( "font loading\n");
	if ( NULL == ( malgun14 = gx_open_font(FONT_MALGUN_14)) )
		return false;
	if ( NULL == ( malgun18 = gx_open_font(FONT_MALGUN_18)) )
		return false;
	
	printf( "running....\n");
	printf( "screen widht= %d\n"      , dc_screen->width);
	printf( "screen color depth= %d\n", dc_screen->colors);
	interface_splash();
	
	pTouchHandler = new TouchHandler;
	
	isRunning = true;
	
	// TODO: make each device thread.
}

void Camculator::main(void)
{
	while(isRunning){
		int e_touch = pTouchHandler->touch();
		if(e_touch == h_touch_home ) {
			interface_layout(HOME);
		} else if(e_touch == h_touch_camera ){
			interface_layout(CAMERA);
		} else if(e_touch == h_touch_crop ){
			interface_alert(const_cast<char*>("Please take photo"));
			sleep(1);
		} else if(e_touch == h_touch_crop) {
			interface_layout(CROP);
			dc_screen->pen_color     = gx_color( 255, 0, 0, 255);
			dc_screen->brush_color   = gx_color( 0, 0, 0, 0);
			gx_rectangle( dc_screen, 100, 100, 200, 200);
		} else if(e_touch ==  h_touch_labeling ){
			interface_layout(LABELING);
			interface_loading(START);
			sleep(1);
			interface_loading(END);
		} else if(e_touch ==  h_touch_edit ){
			interface_layout(EDIT);
		} else if(e_touch ==  h_touch_result ){
			interface_layout(RESULT);
		} else if(e_touch ==  h_touch_ok ){
			interface_info();
			sleep(1);
		}
		usleep(1);
	}
}



void Camculator::interface_Background(int mode)
{
	png_t   *back;
	back = gx_png_create( 320, 240);
	switch(mode){
	case HOME : 
		back = gx_png_open( "interface/background/home.png");
		//display history//
		gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
		dc_buffer->font	= malgun14;
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
		dc_buffer->font	= malgun14;
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
		gx_print_error(8,"background");                                       // ���� �� ���� ������ ���
	}else
	{
		//gx_bitblt( dc_screen, 0, 0, ( dc_t *)dc_buffer, 0, 0,320, 48);
		gx_png_close( back);
	}
}

void Camculator::interface_layout(int mode)
{
	png_t   *top;
	png_t   *title;
	png_t   *button;
	png_t   *bottom;
	png_t   *active;
	int x;
	top = gx_png_open( "interface/top.png");
	bottom = gx_png_open( "interface/menu.png");
	active = gx_png_create( 63, 49);
	gx_clear( ( dc_t *)active, gx_color( 255, 255, 255, 100));
	x = 64*(mode-1);
	interface_Background(mode);
	switch(mode){
	case HOME : 
		title = gx_png_open( "interface/title/home.png");
		button = gx_png_open( "interface/button/info.png");
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
		gx_print_error(8, "layout pngs");                                       // ���� �� ���� ������ ���
	else
	{
		//gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 0));
		gx_bitblt( dc_buffer, 0, 0, ( dc_t *)top, 0, 0, top->width, top->height);
		gx_bitblt( dc_buffer, 98, 8, ( dc_t *)title, 0, 0, title->width, title->height);
		gx_bitblt( dc_buffer, 281, 7, ( dc_t *)button, 0, 0, button->width, button->height);
		gx_bitblt( dc_buffer, 0, 191, ( dc_t *)bottom, 0, 0, bottom->width, bottom->height);
		if(x>0){
			gx_bitblt( dc_buffer, x, 191, ( dc_t *)active, 0, 0, active->width, active->height);
			gx_png_close( bottom);
			gx_png_close( active);
		}

		gx_bitblt( dc_screen, 0, 0, ( dc_t *)dc_buffer, 0, 0,320, 240);
		gx_png_close( top);
		gx_png_close( title);
		gx_png_close( button);
	}
}

//�ΰ��� ����� �ִµ� �ӵ� �׽�Ʈ �غ�����.
/*
static void interface_home( void){
    dc_t       *dc_comp;
    dc_comp = gx_get_compatible_dc(dc_screen);             // dc_screen�� Ŭ���� ����

    gx_open_file( dc_comp, "interface/home.png");                   // ������ open
    gx_bitblt( dc_screen, 0, 0, dc_comp, 0, 0, dc_screen->width, dc_screen->height);
}
 */


void Camculator::interface_splash(void)
{
	bmp_t   *bmp;
	bmp = gx_bmp_open( "interface/background/splash.bmp");
	if ( NULL == bmp)
		gx_print_error(8, "interface/background/splash.bmp");                                         // ���� �� ���� ������ ���
	else
	{
		gx_bitblt( dc_screen, 0, 0, ( dc_t *)bmp, 0, 0, bmp->width, bmp->height);
		gx_bmp_close( bmp);
	}
}
void Camculator::interface_loading(int mode)
{
	png_t   *png;
	switch(mode) {
	case START :
		gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
		png = gx_png_open( "interface/background/loading.png");
		if ( NULL == png)
			gx_print_error(8, "interface/background/loading.png");                                         // ���� �� ���� ������ ���
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

void Camculator::interface_alert(char* msg)
{
	png_t   *png;
	gx_bitblt( dc_buffer, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	png = gx_png_open( "interface/alert.png");
	if ( NULL == png)
		gx_print_error(8, "interface/alert.png");                                         // ���� �� ���� ������ ���
	else
	{
		//100,120
		gx_bitblt( dc_buffer, 0, 0, (dc_t*)png, 0, 0, 320, 240);
		sprintf( buff, "%s", msg);
		dc_buffer->font	= malgun14;
		dc_buffer->font_color	= gx_color( 0,    0, 0, 255);
		gx_text_out( dc_buffer, 100, 120, buff);
		gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
		gx_png_close( png);
	}
	touch();
	gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}

void Camculator::interface_info(void)
{
	png_t   *png;
	gx_bitblt( dc_buffer, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	png = gx_png_open( "interface/info.png");
	if ( NULL == png)
		gx_print_error(8, "interface/info.png");                                         // ���� �� ���� ������ ���
	else
	{
		gx_bitblt( dc_buffer, 0, 0, (dc_t*)png, 0, 0, 320, 240);
		dc_buffer->font	= malgun18;
		dc_buffer->font_color	= gx_color( 0, 0, 0, 255);
		gx_text_out( dc_buffer, 263, 68, "1.0");
		dc_buffer->font	= malgun14;
		dc_buffer->font_color	= gx_color( 90, 90, 90, 255);
		gx_text_out( dc_buffer, 175, 121, "JYG");
		gx_text_out( dc_buffer, 175, 147, "KHJ");
		gx_text_out( dc_buffer, 175, 173, "PSY");
		gx_text_out( dc_buffer, 175, 199, "JYJ");
		gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
		gx_png_close( png);
	}
	touch();
	gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}


void Camculator::setTouchEvents(void)
{
	h_touch_home		= pTouchHandler->addTouchevent(0, 0, 44, 44);
	h_touch_camera		= pTouchHandler->addTouchevent(0, 191, 63, 49);
	h_touch_crop		= pTouchHandler->addTouchevent(64, 191, 63, 49);
	h_touch_labeling	= pTouchHandler->addTouchevent(127, 191, 63, 49);
	h_touch_edit		= pTouchHandler->addTouchevent(192, 191, 63, 49);
	h_touch_result		= pTouchHandler->addTouchevent(254, 191, 63, 49);
	h_touch_ok			= pTouchHandler->addTouchevent(276, 0, 44, 44);

}
