#include "ECSFrameworkPCH.h"
#include "DefaultSpawnSystem.h"


CSpwanFile::CSpwanFile(const char* _pcFileName)
    : m_kFileName(_pcFileName)
{

}

CSpwanFile::~CSpwanFile()
{

}

bool CSpwanFile::Load()
{
    FILE* pkFile = nullptr;
    pkFile = fopen(m_kFileName.c_str(), "rb");
    if (!pkFile)
        return false;
    if (fseek(pkFile, 0, SEEK_END)) {
        fclose(pkFile);
        return false;
    }
    size_t nSize = ftell(pkFile);
    rewind(pkFile);
    m_kXmlData.resize(nSize);
    fread((void*)m_kXmlData.data(), nSize, 1, pkFile);
    fclose(pkFile);
    return true;
}

bool CSpwanFile::Save()
{
    return true;
}

void CDefaultSpawnSystem::InsertAttr(const rapidxml::xml_attribute<>& _rkAttr, std::map<std::string, std::string>& _rkMap)
{
    const rapidxml::xml_attribute<>* pkCurrAttr = &_rkAttr;


    while (pkCurrAttr) {
        // assigned data
        _rkMap.insert (std::make_pair (pkCurrAttr->name (), pkCurrAttr->value ()));
        pkCurrAttr = pkCurrAttr->next_attribute ();
    }
}

CDefaultSpawnSystem::CDefaultSpawnSystem(const char* _pcSparkXmlFile, const char* _pcSparkXmlPath, const char* _pcVersion)
    : m_kSparkXmlFile(_pcSparkXmlFile)
    , m_kSparkXmlPath(_pcSparkXmlPath)
    , m_kVersion(_pcVersion)
{
    m_kSparkXmlNamePath = m_kSparkXmlPath + "/" + m_kSparkXmlFile;
}

CDefaultSpawnSystem::~CDefaultSpawnSystem()
{

}

void CDefaultSpawnSystem::configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    if (_rkEntityMgr.capacity () == 0) {
        // Make a Spark
        entityx::Entity kSpark = _rkEntityMgr.create ();
        entityx::ComponentHandle <IResourceFileHandler> kHanlder = kSpark.assign <IResourceFileHandler>(std::make_shared <CDefaultResourceFileComponent>());
        IResourceFileHandler& rkResourceComp = (*kHanlder.get ());
        rkResourceComp->SetFileCallBack(this);
        rkResourceComp->SetLoad();
    }
}

void CDefaultSpawnSystem::SetResourcePath(const char* _pcPath)
{
    m_kSparkXmlPath = _pcPath;
}

void CDefaultSpawnSystem::SetResourceName(const char* _pcName)
{
    m_kSparkXmlFile = _pcName;
}

const char* CDefaultSpawnSystem::GetResourcePath()
{
    return m_kSparkXmlPath.c_str();
}

const char* CDefaultSpawnSystem::GetResourceName()
{
    return m_kSparkXmlFile.c_str();
}

const char* CDefaultSpawnSystem::GetResourcePathName()
{
    return m_kSparkXmlNamePath.c_str();
}

std::shared_ptr <IFile> CDefaultSpawnSystem::OnCreateFile(const char* _pcPathName)
{
    return std::make_shared <CSpwanFile>(_pcPathName);
}

bool CDefaultSpawnSystem::OnLoadFile(IResourceFileComponent& _rkResourceComp)
{
    CSpwanFile* pkFile = dynamic_cast <CSpwanFile*>(_rkResourceComp.GetFile());
    if (!pkFile)
        return false;

    std::string& rkXmlData = pkFile->GetXmlData ();
    CRapidXMLDocument kDoc(*this, rkXmlData.c_str (), rkXmlData.size ());
    return true;
}

bool CDefaultSpawnSystem::OnSaveFile(IResourceFileComponent& _rkResourceComp)
{
    return true;
}

bool CDefaultSpawnSystem::IsHandled()
{
    return true;
}

const char* CDefaultSpawnSystem::GetSchemaName () const
{
    return "";
}

