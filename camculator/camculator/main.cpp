//
//  main.cpp
//  camculator
//
//  Created by Christopher Kim on 6/6/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "define.h"
#include "camculator.h"

int main(int argc, const char * argv[])
{
	Camculator::createSingleton();
	
	Camculator::getSingleton().init();
	
	Camculator::getSingleton().main();
	
	Camculator::destroySingleton();

    return 0;
}

