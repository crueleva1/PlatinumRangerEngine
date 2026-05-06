#include "ECSFrameworkPCH.h"
#include "EventSystem.h"
#if _WIN32
#include "windows.h"
#else
#include <chrono>
#include <thread>
#endif

template<> CEventSystem* TSingleton<CEventSystem>::ms_pkSingleton = 0;

class ECS_API CEventWorker 
    : public IThreadWorker
    , public IDelegate
{
    typedef std::multimap <unsigned short, std::shared_ptr<IFunctionInterface> > CPriorityExecuteMap;
    typedef CPriorityExecuteMap::iterator CExeIterator;
    typedef std::pair <CExeIterator, CExeIterator> CExeIteratorPair;
    CPriorityExecuteMap m_kExecuteMap;
    typedef std::map <std::string, unsigned short> CPriorityKeyMap;
    typedef CPriorityKeyMap::iterator CKeyIterator;
    CPriorityKeyMap m_kKeyMap;
    CEventSystem& m_rkEvent;
    unsigned short m_nDefaultPriority;
public:
    CEventWorker(CEventSystem& _rkSystem)
        :m_rkEvent(_rkSystem)
        ,m_nDefaultPriority(1000)
    {

    }

    bool OnExecute(IThread* _pkThread)
    {
        bool bRet = false;
        for (auto& kPair : m_kExecuteMap) {
            IFunctionInterface* pkFunction = kPair.second.get();
            switch (pkFunction->GetType())
            {
            case eDFT_Function:
            {
                bRet &= static_cast<TFunctionInterface<bool, IThread*>*>(pkFunction)->Invoke(std::forward<IThread*>(_pkThread));
                break;
            }
            case eDFT_MemberFunction:
            {
                bRet &= static_cast<TFunctionInterface<bool, IThread*>*>(pkFunction)->Invoke(std::forward<IThread*>(_pkThread));
                break;
            }
            default:
                break;
            }
            if (bRet)
                break;
        }
        return bRet;
    }

    bool RegisterDelegateByPriority(unsigned short _nPriority, std::shared_ptr<IFunctionInterface> _spkFunction)
    {
        CKeyIterator kKeyIt = m_kKeyMap.find(_spkFunction->GetFunctionName());
        if (kKeyIt != m_kKeyMap.end())
            return false;
        m_kKeyMap.insert(std::make_pair(_spkFunction->GetFunctionName(), _nPriority));
        m_kExecuteMap.insert(std::make_pair(_nPriority, _spkFunction));
        return true;
    }

    bool UnregisterDelegateByPriority(unsigned short _nPriority, std::shared_ptr<IFunctionInterface> _spkFunction)
    {
        CKeyIterator kKeyIt = m_kKeyMap.find(_spkFunction->GetFunctionName());
        if (kKeyIt == m_kKeyMap.end())
            return false;
        if (kKeyIt->second != _nPriority)
            return false;
        CExeIterator kHeadIt = m_kExecuteMap.find(_nPriority);
        if (kHeadIt == m_kExecuteMap.end())
            return false;
        for (CExeIterator kIt = kHeadIt; kIt != m_kExecuteMap.end();) {
            if (kIt->first != _nPriority)
                break;
            if (kIt->second == _spkFunction) {
                m_kExecuteMap.erase(kIt);
                m_kKeyMap.erase(_spkFunction->GetFunctionName());
                return true;
            }
        }
        return false;
    }

    bool UnregisterDelegateByPriority(unsigned short _nPriority, const char* _pcFunctionName)
    {
        CKeyIterator kKeyIt = m_kKeyMap.find(_pcFunctionName);
        if (kKeyIt == m_kKeyMap.end())
            return false;
        if (kKeyIt->second != _nPriority)
            return false;
        CExeIterator kHeadIt = m_kExecuteMap.find(_nPriority);
        if (kHeadIt == m_kExecuteMap.end())
            return false;
        for (CExeIterator kIt = kHeadIt; kIt != m_kExecuteMap.end();) {
            if (kIt->first != _nPriority)
                break;
            if (kIt->second->GetFunctionName() == _pcFunctionName) {
                m_kExecuteMap.erase(kIt);
                m_kKeyMap.erase(_pcFunctionName);
                return true;
            }
        }
        return false;
    }

    virtual bool RegisterDelegate(std::shared_ptr<IFunctionInterface> _spkFunction)
    {
        return RegisterDelegateByPriority(m_nDefaultPriority, _spkFunction);
    }

    virtual void UnregisterDelegate(std::shared_ptr<IFunctionInterface> _spkFunction)
    {
        UnregisterDelegateByPriority(m_nDefaultPriority, _spkFunction);
    }

    virtual void UnregisterDelegate(const char* _pcFunctionName)
    {
        CKeyIterator kKeyIt = m_kKeyMap.find(_pcFunctionName);
        if (kKeyIt == m_kKeyMap.end())
            return;
        CExeIterator kHeadIt = m_kExecuteMap.find(kKeyIt->second);
        if (kHeadIt == m_kExecuteMap.end())
            return;
        for (CExeIterator kIt = kHeadIt; kIt != m_kExecuteMap.end();) {
            if (kIt->first != kKeyIt->second)
                break;
            if (!strcmp(kIt->second->GetFunctionName(), _pcFunctionName)) {
                m_kExecuteMap.erase(kIt);
                m_kKeyMap.erase(_pcFunctionName);
                return;
            }
        }
    }

    virtual void UnregisterDelegate(void* _pInstnace, const char* _pcFunctionName)
    {
        CKeyIterator kKeyIt = m_kKeyMap.find(_pcFunctionName);
        if (kKeyIt == m_kKeyMap.end())
            return;
        CExeIterator kHeadIt = m_kExecuteMap.find(kKeyIt->second);
        if (kHeadIt == m_kExecuteMap.end())
            return;
        for (CExeIterator kIt = kHeadIt; kIt != m_kExecuteMap.end();) {
            if (kIt->first != kKeyIt->second)
                break;
            if (!strcmp(kIt->second->GetFunctionName(), _pcFunctionName)) {
                if (kIt->second->GetType() != eDFT_MemberFunction)
                    continue;
                if (kIt->second->GetInsPtr() != _pInstnace)
                    continue;
                m_kExecuteMap.erase(kIt);
                m_kKeyMap.erase(_pcFunctionName);
                return;
            }
        }
    }

    virtual bool IsExist(const char* _pcFunctionName)
    {
        return m_kKeyMap.find(_pcFunctionName) != m_kKeyMap.end();
    }
};

