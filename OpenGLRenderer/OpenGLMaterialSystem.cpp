#include "ECSOpenGLPCH.h"
#include "OpenGLMaterialSystem.h"
#include "OpenGLMaterial.h"
#include "DefaultMaterialComponent.h"
#include "RenderSystem.h"

class ECS_OGL_API COpenGLShaderFactory : public IGLSLShaderFactory
{
public:
    COpenGLShaderFactory(EShaderType _eType)
        : IGLSLShaderFactory(_eType)
    {
    }

    virtual std::shared_ptr <CGLSLShader> operator()()
    {
        return std::make_shared <COpenGLShader>(GetType());
    }
};

class ECS_OGL_API COpenGLMaterialFactory : public IGLSLMaterialFactory
{
public:
    COpenGLMaterialFactory(std::string _kType)
        : IGLSLMaterialFactory(_kType)
    {
    }

    virtual std::shared_ptr <CGLSLMaterial> operator()(class CGLSLMaterialSystem& _rkSystem)
    {
        return std::make_shared <COpenGLMaterial>(_rkSystem);
    }
};

class ECS_OGL_API COpenGLMaterialCompFactory : public IMaterialCompFactory
{
public:
    COpenGLMaterialCompFactory(std::string _kType)
        : IMaterialCompFactory(_kType)
    {
    }

    virtual std::shared_ptr <IMaterialComponent> operator()(IMaterial* _plMaterialIns)
    {
        std::shared_ptr <IMaterialComponent> spkComp = std::make_shared <CDefaultMaterialComponent>();
        spkComp->SetMaterialInstance(_plMaterialIns);
        return spkComp;
    }
};

COpenGLMaterialSystem::COpenGLMaterialSystem()
{
}

COpenGLMaterialSystem::~COpenGLMaterialSystem()
{
}

void COpenGLMaterialSystem::configure(entityx::EntityManager& EntityMgr, entityx::EventManager& _rkEvents)
{
    CGLSLMaterialSystem::configure(EntityMgr, _rkEvents);

    RegisterComponentFactory<COpenGLMaterialCompFactory>("Default");
    RegisterMaterialFactory<COpenGLMaterialFactory>(IRenderSystem::GetSystem()->GetRendererName());
    RegisterShaderFactory<COpenGLShaderFactory>(eShader_Vertex);
    RegisterShaderFactory<COpenGLShaderFactory>(eShader_Pixel);
    RegisterShaderFactory<COpenGLShaderFactory>(eShader_Geomtery);
}
