//
//  WolframAlphaManager.cpp
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 5/13/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "WolframAlphaManager.h"
#include "tinyxml/tinyxml.h"

WolframAlphaManager::stMark WolframAlphaManager::mark[4] =
{
	{"%2B", 3, '+'},
	{"%2D", 3, '-'},
	{"%2A", 3, '*'},
	{"%2F", 3, '/'}
};

WolframAlphaManager::WolframAlphaManager(void)
{
	appId.clear();
	hostName.clear();
}

WolframAlphaManager::~WolframAlphaManager(void)
{
}

bool WolframAlphaManager::parseXmlResponse(char* pData)
{
	if (pData == NULL)
	{
		printf("data is null.\n");
		return false;
	}
	
	TiXmlDocument doc;
	doc.Parse(pData, 0, TIXML_ENCODING_UTF8);
	
//	doc.FirstChild("")
	
//	if (doc.Parse(pData, 0, TIXML_ENCODING_UTF8) == false)
//	{
//		printf("invalid xml data.");
//		return false;
//	}
	
	
	
}
//
//bool ConfigLoader::xmlParser(const char* fileName)
//{
//	if ( NULL == fileName )
//	{
//		logPrint("[Read Config] XML file name is NULL.");
//		return false;
//	}
//	
//	TiXmlDocument testDocument;
//	
//	if ( testDocument.LoadFile(fileName) == false )
//	{
//		logPrint("[Read Config] XML file cannot found. [%s]", fileName);
//		return false;
//	}
//	
//	TiXmlNode* pNode = testDocument.FirstChild("CONFIG")->FirstChild("CATEGORY");
//	
//	while ( NULL != pNode )
//	{
//		const char* pCategory = pNode->ToElement()->Attribute("name");
//		
//		TiXmlNode* pChildNode = pNode->FirstChild("DATA");
//		ConfigMap* pConfigMap = new ConfigMap;
//		
//		while ( NULL != pChildNode )
//		{
//			const char* pName = pChildNode->ToElement()->Attribute("name");
//			const char* pData = pChildNode->ToElement()->GetText();
//			
//			stringToLower( const_cast<char*>(pName) );
//			
//			logPrint("[Read Config][%14s] Name - %24s, Data - %s", pCategory, pName, pData);
//			pConfigMap->insert( ConfigMap::value_type((pName), pData) );
//			pChildNode = pChildNode->NextSibling("DATA");
//		}
//		
//		stringToLower( const_cast<char*>(pCategory) );
//		
//		m_CategoryMap.insert( CategoryMap::value_type(pCategory, pConfigMap) );
//		
//		pNode = pNode->NextSibling("CATEGORY");
//	}
//	
//	logPrint("[Read Config] XML file load complete.");
//	
//	return true;
//} // xmlParser()
