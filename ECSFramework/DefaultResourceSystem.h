#pragma once

#include "ResourceSystem.h"
#include "IResourceComponent.h"
#include "Thread.h"

class ECS_API CDefaultResourceSystem : public IResourceSystem
{
    typedef TScopeArgHelper<CDefaultResourceSystem, std::shared_ptr<IMutex> > CScopeLocker;

    std::map <size_t, std::shared_ptr <IStream> > m_kCachedStreamResource;
    std::map <size_t, std::shared_ptr <IFile> > m_kCachedFileResource;

    std::queue <std::weak_ptr <IResourceComponent> > m_kAsyncLoadingQueue;
    std::queue <std::weak_ptr <IResourceComponent> > m_kAsyncSavingQueue;
    std::shared_ptr<IMutex> m_spkAsyncLoadingMutex;
    std::shared_ptr<IMutex> m_spkAsyncSavingMutex;
    std::shared_ptr<IFunctionInterface> m_spkLoadingEvent;
    std::shared_ptr<IFunctionInterface> m_spkSavingingEvent;

    bool AsyncLoading(IThread* _pkThread);

    bool AsyncSaving(IThread* _pkThread);
public:
    inline void EnterScope(std::shared_ptr<IMutex> _spkMutex)
    {
        _spkMutex->Lock();
    }

    inline void LeaveScope(std::shared_ptr<IMutex> _spkMutex)
    {
        _spkMutex->Unlock();
    }

    CDefaultResourceSystem();

    virtual ~CDefaultResourceSystem();

    virtual void configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents);

    virtual void shutdown(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents);

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt);

    virtual void validateResource(entityx::EntityManager& _rkEnitityMgr);

    virtual void OnLoadingStream(IResourceStreamComponent& _rkStream, bool _bAsync = false);

    virtual void OnSavingStream(IResourceStreamComponent& _rkStream, bool _bAsync = false);

    virtual void OnLoadingFile(IResourceFileComponent& _rkFile, bool _bAsync = false);

    virtual void OnSavingFile(IResourceFileComponent& _rkFile, bool _bAsync = false);

    virtual void OnAsyncLoadingStream(std::weak_ptr<IResourceStreamComponent>& _wpkStream);

    virtual void OnAsyncSavingStream(std::weak_ptr<IResourceStreamComponent>& _wpkStream);

    virtual void OnAsyncLoadingFile(std::weak_ptr<IResourceFileComponent>& _wpkFile);

    virtual void OnAsyncSavingFile(std::weak_ptr<IResourceFileComponent>& _wpkFile);

    std::shared_ptr <IStream> GetCachedStreamResource(size_t _nKey);

    std::shared_ptr <IFile> GetCacheFileResource(size_t _nKey);
};