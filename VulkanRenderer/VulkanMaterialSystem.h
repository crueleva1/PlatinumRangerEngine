#pragma once


#include "GLSLMaterialSystem.h"


class CVulkanMaterialSystem : public CGLSLMaterialSystem
{
    virtual std::shared_ptr <IConstantSemantic> CreateConstantSematic(glslang::TProgram* _pkProgream, int _nConstantIndex);
    virtual std::shared_ptr <IStructConstantSematic> CreateConstantStructSematic(glslang::TProgram* pkProgream, int _nConstantIndex);
public:
    CVulkanMaterialSystem();

    virtual ~CVulkanMaterialSystem();

    virtual void configure(entityx::EntityManager& EntityMgr, entityx::EventManager& _rkEvents);

    virtual IMaterial* CreateMaterialInstance(size_t _nHash);

    virtual std::shared_ptr <IShader> CreateShaderInstance(const std::string& _rkName, const std::string& _rkSource, EShaderType _eType);
};