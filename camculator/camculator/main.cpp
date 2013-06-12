//
//  main.cpp
//  camculator
//
//  Created by Christopher Kim on 6/6/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "define.h"
#include "camculator.h"
#include "WolframAlphaManager.h"
int main(int argc, const char * argv[])
{
	
//	WolframAlphaManager::createSingleton();
////	http://api.wolframalpha.com/v2/query?input=integral+x+dx+from+0+to+10&appid=EUGLWW-4XTP6Q3GK2
//	WolframAlphaManager::get().testSendRequest("input=integral+x+dx+from+0+to+10", strlen("input=integral+x+dx+from+0+to+10"));
	
	WolframAlphaManager::destroySingleton();
	return 0;
	
	Camculator::createSingleton();
	
	Camculator::get().init();
	Camculator::get().main();
	
	Camculator::destroySingleton();

    return 0;
}

