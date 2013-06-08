#include "define.h"
#include "camculator.h"

#include "touch.h"


#include "gxjpg.h"
#include "gxpng.h"

// each states.
#include "state.h"
#include "home.h"
#include "camera.h"
#include "crop.h"
#include "edit.h"
#include "result.h"

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
static char  buff[MAX_BUFF_SIZE];

template<> Camculator* Singleton<Camculator>::ms_Singleton = 0;

Camculator::Camculator()
: isRunning(false)
, dc_screen(NULL)
, dc_buffer(NULL)
, before_screen(NULL)
, setting_frame(NULL)
, font14(NULL)
, font18(NULL)
, currentState(TOUCH_EVENT_MAIN_HOME)
, isSettingInitialized(false)
, pTouchHandler(NULL)
{
	memset(pState, 0x00, sizeof(pState));
}

Camculator::~Camculator()
{
}

bool Camculator::init(void)
{
	if  ( GX_SUCCESS != gx_open(const_cast<char*>(FRAME_BUFFER_DEVICE)) )
	{
		printf("Frame buffer device load open error.\n");
		return false;
	}
	dc_screen = gx_get_screen_dc();	// get current screen.
	dc_buffer = gx_get_compatible_dc(dc_screen);
	before_screen = gx_get_compatible_dc(dc_screen);
	
	gx_clear(dc_screen, gx_color( 0, 0, 0, 255));
	gx_clear(dc_buffer, gx_color( 0, 0, 0, 255));
	
	printf( "font loading\n");
	if ( NULL == ( font14 = gx_open_font(FONT_MALGUN_14)) )
		return false;
	if ( NULL == ( font18 = gx_open_font(FONT_MALGUN_18)) )
		return false;
	
	pTouchHandler = new TouchHandler;
	pTouchHandler->init(dc_screen);
	initTouchEvents();
		
	printf( "running....\n");
	printf( "screen widht= %d\n"      , dc_screen->width);
	printf( "screen color depth= %d\n", dc_screen->colors);
	
	// TODO: create each states.
	pState[SCREEN_TYPE_HOME]	= new Home;
	pState[SCREEN_TYPE_CAMERA]	= new Camera;
	pState[SCREEN_TYPE_CROP]	= new Crop;
	pState[SCREEN_TYPE_EDIT]	= new Edit;
	pState[SCREEN_TYPE_RESULT]	= new Result;
	
	for (int i = 0 ; i < SCREEN_TYPE_MAX ; ++i)
	{
		if (pState[i] != NULL)
			pState[i]->init(dc_buffer, font14);
	}

	interface_splash();
	
	isRunning = true;
	
	// draw home screen.
	currentState = SCREEN_TYPE_HOME;
	pCurrentState = pState[SCREEN_TYPE_HOME];
	pCurrentState->makeScreen(dc_buffer, dc_screen);
	
}

void Camculator::main(void)
{
	while(isRunning)
	{
		// get event.
		stEvent* pEv = popEvent();
		
		if (pEv != NULL)
		{
			printf("Pop Event : %d\n", pEv->eventType);
			switch(pEv->eventType)
			{
				case EVENT_TYPE_TOUCH_PAD:
				{
					interfaceDispatcher(pEv);
					break;
				}
				case EVENT_TYPE_HTTP_RESPONSE:
					break;
				default:
				{
					break;
				}
			}
			
			delete pEv;
		}
		else
		{
			pCurrentState->makeScreen(dc_buffer, dc_screen);
		}
		
		pCurrentState->drawScreen(dc_buffer, dc_screen);
		usleep(1);
	}
	
	gx_release_dc(dc_buffer);
	gx_release_dc(dc_screen);
	gx_close();
}


void Camculator::interfaceDispatcher(stEvent* pEv)
{
	// recv ENUM_TOUCH_EVENT.
	int event = *(int*)(pEv->pData);
	
	// dispatching about touch event.
	pCurrentState->dispatchTouchEvent((ENUM_TOUCH_EVENT)event);
	
	// get screen type by event.
	int state = getCurrentScreenType(event);
	
	if (state != SCREEN_TYPE_MAX)
	{
		currentState = state;
		
		if (pState[state] != NULL)
			pCurrentState = pState[state];
		
	}
	printf("State : %d\n", state);
	
	
	
	pCurrentState->makeScreen(dc_buffer, dc_screen);
	
}


