//
//  signal.h
//  Framework
//
//  Created by Christopher Kim on 11/2/12.
//  Copyright (c) 2012 Christopher Kim. All rights reserved.
//

#pragma once

#include <sys/stat.h>
#include "atomic_value.h"

#define	NONSIGNALED	false
#define	SIGNALED	true

#define FCODE_SUCCESS						1
#define FCODE_FAIL							100
#define FCODE_CLOSE							0
#define FCODE_TIMEDOUT						160


struct Signal
{
	AtomicValue<int> state;	//SIGNALED or NONSIGNALED
    bool manual_reset;
	
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}; // Signal

inline bool createSignal(Signal* pSignal, bool manual_reset = false)
{
    if( pSignal == NULL )
        return false;
	
    if(0 != pthread_mutex_init(&(pSignal->mutex), NULL))
	{
		return false;
	}
	
    if(0 != pthread_cond_init(&(pSignal->cond), NULL))
    {
        pthread_mutex_destroy(&(pSignal->mutex));
        return false;
    }
	
    pSignal->state.set(NONSIGNALED);
    pSignal->manual_reset = manual_reset;
	
    return true;
} // createSignal()

inline bool destroySignal(Signal* pSignal)
{
    if( pSignal == NULL )
        return false;
	
    if(0 != pthread_mutex_destroy(&(pSignal->mutex)))
	{
		return false;
	}
	
    if(0 != pthread_cond_destroy(&(pSignal->cond)))
    {
        return false;
    }
    return true;
} // destroySignal()

inline bool setSignal(Signal* pSignal)
{
	bool ret;
	
    if( pSignal == NULL )
        return false;
	
	//    thread pooler 에서 버그 있어서 사용 못함
    if( pSignal->state.cas(NONSIGNALED, SIGNALED) == false )
    {
        return true;
    }
	
	if(0 != (ret = pthread_mutex_lock(&(pSignal->mutex))))
	{
		return false;
	}
	
    pSignal->state.set(SIGNALED);
	
    if(0 != (ret = pthread_mutex_unlock(&(pSignal->mutex))))
	{
		return false;
	}
	
    if(0 != (ret = pthread_cond_signal(&(pSignal->cond))))
	{
		return false;
	}
	
    return true;
} // setSignal()

inline bool notifySignal(Signal* pSignal)
{
	return setSignal(pSignal);
} // notifySignal()

inline bool resetSignal(Signal* pSignal)
{
    if( pSignal == NULL )
        return false;
	
	int64_t ret;
	if(0 != (ret = pthread_mutex_lock(&(pSignal->mutex))))
	{
		return false;
	}
	
    pSignal->state.set(NONSIGNALED);
	
    if(0 != (ret = pthread_mutex_unlock(&(pSignal->mutex))))
	{
		return false;
	}
	
    return true;
} // resetSignal()

inline int64_t waitSignal(Signal* pSignal, uint64_t waitTimeAtMicroLevel)
{
	int64_t result;
	struct timeval now;
	struct timespec timeout;
	int64_t retcode;
	//	const uint64_t kOneSecAtNanoLevel = 1000000000; // one second in nano scale.
	if( pSignal == NULL )
    {
        return FCODE_FAIL;
    }
	
	if(0 != (retcode = pthread_mutex_lock(&(pSignal->mutex))))
	{
		return FCODE_FAIL;
	}
	
	if(0 == waitTimeAtMicroLevel)
	{
        while(NONSIGNALED == pSignal->state.get())
        {
            pthread_cond_wait(&(pSignal->cond), &(pSignal->mutex));
        }
		
        result = FCODE_SUCCESS;
    }
	else
    {
        if(gettimeofday(&now, NULL) == 0)
        {
            timeout.tv_sec = now.tv_sec + waitTimeAtMicroLevel/1000000;
            timeout.tv_nsec = now.tv_usec*1000 + waitTimeAtMicroLevel%1000000;
            retcode = 0;
			
            while(NONSIGNALED == pSignal->state.get() && ETIMEDOUT != retcode)
            {
                retcode = pthread_cond_timedwait(&(pSignal->cond), &(pSignal->mutex), &timeout);
                if (EINVAL == retcode)
                {
                    result = FCODE_FAIL;
                }
            }
            if (0 == retcode)
            {
                result = FCODE_SUCCESS;
            }
            else if (ETIMEDOUT == retcode)
            {
                result = FCODE_TIMEDOUT;
            }
            else
				
            {
                result = FCODE_FAIL;
            }
        }
        else
        {
            result = FCODE_FAIL;
        }
	}
	
	if(FCODE_SUCCESS == result)
	{
		if(true == pSignal->manual_reset)
		{
			if (0 != (retcode = pthread_mutex_unlock(&(pSignal->mutex))))
			{
				return FCODE_FAIL;
			}
			if (0 != (retcode = pthread_cond_signal(&(pSignal->cond))))
			{
				return FCODE_FAIL;
			}
			return result;
		}
		else
		{
			pSignal->state.set(NONSIGNALED);
		}
	}
	
    if(0 != (retcode = pthread_mutex_unlock(&(pSignal->mutex))))
	{
		return FCODE_FAIL;
	}
	
	return result;
} // waitSignal()

inline int64_t waitSignal(Signal* pSignal)
{
	return waitSignal(pSignal, 0);
} // waitSignal()


inline int custDaemon (int nochdir, int noclose)
{
	umask(0);
	
	pid_t pid;
	pid = fork ();
	
	/* In case of fork is error. */
	if (pid < 0)
	{
		printf("fork\n");
		return -1;
	}
	
	/* In case of this is parent process. */
	if (pid != 0)
	{
		//		printf("exit daemon %d\n", pid);
		exit (0);
	}
	
	/* Become session leader and get pid. */
	pid = setsid();
	if (pid < -1)
	{
		perror ("setsid");
		return -1;
	}
	
	/* Change directory to root. */
	if (! nochdir)
	{
		chdir ("/");
	}
	
	/* File descriptor close. */
	if (! noclose)
	{
		int fd;
		
		fd = open ("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{
			dup2 (fd, STDIN_FILENO);
			dup2 (fd, STDOUT_FILENO);
			dup2 (fd, STDERR_FILENO);
			if (fd > 2)
				close (fd);
		}
	}
	
	umask (0027);
	
	return 0;
} // custDaemon()
