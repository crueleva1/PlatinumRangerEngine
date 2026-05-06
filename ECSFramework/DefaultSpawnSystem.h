#pragma once

#include "SpawnSystem.h"
#include "DefaultResourceComponent.h"
#include "XmlParser.h"


class ECS_API CDefaultSpawnDesc : public ISpawnDesc
{
    std::vector <std::shared_ptr<IComponentDesc> > m_kCompDesc;
public:
    CDefaultSpawnDesc()
    {

    }

    virtual ~CDefaultSpawnDesc()
    {

    }

    size_t GetComponentsCount()
    {
        return m_kCompDesc.size();
    }

    IComponentDesc& GetComponent(size_t _nIndex)
    {
        return *m_kCompDesc[_nIndex];
    }

    template <class T>
    IComponentDesc& NewComponentDesc()
    {
        m_kCompDesc.push_back(std::make_shared <T>());
        return **m_kCompDesc.rbegin();
    }
};

typedef std::shared_ptr <CDefaultSpawnDesc> CDefaultSpawnDescPtr;

class ECS_API CSpwanFile : public IFile
{
    std::string m_kXmlData;
    std::string m_kFileName;
public:
    CSpwanFile(const char* _pcFileName);

    virtual ~CSpwanFile();

    virtual bool Load();

    virtual bool Save();

    virtual const char* GetFileName()
    {
        return m_kFileName.c_str();
    }

    inline std::string& GetXmlData()
    {
        return m_kXmlData;
    }
};

class SComponentDesc : public IComponentDesc
{
    std::string m_kAttrTag;
public:
    SComponentDesc()
    {

    }

    virtual ~SComponentDesc()
    {
    }

    virtual void SetAttrTag(const char* _pcTag)
    {
        m_kAttrTag = _pcTag;
    }

    virtual const char* GetAttrTag()
    {
        return m_kAttrTag.c_str();
    }

    virtual bool IsAttrTagMatch(const char* _pcTag)
    {
        return (m_kAttrTag == _pcTag);
    }
};

class ECS_API CDefaultSpawnSystem 
    : public ISpawnSystem
    , public IFileCallBack
    , public IXMLHandler
{
    std::string m_kSparkXmlFile;
    std::string m_kSparkXmlPath;
    std::string m_kSparkXmlNamePath;
    std::string m_kVersion;

    std::stack <std::shared_ptr <ISpawnDesc> > m_kCurrentDesc;
protected:
    static void InsertAttr(const rapidxml::xml_attribute<>& _rkAttr, std::map <std::string, std::string>& _rkMap);
public:
    CDefaultSpawnSystem(const char* _pcSparkXmlFile, const char* _pcSparkXmlPath, const char* _pcVersion);

    virtual ~CDefaultSpawnSystem();

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void SetResourcePath(const char* _pcPath);

    virtual void SetResourceName(const char* _pcName);

    virtual const char* GetResourcePath();

    virtual const char* GetResourceName();

    virtual const char* GetResourcePathName();

    virtual std::shared_ptr <IFile> OnCreateFile(const char* _pcPathName);

    virtual bool OnLoadFile(IResourceFileComponent&);

    virtual bool OnSaveFile(IResourceFileComponent&);

    virtual bool IsHandled();

    virtual const char* GetSchemaName () const;

    virtual void OnElementStart (const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr);

    virtual void OnElementEnd (const std::string& _rkElement);

    virtual void OnText (const std::string& _rkText);

    virtual void ComponentElementStart(const rapidxml::xml_attribute<>& _rkAttr);

    virtual void ComponentElementEnd();

    virtual void EntityElementStart(const rapidxml::xml_attribute<>& _rkAttr);

    virtual void EntityElementEnd();

    virtual void SpawnSystemElementStart(const rapidxml::xml_attribute<>& _rkAttr);

    virtual void SpawnSystemElementEnd();

    virtual void ResolveDesc(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, ISpawnDesc* _pkDesc);

    virtual bool AssignComp(entityx::EventManager& _rkEventMgr, entityx::Entity& _rkEntity, IComponentDesc& _rkDesc) = 0;
};