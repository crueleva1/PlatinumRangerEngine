#include "MainPCH.h"
#include "PhysicalBasedRenderPipeline.h"
#include "GLSLMaterialSystem.h"
#include "PBRMaterialComponent.h"
#include "DefaultLightComponent.h"
#include "GLMSceneComponent.h"
#include "IRenderComponent.h"
#include "GLMMainCameraComponent.h"
#include "GLMCubeCameraComponent.h"
#include "IGeometryComponent.h"
#include "DefaultLightComponent.h"
#include "MeshData.h"
#include "GLMBoxBound.h"
#include "ITexture.h"
#include "Win32InputSystem.h"

CPhysicalBasedRenderPipeline::CPhysicalBasedRenderPipeline(entityx::EventManager& _rkEvent, int _nWidth, int _nHeight)
    : CDeferredRenderPipeline(_rkEvent, _nWidth, _nHeight)
    , m_nPipelineSwitch(eSpecularPipeline_Cook | eCookPipeline_Beckann)
    , m_fRimLighting(1.0f)
    , m_fRimLightingStep(0.0f)
{
    m_nSpecBit = GetBitPos <3>(m_nPipelineSwitch);
    m_nCookBit = GetBitPos <3, 3>(m_nPipelineSwitch);
}

CPhysicalBasedRenderPipeline::~CPhysicalBasedRenderPipeline()
{
}

void CPhysicalBasedRenderPipeline::Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    if (IsInited())
        return;
    CDeferredRenderPipeline::Prepared(_nWidth, _nHeight, _rkEntityMgr, _rkEventMgr);
    // we extend MRT for Metallic & Roughness
    m_spkGBuffer->SetRenderBufferColorAttactmentCount(4);
    std::shared_ptr <ITexture> spkTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    spkTexture->SetTextureFormat(gli::FORMAT_RG8_UNORM);
    spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    spkTexture->SetFilterMode(eFT_Min, eFM_Linear);
    spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    spkTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    spkTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
    m_spkGBuffer->SetTexture(spkTexture, 3);
}