void Camculator::interface_Background(int mode)
{
//	png_t*	back;
//	back = (png_t*)gx_png_create(320, 240);
//	switch(mode) {
//		case TOUCH_EVENT_MAIN_CROP :
//			gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
//			gx_rectangle( dc_buffer, 50, 50, 100, 100);
//			gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
//			break;
//		case TOUCH_EVENT_MAIN_LABELING :
//			gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
//			gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
//			break;
//		case TOUCH_EVENT_MAIN_EDIT :
//			back = (png_t*)gx_png_open((char*)"interface/background/edit.png");
//			//display edit//
//			gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
//			gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
//			dc_buffer->font	= font14;
//			dc_buffer->font_color	= gx_color( 0, 0, 0, 255);
//			//sprintf( buff, "%s", msg);
//			gx_text_out( dc_buffer, 9, 75 , "lim (15x^2/621x)");
//			gx_text_out( dc_buffer, 9, 108, "integral(4x+2)");
//			gx_text_out( dc_buffer, 9, 145, "sum(4x*8)");
//			gx_text_out( dc_buffer, 9, 178, "sum(4x*8)");
//			break;
//		case TOUCH_EVENT_MAIN_RESULT :
//			gx_clear( ( dc_t *)back, gx_color( 255, 255, 255, 255));
//			gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);
//			break;
//	}
//	if ( back == NULL){
//		gx_print_error(8,"background");                                       // 실행 중 에러 내용을 출력
//	}else
//	{
//		//gx_bitblt( dc_screen, 0, 0, ( dc_t *)dc_buffer, 0, 0,320, 48);
//		gx_png_close((dc_t*)back);
//	}
}
void Camculator::initSettingLayout(void)
{
	if (isSettingInitialized == true)
		return;
	
	png_t* png;
	int t_event;
	setting_frame = gx_get_compatible_dc(dc_screen);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	gx_bitblt( setting_frame, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	png = (png_t*)gx_png_open( "interface/setting.png");

	gx_bitblt( setting_frame, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
	
	isSettingInitialized = true;
}

void Camculator::interface_layout(int mode, int state)
{
	
	if (currentState == SCREEN_TYPE_SETTING)
	{
		initSettingLayout();
		// draw settings.
		gx_bitblt( dc_buffer, 0, 0, setting_frame, 0, 0, 320, 240);
		dc_buffer->font	= font14;
		dc_buffer->font_color	= gx_color( 90, 90, 90, 255);
		gx_text_out( dc_buffer, 149, 89, "font14");
		gx_text_out( dc_buffer, 149, 124, "font18");
		gx_text_out( dc_buffer, 149, 158, "WIFI");
		gx_text_out( dc_buffer, 149, 198, "20");
		gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
		
		if(mode == TOUCH_EVENT_SETTING_FONT14)
		{
			
		}
		else if (mode == TOUCH_EVENT_SETTING_FONT18)
		{
			
		}
		else if (mode == TOUCH_EVENT_SETTING_NETWORK)
		{
			
		}
		else if (mode == TOUCH_EVENT_SETTING_VOLUME)
		{
			
		}
		else if (mode == TOUCH_EVENT_SETTING_OK)
		{
			// disable setting events.
			disableSettingEvent();
//			gx_png_close((dc_t*)png);
			gx_bitblt( dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
		}
		else if (mode == TOUCH_EVENT_SETTING_CLOSE)
		{
			// disable setting events.
			disableSettingEvent();
//			gx_png_close((dc_t*)png);
			gx_bitblt( dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
		}
	}
	else if (currentState == TOUCH_EVENT_MAIN_OK)
	{
		interface_info();
	}
	else
	{
		png_t   *top;
		png_t   *title;
		png_t   *button;
		png_t   *button2;
		png_t   *bottom;
		png_t   *active;
		int x;
		top = (png_t*)gx_png_open( "interface/top.png");
		bottom = (png_t*)gx_png_open( "interface/menu.png");
		button2 = (png_t*)gx_png_open( "interface/button/home.png");
		active = (png_t*)gx_png_create( 63, 49);
		gx_clear( ( dc_t *)active, gx_color( 255, 255, 255, 100));
		x = 64*(mode-1);
		interface_Background(mode);
		switch(mode){
			case TOUCH_EVENT_MAIN_HOME:
				title = (png_t*)gx_png_open( "interface/title/home.png");
				button = (png_t*)gx_png_open( "interface/button/info.png");
				button2 = (png_t*)gx_png_open( "interface/button/setting.png");
				break;
			case TOUCH_EVENT_MAIN_CAMERA:
				title = (png_t*)gx_png_open( "interface/title/camera.png");
				button = (png_t*)gx_png_open( "interface/button/camera.png");
				break;
			case TOUCH_EVENT_MAIN_CROP:
				title = (png_t*)gx_png_open( "interface/title/crop.png");
				button = (png_t*)gx_png_open( "interface/button/check.png");
				break;
			case TOUCH_EVENT_MAIN_LABELING:
				title = (png_t*)gx_png_open( "interface/title/labeling.png");
				button = (png_t*)gx_png_open( "interface/button/check.png");
				break;
			case TOUCH_EVENT_MAIN_EDIT:
				title = (png_t*)gx_png_open( "interface/title/edit.png");
				button = (png_t*)gx_png_open( "interface/button/check.png");
				break;
			case TOUCH_EVENT_MAIN_RESULT:
				title = (png_t*)gx_png_open( "interface/title/result.png");
				button = (png_t*)gx_png_open( "interface/button/send.png");
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
			if(x > 0){
				gx_bitblt( dc_buffer, x, 191, ( dc_t *)active, 0, 0, active->width, active->height);
				gx_png_close((dc_t*)bottom);
				gx_png_close((dc_t*)active);
			}
			
			gx_bitblt( dc_screen, 0, 0, ( dc_t *)dc_buffer, 0, 0,320, 240);
			gx_png_close((dc_t*)button2);
			gx_png_close((dc_t*)top);
			gx_png_close((dc_t*)title);
			gx_png_close((dc_t*)button);
		}
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


void Camculator::interface_splash(void)
{
	bmp_t   *bmp;
	bmp = (bmp_t*)gx_bmp_open( "interface/background/splash.bmp");
	if ( NULL == bmp)
		gx_print_error(8, "interface/background/splash.bmp");                                         // 실행 중 에러 내용을 출력
	else
	{
		gx_bitblt( dc_screen, 0, 0, ( dc_t *)bmp, 0, 0, bmp->width, bmp->height);
		gx_bmp_close((dc_t*)bmp);
	}
}
void Camculator::interface_loading(int mode)
{
	png_t   *png;
	switch(mode) {
		case START :
			gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
			png = (png_t*)gx_png_open( "interface/background/loading.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_screen, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case END :
			gx_bitblt( dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
			break;
	}
}

//void Camculator::interface_setting(int mode)
//{
//	// disable all touch event.
//	// enable main touch event.
//	// enable setting touch event.
//	
//}


void Camculator::interface_alert(char* msg)
{
	png_t   *png;
	int touch_all;
//	pTouchHandler->pauseTouchevent();
	pTouchHandler->addTouchevent(0, 0, 320, 240, TOUCH_EVENT_ALERT_ALL);
	gx_bitblt( dc_buffer, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	//gx_to_screen_dc(dc_buffer,dc_screen);
	//gx_to_screen_dc(before_screen,dc_screen);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	png = (png_t*)gx_png_open( "interface/alert.png");
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
		gx_png_close((dc_t*)png);
	}
//	pTouchHandler->touch(dc_screen);
//	pTouchHandler->resumeTouchevent();
	//gx_to_screen_dc(dc_screen,before_screen);
	gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}

void Camculator::interface_info(void)
{
	png_t   *png;
	gx_bitblt( dc_buffer, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
//	pTouchHandler->addTouchevent(0, 0, 320, 240);
	png = (png_t*)gx_png_open( "interface/info.png");
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
		gx_png_close((dc_t*)png);
	}
//	pTouchHandler->touch(dc_screen);
//	pTouchHandler->resumeTouchevent();
	gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}


void Camculator::initTouchEvents(void)
{
	pTouchHandler->addTouchevent(0, 0, 44, 44, TOUCH_EVENT_MAIN_HOME);
	pTouchHandler->addTouchevent(0, 191, 63, 49, TOUCH_EVENT_MAIN_CAMERA);
	pTouchHandler->addTouchevent(64, 191, 63, 49, TOUCH_EVENT_MAIN_CROP);
	pTouchHandler->addTouchevent(127, 191, 63, 49, TOUCH_EVENT_MAIN_LABELING);
	pTouchHandler->addTouchevent(192, 191, 63, 49, TOUCH_EVENT_MAIN_EDIT);
	pTouchHandler->addTouchevent(254, 191, 63, 49, TOUCH_EVENT_MAIN_RESULT);
	pTouchHandler->addTouchevent(276, 0, 44, 44, TOUCH_EVENT_MAIN_OK);
	
//	pTouchHandler->addTouchevent(268, 30, 30, 30, TOUCH_EVENT_SETTING_CLOSE);
//	pTouchHandler->addTouchevent(136, 69, 170, 30, TOUCH_EVENT_SETTING_FONT14);
//	pTouchHandler->addTouchevent(136,103, 170, 30, TOUCH_EVENT_SETTING_FONT18);
//	pTouchHandler->addTouchevent(136, 140, 170, 30, TOUCH_EVENT_SETTING_NETWORK);
//	pTouchHandler->addTouchevent(136, 178, 170, 30, TOUCH_EVENT_SETTING_VOLUME);
//	pTouchHandler->addTouchevent(231, 30, 30, 30, TOUCH_EVENT_SETTING_OK);
}

int Camculator::fontloader14(char* file)
{
	char buff[30];
	sprintf(buff,"font/14pt/%s",file);
	printf( "font loading\n");
	if ( NULL == ( font14 = gx_open_font(buff) )  )
		return 1;
}

int Camculator::fontloader18(char* file)
{
	char buff[30];
	sprintf(buff,"font/18pt/%s",file);
	printf( "font loading\n");
	if ( NULL == ( font18 = gx_open_font(buff) )   )
		return 1;
}

void Camculator::pushEvent(stEvent* pEv)
{
	
	queueLock.lock();
	eventQueue.push(pEv);
	queueLock.unlock();
	
	printf("queue size: %d\n", eventQueue.size());
}
stEvent* Camculator::popEvent(void)
{
	stEvent* pEv = NULL;
//	printf("curr queue size: %d\n", eventQueue.size());

	queueLock.lock();
	if (!eventQueue.empty())
	{
		pEv = eventQueue.front();
		eventQueue.pop();
	}
	queueLock.unlock();
	
	return pEv;
}

void Camculator::drawPartScreen(int x, int y, int w, int h, color_t color)
{
	png_t* active = (png_t*)gx_png_create(w, h);
	gx_clear((dc_t *)active, color);
	gx_bitblt(dc_screen, x, y, (dc_t *)active, 0, 0, w, h);
	gx_png_close((dc_t*)active);
}

void Camculator::drawBeforeScreen(void)
{
	gx_bitblt(dc_screen, 0, 0, before_screen, 0, 0, 320, 240);
}

int Camculator::getCurrentScreenType(int mode)
{
	int ret = SCREEN_TYPE_MAX;
	
////	if ((mode == TOUCH_EVENT_MAIN_HOME) &&
////		(currentState == SCREEN_TYPE_HOME))
////	{
////		ret = SCREEN_TYPE_SETTING;
////	}
////	else if ((mode == TOUCH_EVENT_MAIN_OK) &&
////			 (currentState == SCREEN_TYPE_HOME))
////	{
////		ret = SCREEN_TYPE_INFO;
////	}
//	else
	{
		switch (mode)
		{
//			case TOUCH_EVENT_SETTING_OPEN:
//			case TOUCH_EVENT_SETTING_CLOSE:
//			case TOUCH_EVENT_SETTING_FONT14:
//			case TOUCH_EVENT_SETTING_FONT18:
//			case TOUCH_EVENT_SETTING_NETWORK:
//			case TOUCH_EVENT_SETTING_VOLUME:
//			case TOUCH_EVENT_SETTING_OK:
//				ret = SCREEN_TYPE_SETTING;
//				break;
			case TOUCH_EVENT_MAIN_HOME:
				ret = SCREEN_TYPE_HOME;
				break;
			case TOUCH_EVENT_MAIN_CAMERA:
				ret = SCREEN_TYPE_CAMERA;
				break;
			case TOUCH_EVENT_MAIN_CROP:
				ret = SCREEN_TYPE_CROP;
				break;
			case TOUCH_EVENT_MAIN_EDIT:
				ret = SCREEN_TYPE_EDIT;
				break;
			case TOUCH_EVENT_MAIN_LABELING:
				ret = SCREEN_TYPE_LABELING;
				break;
			case TOUCH_EVENT_MAIN_RESULT:
				ret = SCREEN_TYPE_RESULT;
				break;
		}
	}
	
	return ret;
}

void Camculator::disableSettingEvent(void)
{
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_OPEN);
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_CLOSE);
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_FONT14);
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_FONT18);
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_NETWORK);
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_VOLUME);
	pTouchHandler->disableTouchEvent(TOUCH_EVENT_SETTING_OK);
}

