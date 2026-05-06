#include "ECSFrameworkPCH.h"
#include "DefaultResourceSystem.h"
#include "DefaultResourceComponent.h"
#include "Thread.h"
#include "EventSystem.h"

bool LoadingFileToStream(std::shared_ptr <IStream>& _rspkStream, const char* _pcFileName)
{
    if (!_rspkStream)
        return false;
    if (!_pcFileName)
        return false;

    FILE* pkFile = NULL;
    pkFile = fopen(_pcFileName, "rb");
    if (!pkFile)
        return false;
    fseek(pkFile, 0, SEEK_END);
    const size_t nSize = ftell(pkFile);
    _rspkStream->SetSize(nSize);
    if (!nSize) {
        fclose(pkFile);
        return true;
    }
    char* pcData = _rspkStream->GetData();
    fseek(pkFile, 0, SEEK_SET);
    fread(pcData, nSize, 1, pkFile);
    fclose(pkFile);
    return true;
}

bool SavingStreamToFile(std::shared_ptr <IStream>& _rspkStream, const char* _pcFileName)
{
    if (!_rspkStream)
        return false;
    if (!_pcFileName)
        return false;

    FILE* pkFile = NULL;
    pkFile = fopen(_pcFileName, "wb");
    if (!pkFile)
        return false;
    const size_t nSize = _rspkStream->GetDataSize();
    char* cpData = _rspkStream->GetData();
    fwrite(cpData, nSize, 1, pkFile);
    fclose(pkFile);
    return true;
}

CDefaultResourceSystem::CDefaultResourceSystem()
{
    
}

CDefaultResourceSystem::~CDefaultResourceSystem()
{

}

bool CDefaultResourceSystem::AsyncLoading(IThread* _pkThread)
{
    if (m_kAsyncLoadingQueue.empty())
        return false;

    while (!m_kAsyncLoadingQueue.empty()) {
        auto wpResource = m_kAsyncLoadingQueue.front();
        {
            CScopeLocker kLock(*this, m_spkAsyncLoadingMutex);
            m_kAsyncLoadingQueue.pop();
        }
        if (wpResource.expired())
            continue;
        auto spResource = wpResource.lock();
        switch (spResource->GetType())
        {
        case eRIO_File:
            OnLoadingFile(*std::static_pointer_cast<IResourceFileComponent>(spResource));
            break;
        case eRIO_Stream:
            OnLoadingStream(*std::static_pointer_cast<IResourceStreamComponent>(spResource));
            break;
        default:
            break;
        }
        OnSetStateus(spResource.get(), CDefaultResourceComponent::eStatus_AsyncLoadingEnd);
    }
    return false;
}

bool CDefaultResourceSystem::AsyncSaving(IThread* _pkThread)
{
    if (m_kAsyncSavingQueue.empty())
        return false;

    while (!m_kAsyncSavingQueue.empty()) {
        auto wpResource = m_kAsyncSavingQueue.front();
        {
            CScopeLocker kLock(*this, m_spkAsyncSavingMutex);
            m_kAsyncSavingQueue.pop();
        }
        if (wpResource.expired())
            continue;
        auto spResource = wpResource.lock();
        switch (spResource->GetType()) {
        case eRIO_File:
            OnSavingFile(*std::static_pointer_cast<IResourceFileComponent>(spResource));
            break;
        case eRIO_Stream:
            OnSavingStream(*std::static_pointer_cast<IResourceStreamComponent>(spResource));
            break;
        default:
            break;
        }
    }
    return false;
}

void CDefaultResourceSystem::configure(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents)
{
    IThreadMutexFactory* pkFactory = CThreadMutexFactoryMgr::GetInstance().GetFactoryByType<IThreadMutexFactory>("POSIXMutex");
    if (pkFactory) {
        m_spkAsyncLoadingMutex = (*pkFactory)();
        m_spkAsyncSavingMutex = (*pkFactory)();
    }

    m_spkLoadingEvent = CEventSystem::GetInstance().RegisterEvent(eEW_File, RegisterObjectFunctionPtr(this, CDefaultResourceSystem::AsyncLoading));
    m_spkSavingingEvent = CEventSystem::GetInstance().RegisterEvent(eEW_File, RegisterObjectFunctionPtr(this, CDefaultResourceSystem::AsyncSaving));
}

void CDefaultResourceSystem::shutdown(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents)
{
    CEventSystem::GetInstance().UnregisterDelegate(eEW_File, m_spkLoadingEvent);
    CEventSystem::GetInstance().UnregisterDelegate(eEW_File, m_spkSavingingEvent);
}

void CDefaultResourceSystem::update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
{
    IResourceSystem::update(_rkEntityMgr, _rkEventMgr, dt);
}