void CDefaultSpawnSystem::OnElementStart(const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr)
{
    if (_rkElement == "SpawnSystem") {
        SpawnSystemElementStart(_rkAttr);
    }
    else if (_rkElement == "Entity") {
        EntityElementStart(_rkAttr);
    }
    else if (_rkElement == "Component") {
        ComponentElementStart(_rkAttr);
    }
}

void CDefaultSpawnSystem::OnElementEnd(const std::string& _rkElement)
{
    if (_rkElement == "SpawnSystem") {
        SpawnSystemElementEnd();
    }
    else if (_rkElement == "Entity") {
        EntityElementEnd();
    }
    else if (_rkElement == "Component") {
        ComponentElementEnd();
    }
}

void CDefaultSpawnSystem::OnText(const std::string& _rkText)
{

}

void CDefaultSpawnSystem::ComponentElementStart(const rapidxml::xml_attribute<>& _rkAttr)
{
    std::map <std::string, std::string> kAttrMap;
    CDefaultSpawnSystem::InsertAttr(_rkAttr, kAttrMap);

    if (kAttrMap.empty())
        return;

    std::shared_ptr <ISpawnDesc> spkDesc = m_kCurrentDesc.top();

    CDefaultSpawnDesc* pkDesc = dynamic_cast <CDefaultSpawnDesc*>(spkDesc.get());
    assert(pkDesc);

    std::string& kCompType = kAttrMap ["Type"];
    assert(!kCompType.empty());
    IComponentDesc& kCompDesc = pkDesc->NewComponentDesc<SComponentDesc>();
    kCompDesc.SetAttrTag(kCompType.c_str());
    for (auto kIt : kAttrMap) {
        if (kIt.first == "Type") {
            continue;
        }
        kCompDesc.SetProperty(kIt.first.c_str(), kIt.second.c_str());
    }
}

void CDefaultSpawnSystem::ComponentElementEnd()
{

}

void CDefaultSpawnSystem::EntityElementStart(const rapidxml::xml_attribute<>& _rkAttr)
{
    IComponentDesc* pkCompDesc = nullptr;
    if (!m_kCurrentDesc.empty()) {
        CDefaultSpawnDesc* pkDesc = dynamic_cast <CDefaultSpawnDesc*>(m_kCurrentDesc.top().get());
        assert(pkDesc);
        const size_t nCount = pkDesc->GetComponentsCount();
        for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
            pkCompDesc = &pkDesc->GetComponent(nIndex);
            if (!pkCompDesc->IsAttrTagMatch("Scene"))
                continue;
            // Set Parent?

        }
    }

    std::shared_ptr <ISpawnDesc> spkDesc = std::make_shared<CDefaultSpawnDesc>();
    m_kCurrentDesc.push(spkDesc);
}

void CDefaultSpawnSystem::EntityElementEnd()
{
    if (m_kCurrentDesc.empty())
        return;

    PushSpawnDesc(m_kCurrentDesc.top());
    m_kCurrentDesc.pop();
}

void CDefaultSpawnSystem::SpawnSystemElementStart(const rapidxml::xml_attribute<>& _rkAttr)
{
    const rapidxml::xml_attribute<>* pkCurrAttr = &_rkAttr;
    while (pkCurrAttr) {
        // assigned data
        if (!strcmp(pkCurrAttr->name(), "version")) {
            assert(!strcmp(pkCurrAttr->value(), m_kVersion.c_str()));
            break;
        }
        pkCurrAttr = pkCurrAttr->next_attribute();
    }
}

void CDefaultSpawnSystem::SpawnSystemElementEnd()
{

}

void CDefaultSpawnSystem::ResolveDesc(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, ISpawnDesc* _pkDesc)
{
    entityx::Entity kEntity = _rkEntityMgr.create();
    for (size_t nIndex = 0; nIndex < _pkDesc->GetComponentsCount(); nIndex++) {
        IComponentDesc& rkCompDesc = _pkDesc->GetComponent(nIndex);
        AssignComp(_rkEventMgr, kEntity, rkCompDesc);
    }
}