#include "ECSFrameworkPCH.h"
#include "Thread.h"
#include "POSIXThread.h"

unsigned int IThread::ms_nCount = 0;

IThread::IThread()
    :m_eStatus(eTS_Idle)
{
    ms_nCount++;
}

IThread::~IThread()
{
    ms_nCount--;
}

void IThread::OnLock()
{
    if (m_spkMutex == nullptr)
        return;
    m_spkMutex->Lock();
}

void IThread::OnUnlock()
{
    if (m_spkMutex == nullptr)
        return;
    m_spkMutex->Unlock();
}

bool IThread::OnTryLock()
{
    if (m_spkMutex == nullptr)
        return false;
    return m_spkMutex->TryLock();
}

bool IThread::Impl_Execute()
{
    bool bReturn = false;
    if (OnTryLock()) {
        bReturn |= (m_spkWorker == nullptr);
        OnUnlock();
    }
    else {  // Skip Frame when setting
        return false;
    }

    if (bReturn)
        return true;

    return m_spkWorker->OnExecute(this);
}

void IThread::SetWorker(std::shared_ptr<IThreadWorker> _spkWorker)
{
    IThread::CScopeLock kLocker(*this);
    m_spkWorker = _spkWorker;
}

CLocalThread::CLocalThread()
{

}

CLocalThread::~CLocalThread()
{

}

bool CLocalThread::Resume()
{
    return true;
}

bool CLocalThread::Execute()
{
    return Impl_Execute();
}

bool CLocalThread::Suspend()
{
    return false;
}

bool CLocalThread::End()
{
    return true;
}

bool CLocalThread::Join(IThread* _pkOther)
{
    return false;
}

void CLocalThread::InitMutex()
{
    SetStatus(eTS_Execute);
}

uint64_t CLocalThread::GetID()
{
    return -1;
}

class ECS_API CPOSIXFactory : public IThreadFactory
{
public:
    CPOSIXFactory()
        : IThreadFactory("POSIXThread")
    {
    }

    virtual ~CPOSIXFactory()
    {
    }

    virtual std::shared_ptr <IThread> operator()()
    {
        return std::make_shared <CPOXISThread>();
    }
};

class ECS_API CLocalFactory : public IThreadFactory
{
public:
    CLocalFactory()
        : IThreadFactory("LocalThread")
    {
    }

    virtual ~CLocalFactory()
    {
    }

    virtual std::shared_ptr <IThread> operator()()
    {
        return std::make_shared <CLocalThread>();
    }
};

template<> CThreadFactoryMgr* TSingleton<CThreadFactoryMgr>::ms_pkSingleton = 0;

CThreadFactoryMgr::CThreadFactoryMgr()
{
    RegisterFactory(std::make_shared<CPOSIXFactory>());
    RegisterFactory(std::make_shared<CLocalFactory>());
}

CThreadFactoryMgr::~CThreadFactoryMgr()
{

}

class ECS_API CPOSIXMutexFactory : public IThreadMutexFactory
{
public:
    CPOSIXMutexFactory()
        : IThreadMutexFactory("POSIXMutex")
    {
    }

    virtual ~CPOSIXMutexFactory()
    {
    }

    virtual std::shared_ptr <IMutex> operator()()
    {
        return std::make_shared <CPOSIXMutex>();
    }
};

template<> CThreadMutexFactoryMgr* TSingleton<CThreadMutexFactoryMgr>::ms_pkSingleton = 0;

CThreadMutexFactoryMgr::CThreadMutexFactoryMgr()
{
    RegisterFactory(std::make_shared<CPOSIXMutexFactory>());
}

CThreadMutexFactoryMgr::~CThreadMutexFactoryMgr()
{

}