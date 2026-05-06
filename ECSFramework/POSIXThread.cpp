#include "ECSFrameworkPCH.h"
#include "POSIXThread.h"

CPOSIXMutex::CPOSIXMutex()
{
    pthread_mutex_init(&m_kMutex, nullptr);
}

CPOSIXMutex::~CPOSIXMutex()
{
    pthread_mutex_destroy(&m_kMutex);
}

void CPOSIXMutex::Lock()
{
    pthread_mutex_lock(&m_kMutex);
}

void CPOSIXMutex::Unlock()
{
    pthread_mutex_unlock(&m_kMutex);
}

bool CPOSIXMutex::TryLock()
{
    return !pthread_mutex_trylock(&m_kMutex);
}

void CPOSIXMutex::Wait(pthread_cond_t& _kCond)
{
    pthread_cond_wait(&_kCond, &m_kMutex);
}

CPOXISThread::CPOXISThread()
    :m_pkMutex(nullptr)
{
    memset(&m_kHandle, 0, sizeof(m_kHandle));
    pthread_cond_init(&m_kCondition, nullptr);
}

CPOXISThread::~CPOXISThread()
{
    pthread_cond_destroy(&m_kCondition);
}

void* CPOXISThread::Entry(void* _pThis)
{
    CPOXISThread* pThis = static_cast<CPOXISThread*>(_pThis);
    if (!pThis)
        return nullptr;

    pThis->Execute();

    return nullptr;
}

void CPOXISThread::InitMutex()
{
    std::shared_ptr<CPOSIXMutex> spkMutex = std::make_shared<CPOSIXMutex>();
    SetMutex(spkMutex);
    m_pkMutex = spkMutex.get();
}

bool CPOXISThread::Resume()
{
    EThreadStatus eStatus = GetStatus();
    switch (eStatus) {
    case eTS_Idle:
    case eTS_End:
        break;
    case eTS_Suspend:
    {
        IThread::CScopeLock kLock(*this);
        SetStatus(eTS_Execute);
        pthread_cond_signal(&m_kCondition);
        return true;
    }
    case eTS_Executing:
    case eTS_Execute:
        return true;
    case eTS_Ending:
    default:
        return false;
    }
    if (m_pkMutex == nullptr)
        InitMutex();
    SetStatus(eTS_Executing);
    pthread_create(&m_kHandle, nullptr, &CPOXISThread::Entry, this);
    return true;
}

bool CPOXISThread::Execute()
{
    EThreadStatus eStatus = GetStatus();
    if (eStatus != eTS_Executing)
        return false;
    {
        IThread::CScopeLock kLocker(*this);
        SetStatus(eTS_Execute);
    }

    bool bReturn = false;
    do {
        bReturn = Impl_Execute();
        // checking suspend
        while (GetStatus() == eTS_Suspend) {
            m_pkMutex->Wait(m_kCondition);
        }
    } while ((GetStatus() == eTS_Execute) && !bReturn);
    IThread::CScopeLock kLocker(*this);
    SetStatus(eTS_End);
    pthread_exit(nullptr);
    memset(&m_kHandle, 0, sizeof(m_kHandle));
    return true;
}

bool CPOXISThread::Suspend()
{
    EThreadStatus eStatus = GetStatus();
    if (eStatus != eTS_Execute)
        return false;

    IThread::CScopeLock kLocker(*this);
    SetStatus(eTS_Suspend);
    return true;
}

bool CPOXISThread::End()
{
    EThreadStatus eStatus = GetStatus();
    if (eStatus != eTS_Execute)
        return false;

    IThread::CScopeLock kLocker(*this);
    SetStatus(eTS_Ending);
    return true;
}

bool CPOXISThread::Join(IThread* _pkOther)
{
    CPOXISThread* pkPOSIX = static_cast<CPOXISThread*>(_pkOther);
    if (!pkPOSIX)
        return false;

    pthread_t& rkHandle = pkPOSIX->GetPthreadHandle();
    bool bRet = (pthread_join(rkHandle, nullptr) == 0);
    pkPOSIX->SetStatus(eTS_Joined);
    return bRet;
}

uint64_t CPOXISThread::GetID()
{
#if _WIN32
    return pthread_getunique_np(m_kHandle);
#else
    return -1;
#endif
}

pthread_t& CPOXISThread::GetPthreadHandle()
{
    return m_kHandle;
}