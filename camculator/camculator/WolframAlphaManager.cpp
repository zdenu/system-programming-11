//
//  WolframAlphaManager.cpp
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/13/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "define.h"
#include "WolframAlphaManager.h"
#include "HttpConnector.h"
#include "tinyxml/tinyxml.h"
#include "camculator.h"
#include "util.h"

WolframAlphaManager::stMark WolframAlphaManager::mark[4] =
{
	{"%2B", 3, '+'},
	{"%2D", 3, '-'},
	{"%2A", 3, '*'},
	{"%2F", 3, '/'}
};

template<> WolframAlphaManager*
Singleton<WolframAlphaManager>::ms_Singleton = 0;

WolframAlphaManager::WolframAlphaManager(void)
{
}

WolframAlphaManager::~WolframAlphaManager(void)
{
}

bool WolframAlphaManager::sendRequest(const char* expression, int size)
{
	if ( (expression == NULL) || size < 0 )
		return false;
	// make new thread.
	
	char* pBuf = new char[size];
	memcpy(pBuf, expression, size);
	printf("Send Request to wolframAlpha.\n");
	reqThreadHandle.start(this, &WolframAlphaManager::reqThread, (void*)expression);

	return true;
}


bool WolframAlphaManager::parseXmlResponse(char* pData, TUrlVector& urlVector)
{
	if (pData == NULL)
	{
		printf("data is null.\n");
		return false;
	}
	
	TiXmlDocument doc;
	doc.Parse(pData, 0, TIXML_ENCODING_UTF8);
	
	
	TiXmlNode* pNode = doc.FirstChild("queryresult")->FirstChild("pod");

	while (pNode != NULL)
	{
		const char* scanner = pNode->ToElement()->Attribute("scanner");
		const char* id		= pNode->ToElement()->Attribute("id");
		printf("%s\n", scanner);
		
//		// 각 연산별로 조건이 다르므로 일단 이걸로 처리해보고 넣는다.
//		if (strcmp(scanner, "Identity") == 0)
//		{
//			if ( (strcmp(id, "Input") == 0) || (strcmp(id, "Result") == 0) )
//			{
				const char* url = pNode->FirstChild("subpod")->FirstChild("img")->ToElement()->Attribute("src");
				stUrlInfo info;
				info.id = id;
				info.url = url;
				
				urlVector.push_back(info);
//			}
//		}
		
		pNode = pNode->NextSibling("pod");
	}
	
	return true;
	
}

void WolframAlphaManager::parseFromURL(stUrlInfo url,
									   std::string& host,
									   std::string& request)
{
	host.clear();
	request.clear();
	
	size_t pos = url.url.find_first_of('/');
	size_t st = 0;
	if (url.url[pos+1] == '/')
	{
		st = pos + 2;
		pos = url.url.find_first_of('/', pos+2);
	}
	
	host.append(url.url, st, pos - st);
	request.append(url.url, pos, std::string::npos);
	
	printf("Host: %s\n", host.c_str());
	printf("Request: %s\n", request.c_str());
}

void* WolframAlphaManager::reqThread(Thread<WolframAlphaManager>* pInstance, void* pParam)
{
	char* expression = (char*)pParam;
	std::string req = "/v2/query?input=";
	req.append(expression);
	req.append("&appid=");
	req.append(WOLFRAM_ALPHA_APP_ID);
	
	HttpConnector conn;
	
	if ( HTTP_REQUEST_FAIL ==
		conn.HttpRequest(WOLFRAM_ALPHA_HOST, req.c_str(), NULL))
	{
		printf("XML Http request is fail.\n");
		return false;
	}
	
	TUrlVector urlVector;
	parseXmlResponse(conn.GetData(), urlVector);
	
	std::string host;
	std::string request;
	
	stEvent* pEvent = new stEvent;
	TImageVector* pVector = new TImageVector;
	pEvent->eventType = EVENT_TYPE_HTTP_RESPONSE;
	
	for (int i = 0 ; i < urlVector.size() ; ++i)
	{
		HttpConnector imgConn;
		parseFromURL(urlVector[i], host, request);
		imgConn.HttpRequest(host.c_str(), request.c_str(), NULL);
		
		printf("Data : %s\n", imgConn.GetData());
		printf("Size : %d\n", imgConn.GetDataSize());
		
		int size = static_cast<int32_t>(imgConn.GetDataSize());
		char* pBuffer = new char[size];
		
		memcpy(pBuffer, imgConn.GetData(), size);
		
		char name[30] = {"\0"};
		sprintf(name, "%d.gif", i);
		FILE* fp = ::fopen(name, "wb+");
		size_t elements = fwrite(imgConn.GetData(), 1, imgConn.GetDataSize(), fp);
		fclose(fp);
		printf("Write Element Coutnts : %d\n", elements);
	
		stImageData* data = new stImageData;
		data->pVector = new TRGBVector;
		Util::GIF2RGB(name,
					  data->pVector,
					  data->size,
					  data->width,
					  data->height);
				
		pVector->push_back(data);
		delete pBuffer;
	}
	pEvent->pData = pVector;
	
	Camculator::get().pushEvent(pEvent);
	
	printf("image list get complete\n");
	
	// 필요한 이미지들을 request 한다.
	// 모든 이미지들을 받아온 후에 camculator event queue에 push한다.
	return NULL;
}

