//
//  atomic_value.h
//  Framework
//
//  Created by Christopher Kim on 11/2/12.
//  Copyright (c) 2012 Christopher Kim. All rights reserved.
//

#pragma once

#ifdef DEBUG
#define ATOMICVALUE_SIZE_CHECK()  if( sizeof(T) > sizeof(unsigned long long) ){ printf("AtomicValue size error : %lu %s\n", sizeof(T), __FUNCTION__); }
#else
#define ATOMICVALUE_SIZE_CHECK()
#endif

//
//
// http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
//
template<typename T>
struct AtomicValue
{
    volatile T value;
    
public:
    inline T get(void)
    {
        ATOMICVALUE_SIZE_CHECK();
		return value;
    }
    
    inline T set(T v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_lock_test_and_set(&value, v);
    }
    
    inline T swap(const T old_v, const T new_v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_val_compare_and_swap(&value, old_v, new_v);
    }
    inline bool cas(const T old_v, const T new_v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_bool_compare_and_swap(&value, old_v, new_v);
    }
    
    inline T addAndFetch(T v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_add_and_fetch(&value, v);
    }
	
    inline T subAndFetch( T v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_sub_and_fetch(&value, v);
    }
    
    inline T incAndFetch()
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_add_and_fetch(&value, 1);
    }
    
    inline T decAndFetch()
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_sub_and_fetch(&value, 1);
    }
	
    inline T fetchAndAdd(T v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_fetch_and_add(&value, v);
    }
    
    inline T fetchAndSub( T v)
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_fetch_and_sub(&value, v);
    }
    
    inline T fetchAndInc()
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_fetch_and_add(&value, 1);
    }
    
    inline T fetchAndDec()
    {
        ATOMICVALUE_SIZE_CHECK();
        return __sync_fetch_and_sub(&value, 1);
    }
};
