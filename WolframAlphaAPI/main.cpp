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
#include <string.h>
#include <stdlib.h>

#include "Define.h"

#include "HttpConnector.h"
#include "WolframAlphaManager.h"
#include "util.h"

#include "gif_lib.h"
#include "gif_lib_private.h"
//#include "TouchListener.h"
// scanner : id
// Identity : Input, Result
// Sum : Result
// Integral : IndefiniteIntegral
// Integral : Input, IndefiniteIntegral

int main(int argc, const char * argv[])
{
	
	TUrlVector urlVector;
	
	std::string host;
	std::string request;
	int i = 0;
//	for (int i = 0 ; i < urlVector.size() ; ++i)
	{
		host.clear();
		request.clear();
		HttpConnector req;
		
		printf("%s: %s\n", urlVector[i].id.c_str(), urlVector[i].url.c_str());
		size_t pos = urlVector[i].url.find_first_of('/');
		size_t st = 0;
		if (urlVector[i].url[pos+1] == '/')
		{
			st = pos + 2;
			pos = urlVector[i].url.find_first_of('/', pos+2);
		}
		
		host.append(urlVector[i].url, st, pos - st);
		request.append(urlVector[i].url, pos, std::string::npos);
		
		printf("Host: %s\n", host.c_str());
		printf("Request: %s\n", request.c_str());
		
		req.HttpRequest(host.c_str(), request.c_str(), NULL);
		
		size_t dataSize = req.GetDataSize();
		char* buffer = new char[dataSize];
		
		memcpy(buffer, req.GetData(), dataSize);
		
		char name[30] = {"\0"};
		sprintf(name, "test%d.gif", i);
		FILE* fp = fopen(name, "wb+");
		fwrite(buffer, dataSize, 1, fp);
		fclose(fp);
		Util::dumpMemory(buffer, (int)dataSize);
		int errCode = 0;
//		GifFileType* pGifFile = DGifOpenFileName(name, &errCode);
		
		GIF2RGB(1, name, true, "output.txt");
		
		
		
//		GIF2RAW(pGifFile, true);
		
//		DGifSlurp(pGifFile);
//		SavedImage* img = pGifFile->SavedImages;
//		Util::dumpMemory(img->RasterBits, strlen((const char*)img->RasterBits));
		
		
	}
	
    return 0;
}