void CPhysicalBasedRenderPipeline::InitMaterial()
{
    CGLSLMaterialCreateInstanceFromFile kArgs;
    std::string kFileName = "data/PBRGBufferShader.vs";
    kArgs.SetShaderName("PBRGBufferVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/PBRGBufferShader.fs";
    kArgs.SetShaderName("PBRGBufferPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkGBufferMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/PhysicalBaseRenderingShader.vs";
    kArgs.SetShaderName("PhysicalBaseRenderingVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/PhysicalBaseRenderingShader.fs";
    kArgs.SetShaderName("PhysicalBaseRenderingPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkDeferredShadingMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/BasicTexturedShader.vs";
    kArgs.SetShaderName("BasicTexturedVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/BasicTexturedShader.fs";
    kArgs.SetShaderName("BasicTexturedPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkBasicTexturedMaterial = kArgs.GetMaterialInstance();


    kFileName = "data/BasicShader.vs";
    kArgs.SetShaderName("BasicVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/LightBody.fs";
    kArgs.SetShaderName("LightBodyPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkLightBodyMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/ScreenSpaceShader.vs";
    kArgs.SetShaderName("ScreenSpaceVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/ScreenSpaceShader.fs";
    kArgs.SetShaderName("ScreenSpacePixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkScreenSpaceMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/ScreenSpaceShader.vs";
    kArgs.SetShaderName("ScreenSpaceVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/BloomShader.fs";
    kArgs.SetShaderName("BloomPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkBloomMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/ScreenSpaceShader.vs";
    kArgs.SetShaderName("ScreenSpaceVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/ContrastShader.fs";
    kArgs.SetShaderName("ContrastPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkContrastMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/WriteLinearDepth.vs";
    kArgs.SetShaderName("WriteLinearDepthVS", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/WriteLinearDepth.fs";
    kArgs.SetShaderName("WriteLinearDepthPS", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkWriteDepthMaterial = kArgs.GetMaterialInstance();
}

bool CPhysicalBasedRenderPipeline::operator()(const TBinTreeNode<3>& _rkSceneNode)
{
    if (!m_pkMainCamera)
        return true;

    if (!_rkSceneNode.IsEnd()) {
        const TPoint <3>& rkBlockCenter = _rkSceneNode.GetBlockCenter();
        const TPoint <3>& rkLength = _rkSceneNode.GetLength();
        glm::vec3 kCenter(rkBlockCenter [0], rkBlockCenter [1], rkBlockCenter [2]);
        glm::vec3 kHalf(rkLength [0] / 2, rkLength [1] / 2, rkLength [2] / 2);
        bool bRet = m_pkMainCamera->IsInView(kCenter, kHalf);
        if (ms_bDrawBinTreeBox && bRet) {
            AddRenderBoundBoxList(kCenter, kHalf, glm::vec4(1.0f, 0.8f, 0.2f, 1.0));
        }
        return bRet;
    }

    const TBinTreeEndNode<std::shared_ptr<ITreeContext>, 3>& rkEndNode = dynamic_cast <const TBinTreeEndNode<std::shared_ptr<ITreeContext>, 3>&>(_rkSceneNode);
    if (ms_bDrawBinTreeBox) {
        const TPoint <3>& rkBlockCenter = rkEndNode.GetBlockCenter();
        const TPoint <3>& rkLength = _rkSceneNode.GetLength();
        glm::vec3 kCenter(rkBlockCenter [0], rkBlockCenter [1], rkBlockCenter [2]);
        glm::vec3 kHalf(rkLength [0] / 2, rkLength [1] / 2, rkLength [2] / 2);
        AddRenderBoundBoxList(kCenter, kHalf, glm::vec4(1.0f, 0.2f, 0.2f, 1.0));
    }
    bool bRet = false;
    const std::set <std::shared_ptr <ITreeContext> >& rkDatas = rkEndNode.GetDatas();
    auto kIt = rkDatas.cbegin();
    for (; kIt != rkDatas.cend(); kIt++) {
        const CSceneContext* pkSceneObject = dynamic_cast <const CSceneContext*>((*kIt).get());
        if (!pkSceneObject)
            return true;

        if (pkSceneObject->IsDataQueried(m_nFrameRevision))
            continue;

        ISceneComponent* pkSceneComp = pkSceneObject->GetSceneComponent();
        if (!pkSceneComp)
            return true;

        CGLMSceneComponent* pkGLMSceneComp = dynamic_cast <CGLMSceneComponent*>(pkSceneComp);
        if (!pkGLMSceneComp)
            return true;

        entityx::Entity& kEntity = pkGLMSceneComp->GetEntity();
        bool bIsLight = kEntity.has_component<ILightHandler>();
        if (!kEntity.has_component<IGeometryHandler>() && !kEntity.has_component<IRenderHandler>()) {
            // Make cube Env Map
            if (!bIsLight && kEntity.has_component <ICameraHandler>()) {
                CGLMCubeCameraComponent* pkCubeCamera = dynamic_cast <CGLMCubeCameraComponent*>(kEntity.component <ICameraHandler>()->get());
                if (pkCubeCamera && pkCubeCamera->IsCameraActive()) {
                    m_kEnvCubeEntity.emplace_back();
                    SCubeMapContext& rkContext = *m_kEnvCubeEntity.rbegin();
                    rkContext.m_kEntity = kEntity;
                }
            }
            return true;
        }

        IRenderHandler& rkRendnerHandler = *(kEntity.component<IRenderHandler>().get());
        if (!rkRendnerHandler->IsRenderable())
            return true;

        if (bIsLight) {
            ILightHandler& rkLightHandler = *(kEntity.component<ILightHandler>().get());
            ELightType eType = (ELightType)rkLightHandler->GetLightType();
            switch (eType) {
            case eLT_Point:
                break;
            case eLT_Direct:
                return true;
            case eLT_Spot:
                return true;
            default:
                return true;
            }
        }

        IGeometryHandler& rkGeom = *(kEntity.component<IGeometryHandler>().get());
        const size_t nGeomSize = rkGeom->GetGeometryCount();
        bool bIsTranspancy = rkGeom->GetTextureDescByName("uOpacity");
        for (size_t nIndex = 0; nIndex < nGeomSize; nIndex++) {
            rkGeom->ActiveGeometry(nIndex);
            IBound* pkBound = rkGeom->GetBound();
#if 0
            if (m_pkMainCamera->IsInView(pkBound, m_pkMainCamera->GetCameraNameHash())) {
#else
            if (m_pkMainCamera->IsInView(pkBound)) {
#endif
                pkSceneObject->SetDataQueryed(m_nFrameRevision);
                if (bIsLight)
                    m_kLightEntity.push_back(kEntity);

                if (bIsTranspancy) {
                    m_kTranspanecyEntity.push_back(kEntity);
                }
                else {
                    m_kDrawEntity.push_back(kEntity);
                }
#if 0   //If Frustum can't see reflect won't be effected
                // Make cube Env Map
                if (!bIsLight && kEntity.has_component <ICameraHandler>()) {
                    CGLMCubeCameraComponent* pkCubeCamera = dynamic_cast <CGLMCubeCameraComponent*>(kEntity.component <ICameraHandler>()->get());
                    if (pkCubeCamera && pkCubeCamera->IsCameraActive()) {
                        m_kEnvCubeEntity.emplace_back();
                        SCubeMapContext& rkContext = *m_kEnvCubeEntity.rbegin();
                        rkContext.m_kEntity = kEntity;
                    }
                }
#endif
                bRet |= true;
                break;
            }
            }
        }
    return bRet;
    }

bool CPhysicalBasedRenderPipeline::operator()(ISceneComponent* _pkDynamicObject)
{
    if (!_pkDynamicObject)
        return false;

    CGLMSceneComponent* pkGLMSceneComp = dynamic_cast <CGLMSceneComponent*>(_pkDynamicObject);
    if (!pkGLMSceneComp)
        return true;

    entityx::Entity& kEntity = pkGLMSceneComp->GetEntity();
    if (!kEntity.has_component<IGeometryHandler>())
        return true;

    if (!kEntity.has_component<IRenderHandler>())
        return true;

    IRenderHandler& rkRendnerHandler = *(kEntity.component<IRenderHandler>().get());
    if (!rkRendnerHandler->IsRenderable())
        return true;

    bool bIsLight = kEntity.has_component<ILightHandler>();
    if (bIsLight) {
        ILightHandler& rkLightHandler = *(kEntity.component<ILightHandler>().get());
        ELightType eType = (ELightType)rkLightHandler->GetLightType();
        switch (eType) {
        case eLT_Point:
            break;
        case eLT_Direct:
            return true;
        case eLT_Spot:
            return true;
        default:
            return true;
        }
    }

    bool bRet = false;
    IGeometryHandler& rkGeom = *(kEntity.component<IGeometryHandler>().get());
    const size_t nGeomSize = rkGeom->GetGeometryCount();
    bool bIsTranspancy = rkGeom->GetTextureDescByName("uOpacity");
    for (size_t nIndex = 0; nIndex < nGeomSize; nIndex++) {
        rkGeom->ActiveGeometry(nIndex);
        IBound* pkBound = rkGeom->GetBound();
#if 0
        if (m_pkMainCamera->IsInView(pkBound, m_pkMainCamera->GetCameraNameHash())) {
#else
        if (m_pkMainCamera->IsInView(pkBound)) {
#endif
            if (bIsLight) {
                bRet |= true;
                m_kLightEntity.push_back(kEntity);
            }

            if (bIsTranspancy) {
                bRet |= true;
                m_kTranspanecyEntity.push_back(kEntity);
            }
            else {
                bRet |= true;
                m_kDrawEntity.push_back(kEntity);
            }
            break;
        }
        }
    return bRet;
    }

void CPhysicalBasedRenderPipeline::UpdateLightUniform(IRenderHandler& _rkRenderComp,
                                                 ICameraComponent* _pkCameraComp,
                                                 ICameraComponent* _pkCubeMapCam,
                                                 ISceneComponent* _pkLightSceneComp,
                                                 ISceneComponent* _pkCameraSceneComp,
                                                 ILightComponent* _pkLight)
{
    CGLMBaseCameraComponent* pkCameraComp = dynamic_cast<CGLMBaseCameraComponent*>(_pkCameraComp);
    CGLMCubeCameraComponent* pkCubeMapCamComp = dynamic_cast<CGLMCubeCameraComponent*>(_pkCubeMapCam);
    CGLMSceneComponent* pkLightSceneComp = dynamic_cast <CGLMSceneComponent*>(_pkLightSceneComp);
    CGLMSceneComponent* pkCameraSceneComp = dynamic_cast<CGLMSceneComponent*>(_pkCameraSceneComp);
    // VertexShader Uniform
    if (pkLightSceneComp) {
        AssignConstant <glm::mat4>(_rkRenderComp, "uWorldMatrix", pkLightSceneComp->GetTransform(), eConstant_Matrix4x4);
    }
    if (pkCameraComp) {
        if (_rkRenderComp->GetConstantByName("uViewProjMatrix")) {
            glm::mat4 kViewProjection = pkCameraComp->GetProjectMatrix() * pkCameraComp->GetViewMatrix();
            AssignConstant <glm::mat4>(_rkRenderComp, "uViewProjMatrix", kViewProjection, eConstant_Matrix4x4);
        }
        else {
            AssignConstant <glm::mat4>(_rkRenderComp, "uViewMatrix", pkCameraComp->GetViewMatrix(), eConstant_Matrix4x4);
            AssignConstant <glm::mat4>(_rkRenderComp, "uProjectionMatrix", pkCameraComp->GetProjectMatrix(), eConstant_Matrix4x4);
        }
    }
    // Fragment Shader Uniform
    ITexture* pkTexture = m_spkGBuffer->GetTexture(0);
    if (pkTexture) {
        AssignConstant <ITexture*>(_rkRenderComp, "uDiffuseSampler", pkTexture, eConstant_Texture);
    }
    pkTexture = m_spkGBuffer->GetTexture(1);
    if (pkTexture) {
        AssignConstant <ITexture*>(_rkRenderComp, "uPosSampler", pkTexture, eConstant_Texture);
    }
    pkTexture = m_spkGBuffer->GetTexture(2);
    if (pkTexture) {
        AssignConstant <ITexture*>(_rkRenderComp, "uNormalSampler", pkTexture, eConstant_Texture);
    }
    pkTexture = m_spkGBuffer->GetTexture(3);
    if (pkTexture) {
        AssignConstant <ITexture*>(_rkRenderComp, "uMetallicRoughness", pkTexture, eConstant_Texture);
    }
    if (pkLightSceneComp) {
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightPos", pkLightSceneComp->GetPosition(), eConstant_floatVec3);
    }
    if (pkCameraSceneComp) {
        AssignConstant <glm::vec3>(_rkRenderComp, "uCameraPos", pkCameraSceneComp->GetPosition(), eConstant_floatVec3);
    }

    if (pkCubeMapCamComp) {
        IFrameBuffer* pkCubeFrameBuffer = pkCubeMapCamComp->GetTargetFrameBuffer();
        AssignConstant <bool>(_rkRenderComp, "uEnableCubeEnvMap", true, eConstant_bool);
        AssignConstant <ITexture*>(_rkRenderComp, "uCubeEnvMap", pkCubeFrameBuffer->GetTexture(0), eConstant_Texture);
        const glm::mat4& kTransform = pkCubeMapCamComp->GetTransform();
        const glm::vec3 kPos = kTransform [3].xyz;
        AssignConstant <glm::vec3>(_rkRenderComp, "uEnvWorldPos", kPos, eConstant_floatVec3);
    }
    else {
        AssignConstant <bool>(_rkRenderComp, "uEnableCubeEnvMap", false, eConstant_bool);
        AssignConstant <ITexture*>(_rkRenderComp, "uCubeEnvMap", nullptr, eConstant_Texture);
        glm::vec3 kPos;
        AssignConstant <glm::vec3>(_rkRenderComp, "uEnvWorldPos", kPos, eConstant_floatVec3);
    }
    if (_pkLight) {
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightDiffuse", (const glm::vec3&)_pkLight->GetDiffuseColor(), eConstant_floatVec3);
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightAmbient", (const glm::vec3&)_pkLight->GetAmbientColor(), eConstant_floatVec3);
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightSpecular", (const glm::vec3&)_pkLight->GetSpecularColor(), eConstant_floatVec3);
        AssignConstant <float>(_rkRenderComp, "uLightDimmer", _pkLight->GetLightDimmer(), eConstant_float);
        CPointLightComponent* pkPointLight = dynamic_cast <CPointLightComponent*>(_pkLight);
        if (pkPointLight) {
            AssignConstant <float>(_rkRenderComp, "uLightRange", pkPointLight->GetRadius(), eConstant_float);
        }
    }
    AssignConstant <float>(_rkRenderComp, "uLightRim", m_fRimLighting, eConstant_float);
    AssignConstant <int>(_rkRenderComp, "uPipelineSwitch", m_nPipelineSwitch, eConstant_int);
}

void CPhysicalBasedRenderPipeline::UpdatePBRParam(IRenderHandler& _rkRenderComp, IMaterialComponent* _pkMaterial)
{
    CPBRMaterialComponent* pkComp = dynamic_cast <CPBRMaterialComponent*>(_pkMaterial);
    if (!pkComp)
        return;

    const glm::vec2 kMetallicRoughness(pkComp->GetMetallic(), pkComp->GetRoughness());
    AssignConstant(_rkRenderComp, "uMetallicRoughnessParam", kMetallicRoughness, eConstant_floatVec2);
}

void CPhysicalBasedRenderPipeline::RenderGbuffer(IRenderer& _rkRenderer)
{
    CGLMBaseCameraComponent* pkCamera = nullptr;
    if (m_kCameraEntity)    // checked at base class
        pkCamera = static_cast<CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());

    if (!pkCamera)
        return;
    ISceneComponent* pkSceneComp = m_kCameraEntity.component <ISceneHandler>()->get();
    // Render non Alpha
    for (auto kEntity : m_kDrawEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        // We override Default Material
        IMaterial* pkMaterial = m_pkGBufferMaterial;
        if (!kRenderHandler->SetMaterial(pkMaterial))
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        if (!kGeometryHandler->IsCastLighting())
            continue;
        if (kEntity.has_component<ILightHandler>()) {
            continue;
        }

        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }

            if (!m_bUseBinTreeTest && !pkBound->IsTested(pkCamera->GetCameraNameHash()))
                continue;

            if (!kRenderHandler->SetAttribute(kGeometryHandler->GetVertexBuffer(), kGeometryHandler->GetIndiceBuffer()))
                continue;
            kRenderHandler->SetRenderState(kGeometryHandler->GetRenderState());

            if (!pkMaterial->ApplyCameraData(pkCamera))
                continue;

            if (kEntity.has_component <ISceneHandler>()) {
                entityx::ComponentHandle <ISceneHandler> kSceneHandler = kEntity.component <ISceneHandler>();
                CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kSceneHandler->get());
                if (pkSceneComp) {
                    pkSceneComp->UpdateMaterialConstant(pkMaterial);
                }
            }

            if (!pkMaterial->ApplyGeometryTexture(kGeometryHandler.get()))
                continue;
            const bool bHasMask = false;
            AssignConstant <bool>(kRenderHandler, "uHasMask", bHasMask, eConstant_bool);
            AssignConstant <bool>(kRenderHandler, "uHasTexture", true, eConstant_bool);
            if (kEntity.has_component <IMaterialHandler>())
                UpdatePBRParam(kRenderHandler, kEntity.component <IMaterialHandler>()->get());

            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;

            _rkRenderer.OnDraw(m_rkEventMgr, kRenderHandler);
            if (ms_bDrawObjectBound)
                AddRenderBoundBoxList(pkBound->GetWorldCenter(), pkBound->GetBoxVector());
        }
    }
    // Render Transparency
    for (auto kEntity : m_kTranspanecyEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        // We override Default Material
        IMaterial* pkMaterial = m_pkGBufferMaterial;
        if (!kRenderHandler->SetMaterial(pkMaterial))
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        if (!kGeometryHandler->IsCastLighting())
            continue;
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }

            if (!m_bUseBinTreeTest && !pkBound->IsTested(pkCamera->GetCameraNameHash()))
                continue;

            if (!kRenderHandler->SetAttribute(kGeometryHandler->GetVertexBuffer(), kGeometryHandler->GetIndiceBuffer()))
                continue;
            kRenderHandler->SetRenderState(kGeometryHandler->GetRenderState());


            if (!pkMaterial->ApplyCameraData(pkCamera))
                continue;

            if (kEntity.has_component <ISceneHandler>()) {
                entityx::ComponentHandle <ISceneHandler> kSceneHandler = kEntity.component <ISceneHandler>();
                CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kSceneHandler->get());
                if (pkSceneComp) {
                    pkSceneComp->UpdateMaterialConstant(pkMaterial);
                }
            }

            if (!pkMaterial->ApplyGeometryTexture(kGeometryHandler.get()))
                continue;

            const bool bHasMask = true;
            AssignConstant <bool>(kRenderHandler, "uHasMask", bHasMask, eConstant_bool);
            if (kEntity.has_component <IMaterialHandler>())
                UpdatePBRParam(kRenderHandler, kEntity.component <IMaterialHandler>()->get());

            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;

            _rkRenderer.OnDraw(m_rkEventMgr, kRenderHandler);
            if (ms_bDrawObjectBound)
                AddRenderBoundBoxList(pkBound->GetWorldCenter(), pkBound->GetBoxVector());
        }
    }
}

void CPhysicalBasedRenderPipeline::RenderLightBody(IRenderer& _rkRenderer)
{
    CDeferredRenderPipeline::RenderLightBody(_rkRenderer);
}

void CPhysicalBasedRenderPipeline::RenderBloom(IRenderer& _rkRenderer, ITexture* _pkTexture, bool _bVertical)
{
    CDeferredRenderPipeline::RenderBloom(_rkRenderer, _pkTexture, _bVertical);
}

void CPhysicalBasedRenderPipeline::RenderLight(IRenderer& _rkRenderer)
{
    CGLMBaseCameraComponent* pkCamera = nullptr;
    if (!m_kCameraEntity)
        return;
    if (!m_kCameraEntity.has_component <ICameraHandler>())
        return;
    if (!m_kCameraEntity.has_component <ISceneHandler>())
        return;
    pkCamera = static_cast<CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());
    ISceneHandler& rkCameraSceneComp = (*m_kCameraEntity.component <ISceneHandler>().get());
    for (auto kLightEntity : m_kLightEntity) {
        // Render Scene
        if (!kLightEntity.has_component <IGeometryHandler>()) {
            continue;
        }
        IRenderHandler& rkComp = *(kLightEntity.component <IRenderHandler>().get());
        rkComp->SetMaterial(m_pkDeferredShadingMaterial);
        IGeometryHandler& rkLightVolume = (*kLightEntity.component<IGeometryHandler>().get());
        IBound* pkBound = rkLightVolume->GetBound();
        if (!pkBound)
            continue;
        if (!m_bUseBinTreeTest && !pkBound->IsTested(pkCamera->GetCameraNameHash()))
            continue;
        ILightHandler& rkLightHandler = (*kLightEntity.component<ILightHandler>().get());
        ISceneHandler& rkSceneHandler = (*kLightEntity.component <ISceneHandler>().get());
        CGLMCubeCameraComponent* pkCubeCam = nullptr;
        SCubeMapContext* pkContext = GetNearestEnvMap(rkSceneHandler.get(), pkBound->GetRadius());
        if (pkContext) {
            pkCubeCam = static_cast <CGLMCubeCameraComponent*>(pkContext->m_kEntity.component <ICameraHandler>()->get());
        }
        UpdateLightUniform(rkComp, pkCamera, pkCubeCam, rkSceneHandler.get(), rkCameraSceneComp.get(), rkLightHandler.get());
        rkComp->SetAttribute(rkLightVolume->GetVertexBuffer(), rkLightVolume->GetIndiceBuffer());
        m_spkRenderLightState->SetPrimitiveTopology((EPrimitiveType)rkLightVolume->GetPrimitiveType());
        rkComp->SetRenderState(m_spkRenderLightState.get());
        if (!rkComp->ValidatePipeline(_rkRenderer)) {
            continue;
        }
        _rkRenderer.OnDraw(m_rkEventMgr, rkComp);

        if (!m_bRenderLightVolume)
            continue;
        // Render Light Volume
        if (!rkComp->SetMaterial(m_pkBasicMaterial))
            continue;
        if (!m_pkBasicMaterial->ApplyCameraData(pkCamera))
            continue;
        CGLMSceneComponent* pkLightSceneComp = dynamic_cast<CGLMSceneComponent*>(rkSceneHandler.get());
        if (pkLightSceneComp)
            AssignConstant <glm::mat4>(rkComp, "uWorldMatrix", pkLightSceneComp->GetTransform(), eConstant_Matrix4x4);
        m_spkRenderLightVolumeState->SetPrimitiveTopology(ePrimitive_LineStrip);
        rkComp->SetRenderState(m_spkRenderLightVolumeState.get());
        if (!rkComp->ValidatePipeline(_rkRenderer))
            continue;
        _rkRenderer.OnDraw(m_rkEventMgr, rkComp);
    }
}

void CPhysicalBasedRenderPipeline::RenderContrast(IRenderer& _rkRenderer)
{
    CDeferredRenderPipeline::RenderContrast(_rkRenderer);
}

void CPhysicalBasedRenderPipeline::RenderCubeMap(IRenderer& _rkRenderer, int _nPass)
{
    if (m_kEnvCubeEntity.empty())
        return;

    IMaterial* pkMaterial = m_pkBasicTexturedMaterial;
    if (!m_kTemplateRenderComp->SetMaterial(pkMaterial))
        return;
    if (!m_kTemplateRenderComp->SetRenderState(m_spkDefaultState.get()))
        return;

    SCubeMapContext& rkContext = m_kEnvCubeEntity [_nPass];
    if (!rkContext.m_bHasRefObject)
        return;
    CGLMCubeCameraComponent* pkCam = static_cast <CGLMCubeCameraComponent*>(rkContext.m_kEntity.component <ICameraHandler>()->get());
    ISceneComponent* pkCamSceneComp = rkContext.m_kEntity.component <ISceneHandler>()->get();
    const char nTotalFace = 6;
    for (char nFace = 0; nFace < nTotalFace; ++nFace) {
        pkCam->SetCurrentFace(nFace);
        // this function will handle cube map framebuffer clear
        if (!pkCam->ActiveCurrentFace())
            return;
        if (!pkMaterial->ApplyCameraData(pkCam))
            continue;
        for (auto kDrawEntity : rkContext.m_kTestedObject [nFace]) {
            if (kDrawEntity.has_component <ISceneHandler>()) {
                entityx::ComponentHandle <ISceneHandler> kSceneHandler = kDrawEntity.component <ISceneHandler>();
                CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kSceneHandler->get());
                if (pkSceneComp) {
                    pkSceneComp->UpdateMaterialConstant(pkMaterial);
                }
            }
            IGeometryHandler& rkGeom = (*kDrawEntity.component <IGeometryHandler>().get());
            const size_t nCount = rkGeom->GetGeometryCount();
            for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
                rkGeom->ActiveGeometry(nIndex);
                IBound* pkBound = rkGeom->GetBound();
                if (!pkBound)
                    continue;
                if (!m_bUseBinTreeTest && !pkBound->IsTested(pkCam->GetCameraNameHash()))
                    continue;
                if (!m_kTemplateRenderComp->SetAttribute(rkGeom->GetVertexBuffer(), rkGeom->GetIndiceBuffer()))
                    continue;
                if (!pkMaterial->ApplyGeometryTexture(rkGeom.get()))
                    continue;
                const glm::mat4& kTrans = pkCam->GetTransform();
                AssignConstant <glm::vec3>(m_kTemplateRenderComp, "uViewWorldPos", kTrans [3].xyz, eConstant_floatVec3);
                if (!m_kTemplateRenderComp->ValidatePipeline(_rkRenderer))
                    continue;
                _rkRenderer.OnDraw(m_rkEventMgr, m_kTemplateRenderComp);
            }
        }
    }
    pkCam->RenderComplate();
}

void CPhysicalBasedRenderPipeline::RenderScreen(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha)
{
    CDeferredRenderPipeline::RenderScreen(_rkRenderer, _rkEventMgr, _pkCamera, _rkOrtha);
}

void CPhysicalBasedRenderPipeline::SetScreenText()
{
    AppendSceenText("RimLighting: %f\n", m_fRimLighting);
    AppendSceenText("SpecularPipelineType: %x\n", 1 << m_nSpecBit);
    AppendSceenText("CookPipelineType: %x\n", 1 << m_nCookBit);
    CDeferredRenderPipeline::SetScreenText();
}

CPhysicalBasedRenderPipeline::SCubeMapContext* CPhysicalBasedRenderPipeline::GetNearestEnvMap(ISceneComponent* _pkSceneComp, float _fRadius)
{
    CGLMSceneComponent* pkRefSceneComp = dynamic_cast <CGLMSceneComponent*>(_pkSceneComp);
    if (!pkRefSceneComp)
        return nullptr;
    CPhysicalBasedRenderPipeline::SCubeMapContext* pkContext = nullptr;
    float fMaxDist = std::numeric_limits<float>::max();
    for (auto& kCubeObejct : m_kEnvCubeEntity) {
        CGLMCubeCameraComponent* pkCam = static_cast <CGLMCubeCameraComponent*>(kCubeObejct.m_kEntity.component <ICameraHandler>()->get());
        CGLMSceneComponent* pkSceneComp = static_cast <CGLMSceneComponent*>(kCubeObejct.m_kEntity.component <ISceneHandler>()->get());
        float fDistance = pkSceneComp->GetDistance(pkRefSceneComp);
        float fNearDist = fDistance - _fRadius;
        if (pkCam->GetRadius() < fNearDist) {
            continue;
        }
        if (fMaxDist < fNearDist)
            continue;
        fMaxDist = fNearDist;
        pkContext = &kCubeObejct;
    }
    return pkContext;
}

void CPhysicalBasedRenderPipeline::Active(entityx::EventManager& _rkEventMgr)
{
    CDeferredRenderPipeline::Active(_rkEventMgr);
    _rkEventMgr.subscribe<IInputEvent, IInputEventListener>(*this);
}

void CPhysicalBasedRenderPipeline::Deactive(entityx::EventManager& _rkEventMgr)
{
    CDeferredRenderPipeline::Deactive(_rkEventMgr);
    _rkEventMgr.unsubscribe<IInputEvent, IInputEventListener>(*this);
}

void CPhysicalBasedRenderPipeline::receive(const IInputEvent& _rkEvent)
{
    if (_rkEvent.GetInputType() != eIET_Keyboard)
        return;
    const CKeyboardEvent& rkKeyBoard = static_cast <const CKeyboardEvent&>(_rkEvent);
    switch (rkKeyBoard.GetKey())
    {
    case CKeyboardEvent::eKBC_Z:
    {
        if (!rkKeyBoard.IsDown())
            return;
        m_nCookBit++;
        if (m_nCookBit >= 3)
            m_nCookBit = 0;
        int nInv = eCookPipeline_Blinn | eCookPipeline_Beckann | eCookPipeline_GGX;
        nInv = ~nInv;
        m_nPipelineSwitch &= nInv;
        m_nPipelineSwitch |= 1 << m_nCookBit + GetBitPos<3>(eSpecularPipeline_Cook);
        break;
    }
    case CKeyboardEvent::eKBC_X:
    {
        if (!rkKeyBoard.IsDown())
            return;
        m_nSpecBit++;
        if (m_nSpecBit >= 3)
            m_nSpecBit = 0;
        int nInv = eSpecularPipeline_Phong | eSpecularPipeline_Blinn | eSpecularPipeline_Cook;
        nInv = ~nInv;
        m_nPipelineSwitch &= nInv;
        m_nPipelineSwitch |= 1 << m_nSpecBit;
        break;
    }
    default:
        break;
    }
    
}

IFrameBuffer* CPhysicalBasedRenderPipeline::GetFrameBuffer(int _nCurrentPass)
{
    return CDeferredRenderPipeline::GetFrameBuffer(_nCurrentPass);
}

void CPhysicalBasedRenderPipeline::SetPassObject(entityx::EntityManager& _rkMgr)
{
    CModelViewPipeline::SetPassObject(_rkMgr);
    m_kLightEntity.clear();
    m_kEnvCubeEntity.clear();
    m_kDrawEntity.clear();
    m_kTranspanecyEntity.clear();
    m_kScreenEntity.clear();
    m_kCameraEntity.invalidate();
    for (entityx::Entity kEntity : _rkMgr.entities_with_components <ICameraHandler>()) {
        entityx::ComponentHandle <ICameraHandler> kHandler = kEntity.component <ICameraHandler>();
        ICameraComponent* pkIComp = kHandler->get();
        if (!pkIComp->IsCameraActive())
            continue;
        if (dynamic_cast <CGLMMainCameraComponent*>(pkIComp)) {
            m_kCameraEntity = kEntity;
            break;
        }
    }
    if (!m_bUseBinTreeTest) {
        CGLMDefaultPipeline::SetPassObject(_rkMgr);
        CGLMBaseCameraComponent* pkMainCamera = nullptr;
        if (m_kCameraEntity.valid() && m_kCameraEntity.has_component <ICameraHandler>()) {
            pkMainCamera = dynamic_cast <CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());
        }
        if (!pkMainCamera)
            return;

        ISceneComponent* pkCamSceneComp = m_kCameraEntity.component <ISceneHandler>()->get();
        // get lights
        for (auto kEntity : _rkMgr.entities_with_components <ILightHandler, ISceneHandler, IGeometryHandler>()) {
            ILightComponent* pkILight = kEntity.component <ILightHandler>()->get();
            // Just support point light
            if (pkILight->GetLightType() != eLT_Point) {
                continue;
            }
            CPointLightComponent* pkPointLight = dynamic_cast <CPointLightComponent*>(pkILight);
            if (!pkPointLight)
                continue;
            CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kEntity.component<ISceneHandler>()->get());
            if (!pkSceneComp)
                continue;
            IBound* pkBound = nullptr;
            IGeometryComponent* pkGeom = (kEntity.component<IGeometryHandler>()->get());
            // For use render light
            if (pkGeom && pkGeom->GetGeometryCount() > 0) {
                pkGeom->ActiveGeometry(0);
                pkBound = pkGeom->GetBound();
                if (!pkBound->IsTested(pkMainCamera->GetCameraNameHash()))
                    continue;
            }
            else { // no render proxy
                continue;
            }
            for (auto kDrawEntity : m_kDrawEntity) {
                CGLMSceneComponent* pkDrawSceneComp = dynamic_cast <CGLMSceneComponent*>(kDrawEntity.component <ISceneHandler>()->get());
                if (!pkDrawSceneComp)
                    continue;
                IGeometryHandler& rkGeometry = (*kDrawEntity.component <IGeometryHandler>().get());
                if (!rkGeometry->IsCastLighting())
                    continue;
                bool bTested = false;
                const float fRadius = pkPointLight->GetRadius();
                const size_t nGeomCount = rkGeometry->GetGeometryCount();
                for (size_t nIndex = 0; nIndex < nGeomCount; nIndex++) {
                    rkGeometry->ActiveGeometry(nIndex);
                    IBound* pkDrawBound = rkGeometry->GetBound();
                    float fDistance = pkDrawSceneComp->GetDistance(pkSceneComp);
                    float fNearDist = fDistance - pkDrawBound->GetRadius();
                    if (fRadius < fNearDist) {
                        continue;
                    }
                    bTested = true;
                    break;
                }
                if (!bTested)
                    continue;
                m_kLightEntity.push_back(kEntity);
                break;
            }
        }

        // get Ref Light Env Map
        for (auto kEntity : _rkMgr.entities_with_components <ICameraHandler, ISceneHandler>()) {
            if (!kEntity.has_component <ILightHandler>() && kEntity.has_component <ICameraHandler>()) {
                CGLMCubeCameraComponent* pkCubeCamera = dynamic_cast <CGLMCubeCameraComponent*>(kEntity.component <ICameraHandler>()->get());
                if (!pkCubeCamera || !pkCubeCamera->IsCameraActive()) {
                    continue;
                }
                CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kEntity.component<ISceneHandler>()->get());
                if (!pkSceneComp)
                    continue;

                for (auto kDrawEntity : m_kLightEntity) {
                    CGLMSceneComponent* pkDrawSceneComp = dynamic_cast <CGLMSceneComponent*>(kDrawEntity.component <ISceneHandler>()->get());
                    if (!pkDrawSceneComp)
                        continue;
                    IGeometryHandler& rkGeometry = (*kDrawEntity.component <IGeometryHandler>().get());
                    if (!rkGeometry->IsCastLighting())
                        continue;
                    bool bTested = false;
                    const float fRadius = pkCubeCamera->GetRadius();
                    const size_t nGeomCount = rkGeometry->GetGeometryCount();
                    for (size_t nIndex = 0; nIndex < nGeomCount; nIndex++) {
                        rkGeometry->ActiveGeometry(nIndex);
                        IBound* pkDrawBound = rkGeometry->GetBound();
                        float fDistance = pkDrawSceneComp->GetDistance(pkSceneComp);
                        float fNearDist = fDistance - pkDrawBound->GetRadius();
                        if (fRadius < fNearDist) {
                            continue;
                        }
                        bTested = true;
                        break;
                    }
                    if (!bTested)
                        continue;
                    m_kEnvCubeEntity.emplace_back();
                    SCubeMapContext& rkContext = *m_kEnvCubeEntity.rbegin();
                    rkContext.m_kEntity = kEntity;
                    rkContext.m_bHasRefObject = true;
                    break;
                }
            }
        }

        for (auto& kCubeObejct : m_kEnvCubeEntity) {
            CGLMCubeCameraComponent* pkCam = static_cast <CGLMCubeCameraComponent*>(kCubeObejct.m_kEntity.component <ICameraHandler>()->get());
            ISceneComponent* pkSceneComp = kCubeObejct.m_kEntity.component <ISceneHandler>()->get();
            const char nTotalFace = 6;
            for (char nFace = 0; nFace < nTotalFace; ++nFace) {
                pkCam->SetCurrentFace(nFace);
                // may be can use seen object only but some shadow issue
                // for (auto kDrawEntity : m_kDrawEntity) {
                for (auto kDrawEntity : _rkMgr.entities_with_components <IRenderHandler, IGeometryHandler>()) {
                    IRenderHandler& rkRenderComp = *(kDrawEntity.component <IRenderHandler>().get());
                    if (!rkRenderComp->IsRenderable())
                        continue;
                    if (kDrawEntity.has_component <ILightHandler>())
                        continue;
                    IGeometryHandler& rkGeomHandler = (*kDrawEntity.component<IGeometryHandler>().get());
                    bool bIsInView = false;
                    const size_t nCount = rkGeomHandler->GetGeometryCount();
                    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
                        rkGeomHandler->ActiveGeometry(nIndex);
                        // We checked by base class
                        CGLMBoundBox* pkBound = static_cast <CGLMBoundBox*>(rkGeomHandler->GetBound());
                        bIsInView |= pkCam->IsInView(pkBound, pkCam->GetCameraNameHash());
                    }
                    if (bIsInView)
                        kCubeObejct.m_kTestedObject [nFace].push_back(kDrawEntity);
                }
            }
        }

        unsigned int nTotalPassCount = m_kEnvCubeEntity.size() + ePT_Max;
        SetRenderPassCount(nTotalPassCount);
        return;
    }
    m_pkMainCamera = nullptr;
    if (m_kCameraEntity.valid() && m_kCameraEntity.has_component <ICameraHandler>()) {
        m_pkMainCamera = dynamic_cast <CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());
    }
    if (!m_pkMainCamera)
        return;
    ISceneComponent* pkCamSceneComp = m_kCameraEntity.component <ISceneHandler>()->get();

    // Query Screen Space Object
    for (entityx::Entity kEntity : _rkMgr.entities_with_components <IRenderHandler, IGeometryHandler>()) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        bool bScreenSpace = false;
        if (!kEntity.has_component <ISceneHandler>()) {
            continue;
        }

        ISceneHandler& rkSceneHandler = (*kEntity.component<ISceneHandler>().get());
        CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(rkSceneHandler.get());
        if (!pkSceneComp)
            continue;

        if (!pkSceneComp->IsScreenSpace())
            continue;

        m_kScreenEntity.push_back(kEntity);
    }

    // Query Static Object
    CModelViewSceneSystem& rkSceneSystem = CModelViewSceneSystem::GetSingleton();
    rkSceneSystem.PreOrderTestOctree <CPhysicalBasedRenderPipeline>(*this);
    rkSceneSystem.TestDynamicObjects <CPhysicalBasedRenderPipeline>(*this);

    std::vector <entityx::Entity> kQueriedLightEntity;
    m_kLightEntity.swap(kQueriedLightEntity);

    for (auto kEntity : kQueriedLightEntity) {
        ILightComponent* pkILight = kEntity.component <ILightHandler>()->get();
        // Just support point light
        if (pkILight->GetLightType() != eLT_Point) {
            continue;
        }
        CPointLightComponent* pkPointLight = dynamic_cast <CPointLightComponent*>(pkILight);
        if (!pkPointLight)
            continue;
        CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kEntity.component<ISceneHandler>()->get());
        if (!pkSceneComp)
            continue;
        IBound* pkBound = nullptr;
        IGeometryComponent* pkGeom = (kEntity.component<IGeometryHandler>()->get());
        // For use render light
        if (pkGeom && pkGeom->GetGeometryCount() > 0) {
            pkGeom->ActiveGeometry(0);
            pkBound = pkGeom->GetBound();
            if (!pkSceneComp->IsStatic()) {
                if (!m_pkMainCamera->IsInView(pkBound))
                    continue;
            }
#if 0
            else if (!pkBound->IsTested(m_pkMainCamera->GetCameraNameHash())) {
                continue;
            }
#endif
        }
        else { // no render proxy
            continue;
        }
        for (auto kDrawEntity : m_kDrawEntity) {
            CGLMSceneComponent* pkDrawSceneComp = dynamic_cast <CGLMSceneComponent*>(kDrawEntity.component <ISceneHandler>()->get());
            if (!pkDrawSceneComp)
                continue;
            IGeometryHandler& rkGeometry = (*kEntity.component <IGeometryHandler>().get());
            if (!rkGeometry->IsCastLighting())
                continue;
            bool bTested = false;
            const float fRadius = pkPointLight->GetRadius();
            const size_t nGeomCount = rkGeometry->GetGeometryCount();
            for (size_t nIndex = 0; nIndex < nGeomCount; nIndex++) {
                rkGeometry->ActiveGeometry(nIndex);
                IBound* pkDrawBound = rkGeometry->GetBound();
                float fDistance = pkDrawSceneComp->GetDistance(pkSceneComp);
                float fNearDist = fDistance - pkDrawBound->GetRadius();
                if (fRadius < fNearDist) {
                    continue;
                }
                bTested = true;
                break;
            }
            if (!bTested)
                continue;
            m_kLightEntity.push_back(kEntity);
            break;
        }
    }
    //If Frustum can't see reflect won't be effected
    // get Ref Light Env Map
    for (auto kEntity : _rkMgr.entities_with_components <ICameraHandler, ISceneHandler>()) {
        if (!kEntity.has_component <ILightHandler>() && kEntity.has_component <ICameraHandler>()) {
            CGLMCubeCameraComponent* pkCubeCamera = dynamic_cast <CGLMCubeCameraComponent*>(kEntity.component <ICameraHandler>()->get());
            if (!pkCubeCamera || !pkCubeCamera->IsCameraActive()) {
                continue;
            }
            CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kEntity.component<ISceneHandler>()->get());
            if (!pkSceneComp)
                continue;

            for (auto kDrawEntity : m_kLightEntity) {
                CGLMSceneComponent* pkDrawSceneComp = dynamic_cast <CGLMSceneComponent*>(kDrawEntity.component <ISceneHandler>()->get());
                if (!pkDrawSceneComp)
                    continue;
                IGeometryHandler& rkGeometry = (*kDrawEntity.component <IGeometryHandler>().get());
                if (!rkGeometry->IsCastLighting())
                    continue;
                bool bTested = false;
                const float fRadius = pkCubeCamera->GetRadius();
                const size_t nGeomCount = rkGeometry->GetGeometryCount();
                for (size_t nIndex = 0; nIndex < nGeomCount; nIndex++) {
                    rkGeometry->ActiveGeometry(nIndex);
                    IBound* pkDrawBound = rkGeometry->GetBound();
                    float fDistance = pkDrawSceneComp->GetDistance(pkSceneComp);
                    float fNearDist = fDistance - pkDrawBound->GetRadius();
                    if (fRadius < fNearDist) {
                        continue;
                    }
                    bTested = true;
                    break;
                }
                if (!bTested)
                    continue;
                m_kEnvCubeEntity.emplace_back();
                SCubeMapContext& rkContext = *m_kEnvCubeEntity.rbegin();
                rkContext.m_kEntity = kEntity;
                rkContext.m_bHasRefObject = true;
                break;
            }
        }
    }

    unsigned short nPointLightIndex = 0;
    for (auto& kCubeObejct : m_kEnvCubeEntity) {
        CGLMCubeCameraComponent* pkCubeCamera = static_cast <CGLMCubeCameraComponent*>(kCubeObejct.m_kEntity.component <ICameraHandler>()->get());
        CGLMSceneComponent* pkSceneComp = static_cast<CGLMSceneComponent*> (kCubeObejct.m_kEntity.component <ISceneHandler>()->get());
        const char nTotalFace = 6;
        for (char nFace = 0; nFace < nTotalFace; ++nFace) {
            pkCubeCamera->SetCurrentFace(nFace);
            unsigned int nRevision = m_nFrameRevision;
            nRevision <<= 3;
            nRevision += nFace;
            nRevision <<= 16;
            nRevision += nPointLightIndex++;
            kCubeObejct.m_nRevision = nRevision;
            CModelViewSceneSystem::GetSingleton().PreOrderTestOctree <CPhysicalBasedRenderPipeline::SCubeMapContext>(kCubeObejct);
        }
    }

    unsigned int nTotalPassCount = m_kEnvCubeEntity.size() + ePT_Max;
    SetRenderPassCount(nTotalPassCount);
}

void CPhysicalBasedRenderPipeline::OnClear(IRenderer& _rkRenderer, int _nCurrentPass)
{
    CDeferredRenderPipeline::OnClear(_rkRenderer, _nCurrentPass);
}

void CPhysicalBasedRenderPipeline::PassBegin(IRenderer& _rkRenderer, int _nCurrent)
{
    CDeferredRenderPipeline::PassBegin(_rkRenderer, _nCurrent);
}

void CPhysicalBasedRenderPipeline::OnDraw(IRenderer& _rkRenderer, entityx::EventManager& rkEventMgr, int _nCurrent)
{
    CDeferredRenderPipeline::OnDraw(_rkRenderer, rkEventMgr, _nCurrent);
}

void CPhysicalBasedRenderPipeline::PassEnd(IRenderer& _rkRenderer, int _nCurrent)
{
    CDeferredRenderPipeline::PassEnd(_rkRenderer, _nCurrent);
}

void CPhysicalBasedRenderPipeline::OnResize(IRenderer& _rkRenderer, int _nWidth, int _nHeight)
{
    CDeferredRenderPipeline::OnResize(_rkRenderer, _nWidth, _nHeight);
}