CEventSystem::CEventSystem(unsigned int _nWorkCount, EEventDrivenType _eType)
    :m_nWorkerCount(_nWorkCount)
    ,m_eDrivenType(_eType)
{
    // Init Thread Factory Singleton
    CThreadFactoryMgr::New();
    CThreadMutexFactoryMgr::New();

    m_kThreads.resize(m_nWorkerCount);
    switch (m_eDrivenType) {
    case eED_SyncOnly:
    {
        IThreadFactory* pkFactory = CThreadFactoryMgr::GetInstance().GetFactoryByType<IThreadFactory>("LocalThread");
        for (auto& kThread : m_kThreads) {
            kThread = (*pkFactory)();
            kThread->SetWorker(std::make_shared<CEventWorker>(*this));
        }
    }
    break;
    case eED_Async:
    {
        IThreadFactory* pkFactory = CThreadFactoryMgr::GetInstance().GetFactoryByType<IThreadFactory>("LocalThread");
        IThreadFactory* pkAsyncFactory = CThreadFactoryMgr::GetInstance().GetFactoryByType<IThreadFactory>("POSIXThread");
        unsigned int nCount = 0;
        for (auto& kThread : m_kThreads) {
            kThread = (nCount > 0) ? (*pkAsyncFactory)() : (*pkFactory)();
            kThread->SetWorker(std::make_shared<CEventWorker>(*this));
            nCount++;
        }
    }
    break;
    default:
        break;
    }
}

CEventSystem::~CEventSystem()
{
    // Destroy Thread Factory Singleton
    CThreadFactoryMgr::Delete();
    CThreadMutexFactoryMgr::Delete();
}

CEventWorker* CEventSystem::GetWorker(unsigned int _eWorkerType)
{
    if (_eWorkerType >= m_kThreads.size())
        return nullptr;

    return dynamic_cast<CEventWorker*>(m_kThreads[_eWorkerType]->GetCurrentWorker());
}

void CEventSystem::configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents)
{
    
}

void CEventSystem::update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta)
{
    for (auto& kThread : m_kThreads)
    {
        if (!kThread->IsLocal()) {
            if (kThread->GetStatus() == eTS_Idle)
                kThread->Resume();
            continue;
        }
        CLocalThread* pkLocal = static_cast<CLocalThread*>(kThread.get());
        pkLocal->Execute();
    }
}

void CEventSystem::shutdown(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents)
{
    for (auto& kThread : m_kThreads) {
        kThread->End();
        if (kThread->IsLocal()) {
            continue;
        }
        while (kThread->GetStatus() != eTS_End) {
#if _WIN32
            Sleep(1000);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
        }
    }
}

bool CEventSystem::RegisterDelegate(unsigned int _eWorkerType, std::shared_ptr<IFunctionInterface> _spkFunction)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return false;

    return pkWorker->RegisterDelegate(_spkFunction);
}

void CEventSystem::UnregisterDelegate(unsigned int _eWorkerType, std::shared_ptr<IFunctionInterface> _spkFunction)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return;

    pkWorker->UnregisterDelegate(_spkFunction);
}

void CEventSystem::UnregisterDelegate(unsigned int _eWorkerType, const char* _pcFunctionName)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return;

    pkWorker->UnregisterDelegate(_pcFunctionName);
}

void CEventSystem::UnregisterDelegate(unsigned int _eWorkerType, void* _pInstnace, const char* _pcFunctionName)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return;

    pkWorker->UnregisterDelegate(_pInstnace, _pcFunctionName);
}

bool CEventSystem::IsExist(unsigned int _eWorkerType, const char* _pcFunctionName)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return false;
    return pkWorker->IsExist(_pcFunctionName);
}

bool CEventSystem::RegisterDelegateByPriority(unsigned int _eWorkerType, unsigned short _nPriority, std::shared_ptr<IFunctionInterface> _spkFunction)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return false;
    return pkWorker->RegisterDelegateByPriority(_nPriority, _spkFunction);
}

bool CEventSystem::UnregisterDelegateByPriority(unsigned int _eWorkerType, unsigned short _nPriority, std::shared_ptr<IFunctionInterface> _spkFunction)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return false;
    return pkWorker->UnregisterDelegateByPriority(_nPriority, _spkFunction);
}

bool CEventSystem::UnregisterDelegateByPriority(unsigned int _eWorkerType, unsigned short _nPriority, const char* _cpFunctionName)
{
    CEventWorker* pkWorker = GetWorker(_eWorkerType);
    if (!pkWorker)
        return false;
    return pkWorker->UnregisterDelegateByPriority(_nPriority, _cpFunctionName);
}