void CDefaultResourceSystem::validateResource(entityx::EntityManager& _rkEnitityMgr)
{
    // Direct Access File
    _rkEnitityMgr.each <IResourceStreamHandler>([this] (entityx::Entity kEntity, IResourceStreamHandler& _kComp)
    {
        CDefaultResourceComponent::EStatus eStatus = (CDefaultResourceComponent::EStatus)_kComp->GetStatus();
        switch (eStatus) {
        case CDefaultResourceComponent::eStatus_Error:
            return;
        case CDefaultResourceComponent::eStatus_Idle:
            return;
        case CDefaultResourceComponent::eStatus_Loading:
            OnLoadingStream(*_kComp.get());
            break;
        case CDefaultResourceComponent::eStatus_Saving:
            OnSavingStream(*_kComp.get());
            break;
        case CDefaultResourceComponent::eStatus_AsyncLoadingStart:
            OnAsyncLoadingStream(_kComp.MakeWeakRef());
            break;
        case CDefaultResourceComponent::eStatus_AsyncLoadingEnd:
            OnSetStateus(_kComp.get(), CDefaultResourceComponent::eStatus_Idle);
            break;
        case CDefaultResourceComponent::eStatus_AsyncSavingStart:
            OnAsyncSavingStream(_kComp.MakeWeakRef());
            break;
        case CDefaultResourceComponent::eStatus_AsyncSavingEnd:
            OnSetStateus(_kComp.get(), CDefaultResourceComponent::eStatus_Idle);
            break;
        default:
            return;
        }
    });
    // Custom Access File (like self file flow)
    _rkEnitityMgr.each <IResourceFileHandler>([this] (entityx::Entity kEntity, IResourceFileHandler& _kComp)
    {
        CDefaultResourceComponent::EStatus eStatus = (CDefaultResourceComponent::EStatus)_kComp->GetStatus();
        switch (eStatus) {
        case CDefaultResourceComponent::eStatus_Error:
            return;
            break;
        case CDefaultResourceComponent::eStatus_Idle:
            return;
            break;
        case CDefaultResourceComponent::eStatus_Loading:
            OnLoadingFile(*_kComp.get());
            break;
        case CDefaultResourceComponent::eStatus_Saving:
            OnSavingFile(*_kComp.get());
            break;
        case CDefaultResourceComponent::eStatus_AsyncLoadingStart:
            OnAsyncLoadingFile(_kComp.MakeWeakRef());
            break;
        case CDefaultResourceComponent::eStatus_AsyncLoadingEnd:
            OnSetStateus(_kComp.get(), CDefaultResourceComponent::eStatus_Idle);
            break;
        case CDefaultResourceComponent::eStatus_AsyncSavingStart:
            OnAsyncSavingFile(_kComp.MakeWeakRef());
            break;
        case CDefaultResourceComponent::eStatus_AsyncSavingEnd:
            OnSetStateus(_kComp.get(), CDefaultResourceComponent::eStatus_Idle);
            break;
        default:
            break;
        }
    });
}

void CDefaultResourceSystem::OnLoadingStream(IResourceStreamComponent& _rkComp, bool _bAsync)
{
    ISerializeStreamCallBack* pkCallBack = _rkComp.GetSerializerCallBack();
    if (!pkCallBack) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    std::string kFullPath = pkCallBack->GetResourcePathName();

    std::hash <std::string> kHash;
    size_t nKey = kHash(kFullPath);
    std::shared_ptr <IStream> spkStream = GetCachedStreamResource(nKey);
    if (!spkStream) {
        spkStream = pkCallBack->CreateStream(kFullPath.c_str());
        if (spkStream)
            m_kCachedStreamResource.insert(std::make_pair(nKey, spkStream));
    }

    if (!spkStream) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    spkStream->ResetSeek();

    if (!LoadingFileToStream(spkStream, kFullPath.c_str())) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    _rkComp.SetStream(spkStream);

    if (!pkCallBack->Deserialize(_rkComp)) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    OnSetStateus(&_rkComp, _bAsync ? CDefaultResourceComponent::eStatus_AsyncLoadingEnd : CDefaultResourceComponent::eStatus_Idle);
}

