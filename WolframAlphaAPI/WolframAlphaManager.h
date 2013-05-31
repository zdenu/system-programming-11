//
//  WolframAlphaManager.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/13/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//
#pragma once

#include <string>

#define APP_ID_LENGTH	20

class WolframAlphaManager
{
public:
	WolframAlphaManager(void);
	~WolframAlphaManager(void);
	
public:
	void setAppId(const char* pAppId) { appId = pAppId; }
	void setHostName(const char* pHost) { hostName = pHost; }
	
	const char* getHostName(void) { return hostName.c_str(); }
	const char* getRequest(void) { return request.c_str(); }
	
	const char* makeRequest(const char* pRequest);
	
	bool parseXmlResponse(char* pData);
	
public:
	// make integral.
	// make arithmetic.
	
	
	
	
private:
	
	struct stMark
	{
		const char* str;
		unsigned int len;
		char		chr;
	};
	
	static stMark mark[4];
	
private:
	std::string appId;
	std::string hostName;
	std::string request;
};