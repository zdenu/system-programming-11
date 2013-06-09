//
//  HttpConnector.h
//  HttpRequestTest
//
//  Created by Christopher Kim on 11/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
//	Usage : Instance를 만들어서 Request를 호출한다.

#pragma once

#include <sys/socket.h>
#include <map>
#include <string>

#define INVALID_SOCKET	-1
#define MAX_BUFFER		4096
#define MAX_RECV_BUFFER	1024768	* 2

#define IN
#define OUT

#define	HTTP_HEADER_CONTENT_LENGTH		"Content-Length"
#define HTTP_HEADER_CONTENT_TYPE		"Content-Type"
#define HTTP_HEADER_HTTP_RESULT			"HTTP"


enum ENUM_HTTP_REQUEST_RETURN_TYPE
{
	HTTP_REQUEST_FAIL			= 0,
	HTTP_REQUEST_SUCCESS		= 1,
};

enum ENUM_HTTP_REQUEST_ERROR_CODE
{
	HTTP_ERROR_NONE				= 0,
	HTTP_ERROR_SOCKET_FAIL,
	HTTP_ERROR_SIZE_MISMATCH,
	HTTP_ERROR_FILE_NOT_FOUND,
	HTTP_ERROR_CONNECT_FAIL,
	HTTP_ERROR_GET_HOST_FAIL,
	HTTP_ERROR_HEADER_PARSING_FAIL,
	HTTP_ERROR_INVALID_HEADER,
	HTTP_ERROR_UNKNOWN_ERROR,
};

typedef std::map<std::string, std::string> THttpHeaderMap;

class HttpConnector
{
public:
	HttpConnector();
	~HttpConnector();
	
public:
	int HttpRequest(IN const char* pHostName,
					IN const char* pRequest,
					OUT char* pBuffer);
	
	char* GetData(void) { return m_pData; }
	size_t GetDataSize(void) { return m_DataSize; }
	int GetLastError(void) { return m_LastErrorCode; }
	
	int ConnectionTest(void);
	
private:
	bool getDataFromServer(IN const char* pHostName,
						   IN const char* pRequest,
						   OUT char* pBuffer);
	
	bool headerDivider(IN const char* buffer,
					   OUT size_t& headerSize,
					   OUT char** body);
	
	bool headerParser(IN const char* pHeader,
					  OUT THttpHeaderMap* pHeaderContainer);
	
	bool isResultValid(IN const size_t headerSize);
	
	bool getAddrInfoByHostName(IN const char* hostName,
							   IN const char* port,
							   OUT sockaddr* retAddr);
	
	void setLastError(int errorCode) { m_LastErrorCode = errorCode; }
	
	
	void testCode(void);
	
private:
	int			m_Socket;
	
	char*		m_pRecvBuffer;
	char*		m_pSendBuffer;
	char*		m_pData;	// m_pRecvBuffer의 pointer를 사용하기 때문에 따로 생성하지 않는다. 대신 instance가 없는 곳에서 쓸려면 memcpy를 해야 함.
	
	size_t		m_DataSize;
	ssize_t		m_RecvSize;
	
	THttpHeaderMap m_HeaderContainer;
	
	int			m_LastErrorCode;
	bool		m_isUsedInstance;	// 하나의 request당 하나의 instance사용을 권장하기위하여.
	
};
