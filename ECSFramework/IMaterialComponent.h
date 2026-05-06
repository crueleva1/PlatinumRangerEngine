#pragma once

#include "IGeometryComponent.h"
#include "ILightComponent.h"
#include "IRenderComponent.h"

class IConstantSemantic;
struct ITexture;
struct IShader;
struct IVariableAttrDesc;
struct ECS_API IMaterial
{
    virtual ~IMaterial()
    {
    }

    virtual void SetMaterialName(const char* _pcName) = 0;

    virtual const char* GetMaterialName() = 0;

    virtual bool SetShader(std::shared_ptr<IShader> _spkShader) = 0;

    virtual bool ApplyGeometryTexture(class IGeometryComponent* _pkGeomtry) = 0;

    virtual bool ApplyCameraData(class ICameraComponent*  _pkCamera) = 0;

    virtual bool SetTexture(ITexture* _pkTexture, size_t _nIndex) = 0;

    virtual void SetTextureCount(size_t _nSize) = 0;

    virtual IShader* GetShader(int _eShaderType) = 0;

    virtual ITexture* GetTexture(size_t _nIndex) = 0;

    virtual bool SetConstant(std::shared_ptr <IConstantSemantic>& _spkConstant, size_t _nIndex) = 0;

    virtual void SetConstantCount(size_t _nCount) = 0;

    virtual IConstantSemantic* GetConstant(size_t _nIndex) = 0;

    virtual size_t GetConstantCount() = 0;

    virtual bool SetConstantStruct(std::shared_ptr <IStructConstantSematic>& _spkStruct, size_t _nIndex) = 0;

    virtual void SetConstantStructCount(size_t _nCount) = 0;

    virtual IStructConstantSematic* GetConstantStruct(size_t _nIndex) = 0;

    virtual size_t GetConstantStructCount() = 0;

    virtual bool SetVariable (std::shared_ptr <IVariableAttrDesc>& _spkVariable, size_t _nIndex) = 0;

    virtual void SetVariableCount (size_t _nCount) = 0;

    virtual IVariableAttrDesc* GetVariableDesc(size_t _nIndex) = 0;

    virtual IVariableAttrDesc* GetVariableDescByName(const char* _pcName) = 0;

    virtual bool IsVariableActive(const char* _pcVarName) = 0;

    virtual const char* GetActiveVariableName(const char* _pcVarSemantic) = 0;

    virtual size_t GetVariableDescCount() = 0;

    virtual bool LinkShader(IBuffer* _pkAttr) = 0;
};


struct ECS_API IMaterialDesc
{
    virtual ~IMaterialDesc()
    {
    }

    virtual std::shared_ptr <IShader> CreateShader(int _eShaderType) = 0;

    virtual bool UpdateDesc(IGeometryHandler& _rkGeometryHandler, ILightHandler& _rkLightHandler) = 0;

    virtual bool IsRescoureReady() = 0;

};

class ECS_API IMaterialComponent
{
public:
    virtual ~IMaterialComponent()
    {

    }

    virtual void SetMaterialInstance(IMaterial* _pkMaterial) = 0;

    virtual IMaterial* GetMaterialInstance() = 0;
};

typedef TComponent <IMaterialComponent> IMaterialHandler;