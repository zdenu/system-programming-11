//
//  thread.h
//  Framework
//
//  Created by Christopher Kim on 8/6/12.
//  Copyright (c) 2012 Christopher Kim. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <iostream>
//#include "atomic_value.h"

typedef pthread_t	THREAD_HANDLE;
typedef uint32_t	THREAD_ID;


template <typename T>
class Thread
{
public:
	Thread()
	: m_Handle(0)
	, m_pData(NULL)
	, m_pFunc(NULL)
	, m_IsRunning(false)
	, m_pInstance(NULL) {}
	~Thread() {}
	
public:
	typedef Thread<T> THREAD_TYPE;
	typedef void *(T::*THREAD_FUNC)(THREAD_TYPE*, void*);
	
	bool start(T* inst, THREAD_FUNC func, void* pData);
	bool stop(void);
	
	void* call(void);
	
	static void* threadProc( void* pParam );
	
	T* getInstance(void) { return m_pInstance; }
	bool isRunning(void) { return m_IsRunning; }
	
private:
	T*					m_pInstance;
	THREAD_FUNC			m_pFunc;
	void*				m_pData;
	
	THREAD_HANDLE		m_Handle;
	bool				m_IsRunning;
};

template <typename T>
bool Thread<T>::start(T* inst, THREAD_FUNC func, void* pData)
{	
	this->m_pInstance = inst;
	this->m_pFunc = func;
	this->m_pData = pData;
	this->m_IsRunning = true;
	
	int64_t ret = pthread_create(&m_Handle, NULL, threadProc, this);
	if (ret != 0)
	{
		m_IsRunning = false;
		return false;
	}
	
	return true;
}

template <typename T>
bool Thread<T>::stop(void)
{
	if (m_Handle == 0)
		return false;
	
//	pthread_kill(m_Handle, -9);
	m_IsRunning = false;
	
	return true;
}

template <typename T>
void* Thread<T>::call(void)
{
	// @Christopher : need to notify this thread is end?
	void* pRet = (m_pInstance->*((m_pFunc)))(this, m_pData);
	m_IsRunning = false;
	
	return pRet;
}

template <typename T>
void* Thread<T>::threadProc( void* pParam )
{
//	std::cout << "in threadproc" << std::endl;
	sleep(5);
	Thread<T>* pThread = (Thread<T>*)pParam;
	return pThread->call();
}