void CDefaultResourceSystem::OnSavingStream(IResourceStreamComponent& _rkComp, bool _bAsync)
{
    ISerializeStreamCallBack* pkCallBack = _rkComp.GetSerializerCallBack();
    if (!pkCallBack) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    std::string kFullPath = pkCallBack->GetResourcePathName();

    std::hash <std::string> kHash;
    size_t nKey = kHash(kFullPath);
    std::shared_ptr <IStream> spkStream = GetCachedStreamResource(nKey);
    if (!spkStream) {
        spkStream = pkCallBack->CreateStream(kFullPath.c_str());
        if (spkStream)
            m_kCachedStreamResource.insert(std::make_pair(nKey, spkStream));
    }

    if (!spkStream) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    spkStream->ResetSeek();

    _rkComp.SetStream(spkStream);

    if (!pkCallBack->Serialize(_rkComp)) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    if (!SavingStreamToFile(spkStream, kFullPath.c_str())) {
        OnSetStateus(&_rkComp, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    OnSetStateus(&_rkComp, _bAsync ? CDefaultResourceComponent::eStatus_AsyncSavingEnd : CDefaultResourceComponent::eStatus_Idle);
}

void CDefaultResourceSystem::OnLoadingFile(IResourceFileComponent& _rkFile, bool _bAsync)
{
    IFileCallBack* pkCallBack = _rkFile.GetFileCallBack();
    if (!pkCallBack) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    std::string kFullPath = pkCallBack->GetResourcePathName();

    std::hash <std::string> kHash;
    size_t nKey = kHash(kFullPath);

    std::shared_ptr <IFile> spkFile = GetCacheFileResource(nKey);
    if (!spkFile) {
        spkFile = pkCallBack->OnCreateFile(kFullPath.c_str());
        if (spkFile)
            m_kCachedFileResource.insert(std::make_pair(nKey, spkFile));
    }

    if (!spkFile) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    _rkFile.SetFile(spkFile);

    if (!spkFile->Load()) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    if (pkCallBack->OnLoadFile(_rkFile)) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    OnSetStateus(&_rkFile, (_bAsync) ? CDefaultResourceComponent::eStatus_AsyncLoadingEnd : CDefaultResourceComponent::eStatus_Idle);
}

void CDefaultResourceSystem::OnSavingFile(IResourceFileComponent& _rkFile, bool _bAsync)
{
    IFileCallBack* pkCallBack = _rkFile.GetFileCallBack();
    if (!pkCallBack) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    std::string kFullPath = pkCallBack->GetResourcePathName();

    std::hash <std::string> kHash;
    size_t nKey = kHash(kFullPath);

    std::shared_ptr <IFile> spkFile = GetCacheFileResource(nKey);
    if (!spkFile) {
        spkFile = pkCallBack->OnCreateFile(kFullPath.c_str());
        if (spkFile)
            m_kCachedFileResource.insert(std::make_pair(nKey, spkFile));
    }

    if (!spkFile) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    _rkFile.SetFile(spkFile);

    if (!pkCallBack->OnSaveFile(_rkFile)) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    if (!spkFile->Save()) {
        OnSetStateus(&_rkFile, CDefaultResourceComponent::eStatus_Error);
        return;
    }

    OnSetStateus(&_rkFile, (_bAsync) ? CDefaultResourceComponent::eStatus_AsyncSavingEnd : CDefaultResourceComponent::eStatus_Idle);
}

void CDefaultResourceSystem::OnAsyncLoadingStream(std::weak_ptr<IResourceStreamComponent>& _wpkStream)
{
    CScopeLocker kLock(*this, m_spkAsyncLoadingMutex);
    OnSetStateus(_wpkStream.lock().get(), CDefaultResourceComponent::eStatus_AsyncLoading);
    m_kAsyncLoadingQueue.push(_wpkStream);
}

void CDefaultResourceSystem::OnAsyncSavingStream(std::weak_ptr<IResourceStreamComponent>& _wpkStream)
{
    CScopeLocker kLock(*this, m_spkAsyncSavingMutex);
    OnSetStateus(_wpkStream.lock().get(), CDefaultResourceComponent::eStatus_AsyncSaving);
    m_kAsyncSavingQueue.push(_wpkStream);
}

void CDefaultResourceSystem::OnAsyncLoadingFile(std::weak_ptr<IResourceFileComponent>& _wpkFile)
{
    CScopeLocker kLock(*this, m_spkAsyncLoadingMutex);
    OnSetStateus(_wpkFile.lock().get(), CDefaultResourceComponent::eStatus_AsyncLoading);
    m_kAsyncLoadingQueue.push(_wpkFile);
}

void CDefaultResourceSystem::OnAsyncSavingFile(std::weak_ptr<IResourceFileComponent>& _wpkFile)
{
    CScopeLocker kLock(*this, m_spkAsyncSavingMutex);
    OnSetStateus(_wpkFile.lock().get(), CDefaultResourceComponent::eStatus_AsyncSaving);
    m_kAsyncSavingQueue.push(_wpkFile);
}

std::shared_ptr <IStream> CDefaultResourceSystem::GetCachedStreamResource(size_t _nKey)
{
    std::map <size_t, std::shared_ptr <IStream> >::iterator kIt = m_kCachedStreamResource.find(_nKey);
    if (kIt != m_kCachedStreamResource.end())
        return kIt->second;

    return nullptr;
}

std::shared_ptr <IFile> CDefaultResourceSystem::GetCacheFileResource(size_t _nKey)
{
    std::map <size_t, std::shared_ptr <IFile> >::iterator kIt = m_kCachedFileResource.find(_nKey);
    if (kIt != m_kCachedFileResource.end())
        return kIt->second;

    return nullptr;
}