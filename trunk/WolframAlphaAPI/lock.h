//
//  lock.h
//  Framework
//
//  Created by Christopher Kim on 8/30/12.
//  Copyright (c) 2012 Christopher Kim. All rights reserved.
//
#pragma once

#include <pthread.h>

class Lock
{
public:
	Lock(void)
	{
		pthread_mutex_init(&m_Lock, 0);
	}
	~Lock()
	{
		pthread_mutex_destroy(&m_Lock);
	}
	
public:
	void lock(void)
	{
		pthread_mutex_lock(&m_Lock);
	}
	
	void unlock(void)
	{
		pthread_mutex_unlock(&m_Lock);
	}
	
	
private:
	pthread_mutex_t m_Lock;
};