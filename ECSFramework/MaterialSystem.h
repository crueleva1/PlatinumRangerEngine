#pragma once

#include "IGeometryComponent.h"
#include "ILightComponent.h"

struct IMaterial;
struct ITexture;
struct ITextureDesc;

enum EMaterialContextType
{
    eMCT_None,
    eMCT_CreateComponent,
    eMCT_CreateInstance,
    eMCT_CreateComponentFromFile,
    eMCT_CreateInstanceFromFile,
    eMCT_GetInstance,
};

class ECS_API IMaterialContextArg
{
public:
    virtual ~IMaterialContextArg()
    {}

    virtual int GetEventType() const = 0;

    virtual void SetShaderFile(const char* _pcShaderSource, int _eShaderType){}

    virtual void SetShaderName(const char* _pcShaderName, int _eShaderType){}

    virtual void SetShaderSource(const char* _pcShaderSource, int _eShaderType) {}

    virtual IMaterial* GetMaterialInstance() const = 0;
};

class ECS_API IMatertialSystem 
    : public entityx::System <IMatertialSystem>
    , public entityx::Receiver <IMatertialSystem>
{
    IMatertialSystem* m_pkThis;
public:
    IMatertialSystem()
    {
        m_pkThis = this;
    }

    virtual ~IMatertialSystem()
    {
    }

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
    {
        ValidateMaterials(_rkEntityMgr, _rkEventMgr);
    }

    virtual void ValidateMaterials(entityx::EntityManager& _rkEnitityMgr, entityx::EventManager& _rkEventMgr) = 0;

    virtual int GetDescHash(IGeometryHandler& _rkGeometry, ILightHandler& _rkLight) = 0;

    virtual IMaterial* CreateMaterialInstance(size_t _nHash) = 0;

    virtual IMaterial* GetMaterialInstance(size_t _nHash) = 0;

    virtual std::shared_ptr <ITexture> CreateRenderData(ITextureDesc* _pkDesc, bool _bEmpty = false) = 0;

    virtual std::shared_ptr <ITexture> GetRenderData(ITextureDesc* _pkDesc) = 0;

    virtual void RemoveRenderData (ITextureDesc* _pkDesc) = 0;

    virtual void receive(const IMaterialContextArg&) = 0;
};