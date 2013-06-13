#include "define.h"
#include "camculator.h"

#include "touch.h"
#include "keypad.h"
#include "ioutil.h"


#include "gxjpg.h"
#include "gxpng.h"

// each states.
#include "state.h"
#include "home.h"
#include "camera.h"
#include "crop.h"
#include "edit.h"
#include "labeling.h"
#include "result.h"

#include "WolframAlphaManager.h"
#include "opencv.h"
#include "mp3.h"

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
, pOpenCV(NULL)
, pIOutil(NULL)
{
	memset(pState, 0x00, sizeof(pState));
}

Camculator::~Camculator()
{
}

bool Camculator::init(void)
{
	WolframAlphaManager::createSingleton();
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
	
	pKeyHandler = new KeyHandler;
	pKeyHandler->init();

	pIOutil = new IOutil;
	pIOutil->init();
	
	pOpenCV = new OpenCV;
	pOpenCV->init();
	
	printf( "running....\n");
	pIOutil->Buzzer(100);
	pIOutil->LED_ON(0xAA);
	pIOutil->textlcd("welcome camculator");
	pIOutil->fnd_init("      ");
	pIOutil->fnd("camcu1");
	pIOutil->fnd_kill();

	printf( "screen [%d,%d]\n"      , dc_screen->width, dc_screen->height);
	printf( "screen color depth= %d\n", dc_screen->colors);

	printf("Resize factor : %d\n",pIOutil->SW_read()*100);
	
	// TODO: create each states.
	pState[SCREEN_TYPE_HOME]	= new Home;
	pState[SCREEN_TYPE_CAMERA]	= new Camera;
	pState[SCREEN_TYPE_LABELING] = new Labeling;
	pState[SCREEN_TYPE_CROP]	= new Crop;
	pState[SCREEN_TYPE_EDIT]	= new Edit;
	pState[SCREEN_TYPE_RESULT]	= new Result;
	
	printf("create states complete.\n");
	pIOutil->Buzzer(100);
	
	
	for (int i = 0 ; i < SCREEN_TYPE_MAX ; ++i)
	{
		if (pState[i] != NULL)
		{
			pState[i]->init(dc_buffer, font14, (ENUM_SCREEN_TYPE)i);
			printf("%d state init complete.\n", i);
		}
	}
	
	interface_splash();
	
	isRunning = true;
	// draw home screen.
	
	pState[SCREEN_TYPE_HOME]->init(dc_buffer, font14, SCREEN_TYPE_HOME);
	MP3_play("/mnt/usb/sound/ko/welcome.mp3");
	currentState = SCREEN_TYPE_HOME;
	pCurrentState = pState[SCREEN_TYPE_HOME];
	pCurrentState->enableTouchEvents();
	pCurrentState->makeScreen(dc_buffer, dc_screen, NULL);
	
	
}

