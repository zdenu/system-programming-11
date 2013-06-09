//
//  HttpConnector.cpp
//  HttpRequestTest
//
//  Created by Christopher Kim on 11/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "HttpConnector.h"

HttpConnector::HttpConnector()
: m_pRecvBuffer(NULL)
, m_pSendBuffer(NULL)
, m_pData(NULL)
, m_DataSize(0)
, m_RecvSize(0)
, m_Socket(INVALID_SOCKET)
, m_isUsedInstance(false)
{
	m_pRecvBuffer = new char[MAX_RECV_BUFFER];
	memset(m_pRecvBuffer, 0x00, MAX_RECV_BUFFER);
	
	m_pSendBuffer = new char[MAX_BUFFER];
	memset(m_pSendBuffer, 0x00, MAX_BUFFER);
}

HttpConnector::~HttpConnector()
{
	if (m_pRecvBuffer != NULL)
		delete [] m_pRecvBuffer;
	
	if (m_pSendBuffer != NULL)
		delete [] m_pSendBuffer;
}

int HttpConnector::HttpRequest(IN const char* pHostName,
							   IN const char* pRequest,
							   OUT char* pBuffer)
{
	if ( (pHostName == NULL) ||
		 (pRequest == NULL) ||
		 (m_isUsedInstance == true) )
		return HTTP_REQUEST_FAIL;

	if ( getDataFromServer(pHostName, pRequest, m_pRecvBuffer) == false )
		return HTTP_REQUEST_FAIL;

	size_t headerSize = 0;
	
	if (headerDivider(m_pRecvBuffer, headerSize, &m_pData) == false)
		return HTTP_REQUEST_FAIL;
	
	headerParser(m_pRecvBuffer, &m_HeaderContainer);
	
	if (isResultValid(headerSize) == false)
		return HTTP_REQUEST_FAIL;
		
	// outBuffer가 없을 경우에는 복사하지 않는다.
	if (pBuffer != NULL)
		memcpy(pBuffer, m_pData, m_DataSize);
	
	m_isUsedInstance = true;
	
	return HTTP_REQUEST_SUCCESS;
}



bool HttpConnector::getDataFromServer(IN const char* pHostName,
									  IN const char* pRequest,
									  OUT char* pBuffer)
{
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == INVALID_SOCKET)
	{
		setLastError(HTTP_ERROR_SOCKET_FAIL);
		return HTTP_REQUEST_FAIL;	// socket creation error.
	}
	
	struct sockaddr retAddr;
    if (getAddrInfoByHostName(pHostName, "80", &retAddr) == false)
	{
		setLastError(HTTP_ERROR_GET_HOST_FAIL);
        return HTTP_REQUEST_FAIL;
	}
	
	if (-1 == ::connect(m_Socket, (struct sockaddr *)&retAddr, sizeof(retAddr)))
	{
		setLastError(HTTP_ERROR_CONNECT_FAIL);
		return HTTP_REQUEST_FAIL;
	}
	
	snprintf(m_pSendBuffer,
			 MAX_BUFFER,
			 "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n",
			 pRequest,
			 pHostName );
	
	
	write(m_Socket, m_pSendBuffer, strlen(m_pSendBuffer));
	
	ssize_t n = 0;
	while ( (n = read(m_Socket, &pBuffer[m_RecvSize], MAX_RECV_BUFFER)) > 0 )
	{
		m_RecvSize += n;
		usleep(1000);
	}
	
	close(m_Socket);
	m_Socket = INVALID_SOCKET;
	
	return true;
}

bool HttpConnector::headerDivider(IN const char* buffer,
								  OUT size_t& headerSize,
								  OUT char** body)
{
	const char* delimiter = "\r\n\r\n";
	
	char* ret = strstr(const_cast<char*>(buffer), delimiter);
	
	if (ret == NULL)
	{
		setLastError(HTTP_ERROR_UNKNOWN_ERROR);
		return false;
	}
	
	for (int i = 0 ; i < strlen(delimiter) ; i++)
		ret[i] = '\0';
	
	headerSize = ret - buffer;
	(*body) = ret + strlen(delimiter);
	
	return true;
}


