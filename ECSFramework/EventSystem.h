#pragma once

#include "Thread.h"

enum EEventDrivenType
{
    eED_SyncOnly,
    eED_Async,
};

enum EEventWorker
{
    eEW_File = 1,
    eEW_Event = 2,
};

class CEventWorker;
class ECS_API CEventSystem 
    : public TSingleton<CEventSystem>
    , public entityx::System<CEventSystem>
{
    std::vector<std::shared_ptr<IThread> > m_kThreads;
    EEventDrivenType m_eDrivenType;
    unsigned int m_nWorkerCount;
protected:
    CEventWorker* GetWorker(unsigned int _eWorker);
public:
    CEventSystem(unsigned int _nWorkerCount, EEventDrivenType _eType);

    virtual ~CEventSystem();

    virtual void configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents);

    virtual void update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta);

    virtual void shutdown(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents);

    virtual bool RegisterDelegate(unsigned int _eWorkerType, std::shared_ptr<IFunctionInterface> _spkFunction);

    virtual void UnregisterDelegate(unsigned int _eWorkerType, std::shared_ptr<IFunctionInterface> _spkFunction);

    virtual void UnregisterDelegate(unsigned int _eWorkerType, const char* _pcFunctionName);

    virtual void UnregisterDelegate(unsigned int _eWorkerType, void* _pInstnace, const char* _pcFunctionName);

    virtual bool IsExist(unsigned int _eWorkerType, const char* _pcFunctionName);

    virtual bool RegisterDelegateByPriority(unsigned int _eWorkerType, unsigned short _nPriority, std::shared_ptr<IFunctionInterface> _spkFunction);

    virtual bool UnregisterDelegateByPriority(unsigned int _eWorkerType, unsigned short _nPriority, std::shared_ptr<IFunctionInterface> _spkFunction);

    virtual bool UnregisterDelegateByPriority(unsigned int _eWorkerType, unsigned short _nPriority, const char* _cpFunctionName);

    template <class T, typename F>
    std::shared_ptr <IFunctionInterface> RegisterEvent(unsigned int _eWorkerType, T* pObject, F Callback, const char* _pcCallbackName)
    {
        std::shared_ptr <IFunctionInterface> pkFunction = std::make_shared<TObjectFunctionCallback<T, bool, IThread*> >(pObject, Callback, _pcCallbackName);
        if (RegisterDelegate(_eWorkerType, pkFunction))
            return pkFunction;
        return nullptr;
    }
};