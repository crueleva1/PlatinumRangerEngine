#pragma once


#include "DefaultSpawnSystem.h"



class ECS_MODELVIEW_API CModelViewSpawnSystem : public CDefaultSpawnSystem
{
public:
    CModelViewSpawnSystem(const char* _pcSparkXmlFile, const char* _pcSparkXmlPath, const char* _pcVersion);

    virtual ~CModelViewSpawnSystem();

    virtual void OnElementStart (const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr);

    virtual void OnElementEnd (const std::string& _rkElement);

    virtual void AssimpElementStart(const rapidxml::xml_attribute<>& _rkAttr);

    virtual void AssimpElementEnd();

    virtual void ConfigElementStart(const rapidxml::xml_attribute<>& _rkAttr);

    virtual void ConfigElementEnd();

    virtual void TextElementStart(const rapidxml::xml_attribute<>& _rkAttr);

    virtual void TextElementEnd();

    virtual bool AssignComp(entityx::EventManager& _rkEventMgr, entityx::Entity& _rkEntity, IComponentDesc& _rkDesc);
};