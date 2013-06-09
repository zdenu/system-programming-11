//
//  WolframAlphaManager.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/13/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//
#pragma once

#include <string>
#include <vector>
#include <map>
#include "singleton.h"
#include "thread.h"

#define APP_ID_LENGTH	20

struct stUrlInfo
{
	std::string id;
	std::string url;
};

typedef std::vector<stUrlInfo> TUrlVector;
typedef std::vector<std::string> TImgNameVector;
typedef std::vector<stImageData> TImageVector;
class WolframAlphaManager : public Singleton<WolframAlphaManager>
{
public:
	WolframAlphaManager(void);
	~WolframAlphaManager(void);
	
public:
	bool sendRequest(const char* expression, int size);
	
public:
//	const char* getRequest(void) { return request.c_str(); }
	const char* makeRequest(const char* pRequest);
	
	bool parseXmlResponse(char* pData, TUrlVector& urlVector);
	
private:
	void* reqThread(Thread<WolframAlphaManager>* pInstance, void* pParam);
	
private:
	void parseFromURL(stUrlInfo url, std::string& host, std::string& request);
	
	Thread<WolframAlphaManager> reqThreadHandle;
	
private:
	
	struct stMark
	{
		const char* str;
		unsigned int len;
		char		chr;
	};
	
	static stMark mark[4];
};