std::string Camculator::replaceAll(const std::string &str, const std::string &pattern, const std::string &replace)
{
	string result = str;
	string::size_type pos = 0;
	string::size_type offset = 0;

	while((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result.c_str();
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
				{
					// go to screen type
					break;
				}
				case EVENT_TYPE_KEY_PAD:
				{
					keypadDispatcher(pEv);
				}
				default:
				{
					break;
				}
			}
			
			delete pEv->pData;
			delete pEv;
		}
		else
		{
			pCurrentState->makeScreen(dc_buffer, dc_screen, NULL);
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
	stTouchData* pTouchEv = (stTouchData*)(pEv->pData);
	void* pParam = NULL;

	pCurrentState->dispatchTouchEvent(dc_screen, pTouchEv, &pParam);
	
	printf("pParam : %x\n", pParam);
	// get screen type by event.
	int nextState = getNextScreenType(pTouchEv->touchType);
	
	if (nextState != SCREEN_TYPE_MAX)
	{
		currentState = nextState;
		
		if (pState[currentState] != NULL)
		{
			pCurrentState->disableTouchEvents();
			pCurrentState = pState[currentState];
			pCurrentState->enableTouchEvents();
		}
		
	}
	printf("State : %d\n", currentState);
	
	pCurrentState->makeScreen(dc_buffer, dc_screen, pParam);
	
	if (pParam != NULL)
		delete pParam;
	
}


void Camculator::httpResponseDispatcher(stEvent* pEv)
{
	if (pCurrentState->getScreenType() == SCREEN_TYPE_RESULT)
	{
		Result* pResult = (Result*)pCurrentState;
		TImageVector* pImgVector = (TImageVector*)(pEv->pData);
		pResult->parseGifImages(pImgVector);
		pResult->makeScreen(dc_buffer, dc_screen, NULL);
		
	}
}

void Camculator::keypadDispatcher(stEvent* pEv)
{
	if (pCurrentState->getScreenType() == SCREEN_TYPE_EDIT)
	{
		Edit* pEdit = (Edit*)pCurrentState;
		stKeyData* pKeyData = (stKeyData*)(pEv->pData);
		
		pEdit->dispatchKeyEvent(dc_buffer, pKeyData);
		
		pEdit->makeScreen(dc_buffer, dc_screen, NULL);
	}
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
//		interface_Background(mode);
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

//
//void Camculator::interface_splash(void)
//{
//	bmp_t   *bmp;
//	bmp = (bmp_t*)gx_bmp_open( "interface/background/splash.bmp");
//	if ( NULL == bmp)
//		gx_print_error(8, "interface/background/splash.bmp");                                         // 실행 중 에러 내용을 출력
//	else
//	{
//		gx_bitblt( dc_screen, 0, 0, ( dc_t *)bmp, 0, 0, bmp->width, bmp->height);
//		gx_bmp_close((dc_t*)bmp);
//	}
//}
void Camculator::interface_loading(int mode)
{
	png_t   *png;
	if(mode != START)
		gx_bitblt( dc_buffer, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
	switch(mode) {
		case START :
			pIOutil->LED_ON(0x00);
			gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
			png = (png_t*)gx_png_open( "interface/background/loading0001.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0001.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP0 : 
			pIOutil->LED_ON(0x00);
			png = (png_t*)gx_png_open( "interface/background/loading0001.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0001.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP1 : 
			pIOutil->LED_ON(0x01);
			png = (png_t*)gx_png_open( "interface/background/loading0002.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0002.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP2 : 
			pIOutil->LED_ON(0x02);
			png = (png_t*)gx_png_open( "interface/background/loading0003.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0003.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP3 : 
			pIOutil->LED_ON(0x04);
			png = (png_t*)gx_png_open( "interface/background/loading0004.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0004.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP4 : 
			pIOutil->LED_ON(0x08);
			png = (png_t*)gx_png_open( "interface/background/loading0005.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0005.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP5 : 
			pIOutil->LED_ON(0x0F);
			png = (png_t*)gx_png_open( "interface/background/loading0006.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0006.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP6 : 
			pIOutil->LED_ON(0x20);
			png = (png_t*)gx_png_open( "interface/background/loading0007.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0007.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case STEP7 : 
			pIOutil->LED_ON(0x40);
			png = (png_t*)gx_png_open( "interface/background/loading0008.png");
			if ( NULL == png)
				gx_print_error(8, "interface/background/loading0008.png");                                         // 실행 중 에러 내용을 출력
			else
			{
				gx_bitblt( dc_buffer, 0, 0, ( dc_t *)png, 0, 0, png->width, png->height);
				gx_png_close((dc_t*)png);
			}
			break;
		case END :
			pIOutil->LED_ON(0x80);
			gx_bitblt( dc_buffer, 0, 0, before_screen, 0, 0, 320, 240);
			break;
	}
	gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
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
	pTouchHandler->addTouchevent(0, 49, 320, 142, TOUCH_EVENT_CROP_CLICK);
	pTouchHandler->addTouchevent(0, 49, 320, 71, TOUCH_EVENT_RESULT_PREV);
	pTouchHandler->addTouchevent(0, 120, 320, 71, TOUCH_EVENT_RESULT_NEXT);
	
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

int Camculator::getNextScreenType(int mode)
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
				if (currentState == SCREEN_TYPE_HOME)
					ret = SCREEN_TYPE_SETTING;
				else
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
			case TOUCH_EVENT_MAIN_OK:
			{
				if (currentState == SCREEN_TYPE_HOME)
					ret = SCREEN_TYPE_INFO;
				else if (currentState == SCREEN_TYPE_CAMERA)
					ret = SCREEN_TYPE_CROP;
				else if (currentState == SCREEN_TYPE_CROP)
					ret = SCREEN_TYPE_LABELING;
				else if (currentState == SCREEN_TYPE_LABELING)
					ret = SCREEN_TYPE_EDIT;
				else if (currentState == SCREEN_TYPE_EDIT)
					ret = SCREEN_TYPE_RESULT;
				break;
			}
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


void Camculator::interface_movie( char* file, int max, int fps)
{
	png_t   *png;
	char namebuff[20];
	int i;
	fps = 1000000/fps;
//	pauseTouchevent();
	//gx_bitblt( before_screen, 0, 0, (dc_t*)dc_screen, 0, 0, 320, 240);
	for(i=0;i<max;i++) {
		sprintf(namebuff,"%s%04d.png",file,(i+1));
		png = (png_t*)gx_png_open(namebuff);
		if ( NULL == png)
			gx_print_error(8, namebuff);
		else
		{
			//	gx_bitblt( dc_buffer, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
			//	gx_bitblt( dc_buffer, 0, 0, (dc_t*)png, 0, 0, 320, 240);
			//	gx_bitblt( dc_screen, 0, 0, dc_buffer, 0, 0, 320, 240);
			gx_bitblt( dc_screen, 0, 0, (dc_t*)png, 0, 0, 320, 240);
			gx_png_close((dc_t*)png);
			usleep(fps);
		}
	}
//	resumeTouchevent();
	//gx_bitblt( dc_screen, 0, 0, (dc_t*)before_screen, 0, 0, 320, 240);
}

void Camculator::interface_splash(void)
{
	MP3_play("/mnt/usb/sound/boot.mp3");
	interface_movie("interface/intro/intro", 99 , 24);
	sleep(2);
}

