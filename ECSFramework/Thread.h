#pragma once

#include "ScopeHelper.h"
#include "Factory.h"
#include "Util.h"

class IThread;
class ECS_API IThreadWorker
{
public:
    virtual ~IThreadWorker()
    {
    }

    virtual bool OnExecute(IThread* _pkThread) = 0;
};

class ECS_API IMutex
{
public:
    virtual ~IMutex()
    {
    }
    
    virtual void Lock() = 0;

    virtual void Unlock() = 0;

    virtual bool TryLock() = 0;
};

enum EThreadStatus
{
    eTS_Idle,
    eTS_Suspend,
    eTS_Executing,
    eTS_Execute,
    eTS_Ending,
    eTS_End,
    eTS_Joined,
};

class ECS_API IThread
{
    static unsigned int ms_nCount;
    std::shared_ptr <IThreadWorker> m_spkWorker;
    std::shared_ptr <IMutex> m_spkMutex;
    EThreadStatus m_eStatus;

protected:
    typedef TScopeHelper<IThread> CScopeLock;
    friend CScopeLock;

    inline void SetStatus(EThreadStatus _eStatus)
    {
        m_eStatus = _eStatus;
    }

    inline void SetMutex(std::shared_ptr<IMutex> _spkMutex)
    {
        m_spkMutex = _spkMutex;
    }

    virtual void InitMutex(){}

    virtual void OnLock();

    virtual void OnUnlock();

    virtual bool OnTryLock();

    inline void EnterScope()
    {
        OnLock();
    }

    inline void LeaveScope()
    {
        OnUnlock();
    }

    virtual bool Impl_Execute();

    virtual bool Execute() = 0;
public:

    IThread();

    virtual ~IThread();

    virtual bool Resume() = 0;

    virtual bool Suspend() = 0;

    virtual bool End() = 0;

    virtual bool Join(IThread* _pkOther) = 0;

    virtual uint64_t GetID() = 0;

    inline unsigned int GetCountID()
    {
        return ms_nCount;
    }

    inline EThreadStatus GetStatus()
    {
        CScopeLock kLock(*this);
        EThreadStatus eStatus = m_eStatus;
        return eStatus;
    }

    virtual bool IsLocal()
    {
        return false;
    }

    void SetWorker(std::shared_ptr<IThreadWorker> _spkWorker);

    inline IThreadWorker* GetCurrentWorker()
    {
        return m_spkWorker.get();
    }
};

class ECS_API CLocalThread : public IThread
{
    void InitMutex();
public:

    CLocalThread();

    virtual ~CLocalThread();

    virtual bool Resume();

    virtual bool Execute();

    virtual bool Suspend();

    virtual bool End();

    virtual bool Join(IThread* _pkOther);

    virtual bool OnTryLock()
    {
        return true;
    }

    uint64_t GetID();

    bool IsLocal()
    {
        return true;
    }
};

struct ECS_API IThreadFactory : public IFactoryObj <IThread, std::string>
{
    IThreadFactory(std::string _kType)
        : IFactoryObj <IThread, std::string>(_kType)
    {
    }

    virtual std::shared_ptr <IThread> operator()() = 0;
};


class ECS_API CThreadFactoryMgr
    : public TSingleton <CThreadFactoryMgr>
    , public IFactoryMgr <IThread, std::string>
{
public:
    CThreadFactoryMgr();

    virtual ~CThreadFactoryMgr();
};

struct ECS_API IThreadMutexFactory : public IFactoryObj <IMutex, std::string>
{
    IThreadMutexFactory(std::string _kType)
        : IFactoryObj <IMutex, std::string>(_kType)
    {
    }

    virtual std::shared_ptr <IMutex> operator()() = 0;
};

class ECS_API CThreadMutexFactoryMgr
    : public TSingleton <CThreadMutexFactoryMgr>
    , public IFactoryMgr <IMutex, std::string>
{
public:
    CThreadMutexFactoryMgr();

    virtual ~CThreadMutexFactoryMgr();
};