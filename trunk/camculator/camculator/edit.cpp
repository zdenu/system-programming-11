//
//  edit.cpp
//  camculator
//
//  Created by Christopher Kim on 6/8/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//
#include "define.h"
#include "edit.h"

#include "WolframAlphaManager.h"
using namespace std;

Edit::Edit()
{
	keymode = 0;
	bkey = 0;
	cursor = 0;
	step = 0;
	txt="integral x^2 dx"; // Formula
	shift = false;
}

Edit::~Edit()
{
}

bool Edit::init(dc_t* dc_buffer, font_t* pFont, ENUM_SCREEN_TYPE state)
{
	printf("edit init start.\n");
	State::init(dc_buffer, pFont, state);
	
	if (back != NULL)
		gx_png_close((dc_t*)back);
	
	back = (png_t*)gx_png_open((char*)"interface/background/edit.png");
	title = (png_t*)gx_png_open( "interface/title/edit.png");
	button = (png_t*)gx_png_open( "interface/button/check.png");
	
	return true;
}
bool Edit::makeScreen(dc_t* dc_buffer, dc_t* dc_screen, void* pParam)
{
	this->makeBackground(dc_buffer, pParam);
	State::makeScreen(dc_buffer, dc_screen, pParam);
	//Line Max 32char
	string tmp(txt);
	tmp.insert(cursor,"|",0,1);
	//replace space
	//replace_if(tmp.begin(), tmp.end(), bind2nd(equal_to<char>(), ' '), '_');
	string l1,l2,l3,l4;
	l1 = tmp.substr(0,32);
	l2 = tmp.substr(33,32);
	l3 = tmp.substr(65,32);
	l4 = tmp.substr(97,32);
	//line1 
	gx_text_out( dc_buffer, 9, 75 , l1.c_Str());
	//line2
	gx_text_out( dc_buffer, 9, 108, l2.c_Str());
	//line3
	gx_text_out( dc_buffer, 9, 145, l3.c_Str());
	//line4
	gx_text_out( dc_buffer, 9, 178, l4.c_Str());

	return true;
}

int Edit::dispatchTouchEvent(dc_t* dc_buffer, stTouchData* pTouchEvent, void** pParam)
{
	if(pTouchEvent->touchType == TOUCH_EVENT_MAIN_OK)
	{
		char expression[1024] = {"\0"};
		expression = txt.c_str();
		// send http request.
		
	}
       
	return true;
}
int Edit::inputKey(int keymode, string n1 ,string n2,string n3, string a1, string a2, string a3 ,string s1,string s2,string s3){
	if(shift){
		transform( a1.begin(), a1.end(), a1.begin(), toupper );
		transform( a2.begin(), a2.end(), a2.begin(), toupper );
		transform( a3.begin(), a3.end(), a3.begin(), toupper );
	}
		
	switch(keymode) {
				case 0 : //num
					switch(step){
						case 0 : 
							txt.insert(cursor,n1,0,n1.length());
						break;
						case 1 : 
							txt.replace(cursor,1,n2,0,n2.length());
						break;
						case 2 :
							txt.replace(cursor,1,n3,0,n3.length());
						break;
						case 3 :
							txt.replace(cursor,1,n1,0,n1.length());
						break;
					}
				break;
				case 1 : //alphabet
					switch(step){
						case 0 : 
						txt.insert(cursor,a1,0,a1.length());
						break;
						case 1 : 
						txt.replace(cursor,1,a2,0,a1.length());
						break;
						case 2 :
						txt.replace(cursor,1,a3,0,a3.length());
						break;
						case 3 :
						txt.replace(cursor,1,a1,0,a1.length());
						break;
					}
				break;
				case 2 : //symbol
				switch(step){
						case 0 : 
						txt.insert(cursor,s1,0,s1.length());
						break;
						case 1 : 
						txt.replace(cursor,1,s2,0,s2.length());
						break;
						case 2 :
						txt.replace(cursor,1,s3,0,s3.length());
						break;
						case 3 :
						txt.replace(cursor,1,s1,0,s1.length());
						break;
					}
				break;
			}
			if((keymode == 0 && n1 == n2) || (keymode == 1 && a1 == a2) || (keymode == 2 && s1 == s2)) {
				step = 0;
			}else if(step<3) {
				step++;
			} else {
				step=1;
			}
	return 1;
}


int Edit::dispatchKeyEvent(dc_t* dc_buffer, stKeyData* pKeyEvent, void** pParam)
{
	//0 : num, 1: alphabet, 2 : symbol
	/*
	  | 1  | 2  | 3  | 4   |
	  | 5  | 6  | 7  | 8   |
	  | 9  |10 |11| 12 |
	  | 13 |14 |15| 16 |
	*/
	if(key != bkey || step == 0){
		if(cursor < txt.length())
			cursor++;
		bkey = key;
	}
	switch(key){
	 case 1:
		//7 abc +
			this->inputKey("0","0","0","a","b","c","+","+","+");
		break;
     case 2:
		//8 def -
			this->inputKey("8","8","8","d","e","f","-","-","-");
		break;
     case 3:
		//9 ghi *
			this->inputKey("9","9","9","g","h","i","*","*","*");
		break;
     case 4:
		// space
			this->inputKey(" "," "," "," "," "," "," "," "," ");
		break;
     case 5:
		//4 jkl /
		this->inputKey("4","4","4","j","k","l","/","/","/");
		break;
     case 6:
		//5 mno ^
		this->inputKey("5","5","5","m","n","o","^","^","^");
		break;
     case 7:
		//6 pqr =
		this->inputKey("6","6","6","p","q","r","=","=","=");
		break;
     case 8:
		// ( { <
		this->inputKey("(","{","<","(","{","<","(","{","<");
		break;
     case 9:
		//1 stu ,
		this->inputKey("1","1","1","s","t","u",",",",",",");
		break;
     case 10:
		//2 vwx %
		this->inputKey("2","2","2","v","w","x","%","%","%");
		break;
     case 11:
		//3 yz 
		this->inputKey("3","3","3","y","z","'","=","=","=");
		break;
     case 12:
		// ) } >
		this->inputKey(")","}",">",")","}",">",")","}",">");	
		break;
     case 13:
		//0  .
		this->inputKey("0","0","0",".",".",".",".",".",".");	
		break;
     case 14:
		// shift
			if(shift){
				shift = false;			
			} else {
				shift = true;
			}
		break;
     case 15:
		// back space
		if(cursor>0){
		txt.erase(cursor-1,1);
		cursor--;
		}
		break;
     case 16:
		//del
		if(cursor<txt.length()-1){
			txt.erase(cursor,1);
		}
		break;
		case 17: //sw1 ->
		step = 0;
		if(cursor<txt.length()){
			cursor++;
		}
		break;
		case 18://sw2 <-
		step = 0;
		if(cursor>0){
			cursor--;
		}
		break;
		case 19://sw3
		//mode
		step = 0;
		if(keymode<3){
			keymode++;	
		} else {
			keymode = 0;		
		}
		step = 0;
		break;
	}
	return 0;
}

bool Edit::makeBackground(dc_t* dc_buffer, void* pParam)
{
	State::makeBackground(dc_buffer, pParam);

	gx_clear( ( dc_t *)dc_buffer, gx_color( 0, 0, 0, 255));
	gx_bitblt( dc_buffer, 0, 0, ( dc_t *)back, 0, 0, back->width, back->height);	
	
	return true;
}