bool HttpConnector::headerParser(IN const char* pHeader,
								 OUT THttpHeaderMap* pHeaderContainer)
{
	const char* delimiter = "\r\n";
	const size_t delimiterLength = strlen(delimiter);
	
	char* pLineArray[256];
	
	char* pHeadPointer = const_cast<char*>(pHeader);
	char* pInterpreter = NULL;
	
	int index = 0;
	
	while ( (pInterpreter = strstr(pHeadPointer, delimiter)) != NULL )
	{
		pLineArray[index] = pHeadPointer;
		
		for (int i = 0 ; i < delimiterLength ; i++)
			pInterpreter[i] = '\0';
		
		pHeadPointer = pInterpreter + delimiterLength;
		index++;
	}
	
	// 각 header별로 parsing.
	// 첫 번째 줄만 특이하다. HTTP/1.1 200 OK 이런식으로 나옴.
	// 버전을 빼고 key값으로 잡자.
	
	pInterpreter = strstr(pLineArray[0], "/");
	
	if (pInterpreter == NULL)
	{
		setLastError(HTTP_ERROR_HEADER_PARSING_FAIL);
		return false;
	}
	
	(*pInterpreter) = '\0';
	pInterpreter += 1;
	
	pInterpreter = strstr(pInterpreter, " ");
	if (pInterpreter == NULL)
	{
		setLastError(HTTP_ERROR_HEADER_PARSING_FAIL);
		return false;
	}
	
	(*pInterpreter) = '\0';
	pInterpreter += 1;
	
	
	// 숫자 뒷부분의 문자열을 떼고 숫자만 집어넣자.
	char* pTemp = strstr(pInterpreter, " ");
	if (pTemp == NULL)
	{
		setLastError(HTTP_ERROR_HEADER_PARSING_FAIL);
		return false;
	}
	(*pTemp) = '\0';
	
	pHeaderContainer->insert(THttpHeaderMap::value_type(pLineArray[0], pInterpreter));
	
	//나머지들은 ": "로 구분한다.
	
	const char* delimiter2 = ": ";
	const size_t delimiterLength2 = strlen(delimiter2);
	
	for (int i = 1 ; i < index ; i++)
	{
		pInterpreter = strstr(pLineArray[i], delimiter2);
		if (pInterpreter == NULL)
		{
			setLastError(HTTP_ERROR_HEADER_PARSING_FAIL);
			continue;
		}
		
		(*pInterpreter) = '\0';
		*(pInterpreter + 1) = '\0';
		
		pInterpreter += delimiterLength2;
		
		pHeaderContainer->insert(THttpHeaderMap::value_type(pLineArray[i], pInterpreter));
	}
	
		
	return true;
}


bool HttpConnector::isResultValid(IN const size_t headerSize)
{
	// data Errorcode 검증.
	THttpHeaderMap::iterator iter = m_HeaderContainer.find(HTTP_HEADER_HTTP_RESULT);
	if (iter == m_HeaderContainer.end())
	{
		setLastError(HTTP_ERROR_INVALID_HEADER);
		return false;
	}
	
	int httpResult = atoi( (iter->second).c_str() );
	
	if (httpResult != 200)
	{
		setLastError(HTTP_ERROR_FILE_NOT_FOUND);
		return false;
	}	

	// data 길이 검증.
	iter = m_HeaderContainer.find(HTTP_HEADER_CONTENT_LENGTH);
	
	if (iter == m_HeaderContainer.end())
	{
		setLastError(HTTP_ERROR_INVALID_HEADER);
		return false;
	}
	
	int dataSize = atoi( (iter->second).c_str() );
	
	if ( (m_RecvSize - headerSize - 4) != dataSize )
	{
		setLastError(HTTP_ERROR_SIZE_MISMATCH);
		return false;
	}
	
	m_DataSize = dataSize;

	return true;
}


bool HttpConnector::getAddrInfoByHostName(IN const char* hostName,
										  IN const char* port,
										  OUT sockaddr* retAddr)
{
    struct addrinfo *retAddrInfo = NULL;
    struct addrinfo hints;
    
    memset(retAddr, 0, sizeof(sockaddr));
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	
    
    if (0 != getaddrinfo(hostName, port, &hints, &retAddrInfo))
    {
        printf(" \n....getaddr fail");
        if (NULL != retAddrInfo)
        {
            freeaddrinfo(retAddrInfo);
        }
        return false;
    }
    
    memcpy(retAddr, retAddrInfo->ai_addr, sizeof(sockaddr));
    
	//    printf("\n[hostName : %s, port : %s] => [%d.%d.%d.%d]\n",  
	//           hostName, port,
	//           (unsigned char)retAddr->sa_data[2], 
	//           (unsigned char)retAddr->sa_data[3], 
	//           (unsigned char)retAddr->sa_data[4],
	//           (unsigned char)retAddr->sa_data[5]);
    freeaddrinfo(retAddrInfo);
    return true;
}



void HttpConnector::testCode(void)
{
	FILE* fp = fopen("testImage.png", "wa");
	if (fp == NULL)
		return;
	
	fwrite(m_pData, sizeof(char), m_DataSize, fp);
	
	fclose(fp);
		
}


int HttpConnector::ConnectionTest(void)
{
	if (INVALID_SOCKET == (m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
		return HTTP_REQUEST_FAIL;
	
	struct sockaddr retAddr;
    if (getAddrInfoByHostName("img.linkoon.co.kr", "80", &retAddr) == false)
	{
		setLastError(HTTP_ERROR_GET_HOST_FAIL);
        return false;
	}
	
	if (-1 == ::connect(m_Socket, (struct sockaddr *)&retAddr, sizeof (retAddr)) )
	{
		setLastError(HTTP_ERROR_CONNECT_FAIL);
		return false;
	}
	
//	snprintf(m_pSendBuffer, MAX_BUFFER, "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n", pRequest, pHostName );
//	
//	//printf("SendBuffer : \n%s\n", m_pSendBuffer);
//
//	write(m_Socket, m_pSendBuffer, strlen(m_pSendBuffer));
//	
//	ssize_t n = 0;
//	while ( (n = read(m_Socket, &pBuffer[m_RecvSize], MAX_RECV_BUFFER)) > 0 )
//	{
//		m_RecvSize += n;
//		usleep(1000);
//	}
	
	close(m_Socket);
	m_Socket = INVALID_SOCKET;
	
	return true;

}
















