#pragma once

#include "Thread.h"
// pthread configuration temprary when split interface between pthread & std::thread
#define __PTW32_STATIC_LIB
#include <pthread.h>

class ECS_API CPOSIXMutex : public IMutex
{
    pthread_mutex_t m_kMutex;
public:

    CPOSIXMutex();

    virtual ~CPOSIXMutex();

    void Lock();

    void Unlock();

    bool TryLock();

    void Wait(pthread_cond_t& _kCond);
};

class ECS_API CPOXISThread : public IThread
{
    CPOSIXMutex* m_pkMutex;
    pthread_cond_t m_kCondition;
    pthread_t m_kHandle;

    static void* Entry(void* _pThis);

    void InitMutex();
public:

    CPOXISThread();

    virtual ~CPOXISThread();

    virtual bool Resume();

    virtual bool Execute();

    virtual bool Suspend();

    virtual bool End();

    virtual bool Join(IThread* _pkOther);

    virtual uint64_t GetID();

    pthread_t& GetPthreadHandle();
};