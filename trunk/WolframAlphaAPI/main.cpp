//
//  main.cpp
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/13/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//
//http://api.wolframalpha.com/v2/query?input=sigma(k=1+to+8)+of+(x^2)+let+x={1,2,3,4,5,7,8,10}&appid=EUGLWW-4XTP6Q3GK2
//http://api.wolframalpha.com/v2/query?input=sigma(k=1+to+8)+of+(x^2)&appid=EUGLWW-4XTP6Q3GK2
//http://api.wolframalpha.com/v2/query?input=integral+x+dx&appid=EUGLWW-4XTP6Q3GK2
//http://api.wolframalpha.com/v2/query?input=integral+x+dx+from+0+to+10&appid=EUGLWW-4XTP6Q3GK2
#include <stdio.h>
#include <unistd.h>
//#include "HttpConnector.h"
//#include "WolframAlphaManager.h"
#include "TouchListener.h"
// scanner : id
// Identity : Input, Result
// Sum : Result
// Integral : IndefiniteIntegral
// Integral : Input, IndefiniteIntegral

int main(int argc, const char * argv[])
{

	TouchListener listener;
	
	listener.initialize();
	
	while(1)
	{
		sleep(100);
	}
	
//	std::string url = "api.wolframalpha.com";
//	std::string req = "/v2/query?input=1%2B2&appid=EUGLWW-4XTP6Q3GK2";
////integral+x+dx+from+0+to+10
//	HttpConnector connector;
//	int ret = connector.HttpRequest(url.c_str(), req.c_str(), NULL);
//	
//	if (ret == HTTP_REQUEST_FAIL)
//		return 0;
//	
//	char* pData = connector.GetData();
//	
//	WolframAlphaManager mgr;
//	
//	mgr.parseXmlResponse(pData);
//	
//	printf("%s\n", pData);
	
    return 0;
}

