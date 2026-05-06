#include "MainPCH.h"
#include "DeferredRenderPipeline.h"
#include "GLSLMaterialSystem.h"
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

#define ENABLE_CUBE_SHADOW 1

float CDeferredRenderPipeline::m_fBrightness(0.0f);
float CDeferredRenderPipeline::m_fContrast(5.0f);
float CDeferredRenderPipeline::m_fBrightnessThreshold(0.5f);
float CDeferredRenderPipeline::m_fBlurWeight(0.01f);
float CDeferredRenderPipeline::m_fBrightnessStep(0.0f);
float CDeferredRenderPipeline::m_fContrastStep(0.0f);
float CDeferredRenderPipeline::m_fBrightnessThresholdStep(0.0f);
float CDeferredRenderPipeline::m_fBlurWeightStep(0.0f);
bool CDeferredRenderPipeline::m_bRenderLightVolume(false);
bool CDeferredRenderPipeline::m_bEnableBloom(true);
bool CDeferredRenderPipeline::m_bUseBinTreeTest(true);

CDeferredRenderPipeline::CDeferredRenderPipeline(entityx::EventManager& _rkEvent, int _nWidth, int _nHeight)
    : CModelViewPipeline(_rkEvent, _nWidth, _nHeight)
    , m_pkGBufferMaterial(nullptr)
    , m_pkDeferredShadingMaterial(nullptr)
    , m_pkBasicTexturedMaterial(nullptr)
    , m_pkWriteDepthMaterial(nullptr)
    , m_pkBloomMaterial(nullptr)
    , m_pkScreenSpaceMaterial(nullptr)
    , m_pkLightBodyMaterial(nullptr)
    , m_pkMainCamera(nullptr)
    , m_kTemplateLight(std::make_shared <CPointLightComponent>())
    , m_nScreenWidth(_nWidth)
    , m_nScreenHeight(_nHeight)
{
}

CDeferredRenderPipeline::~CDeferredRenderPipeline()
{

}

bool CDeferredRenderPipeline::SCubeMapContext::operator()(const TBinTreeNode<3>& _rkSceneNode)
{
    if (!m_kEntity.has_component <ICameraHandler>())
        return true;

    CGLMCubeCameraComponent* pkCam = static_cast <CGLMCubeCameraComponent*>(m_kEntity.component <ICameraHandler>()->get());
    int nFace = pkCam->GetCurrentFace();

    if (!_rkSceneNode.IsEnd ()) {
        const TPoint <3>& rkBlockCenter = _rkSceneNode.GetBlockCenter ();
        const TPoint <3>& rkLength = _rkSceneNode.GetLength ();
        glm::vec3 kCenter (rkBlockCenter [0], rkBlockCenter [1], rkBlockCenter [2]);
        glm::vec3 kHalf (rkLength [0] / 2, rkLength [1] / 2, rkLength [2] / 2);
        return pkCam->IsInView(kCenter, kHalf);
    }

    const TBinTreeEndNode<std::shared_ptr<ITreeContext>, 3>& rkEndNode = dynamic_cast <const TBinTreeEndNode<std::shared_ptr<ITreeContext>, 3>&>(_rkSceneNode);

    const std::set <std::shared_ptr <ITreeContext> >& rkDatas = rkEndNode.GetDatas();
    auto kIt = rkDatas.cbegin();
    bool bRet = false;
    for (; kIt != rkDatas.cend(); kIt++) {
        const CSceneContext* pkSceneObject = dynamic_cast <const CSceneContext*>((*kIt).get());
        if (!pkSceneObject)
            return true;

        if (pkSceneObject->IsDataQueried(m_nRevision))
            continue;
        
        ISceneComponent* pkSceneCamComp = m_kEntity.component <ISceneHandler>()->get();
        ISceneComponent* pkSceneComp = pkSceneObject->GetSceneComponent();
        if (!pkSceneComp)
            return true;

        CGLMSceneComponent* pkGLMSceneComp = dynamic_cast <CGLMSceneComponent*>(pkSceneComp);
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

        if (kEntity.has_component<ILightHandler>()) {
            return true;
        }
        
        IGeometryHandler& rkGeom = *(kEntity.component<IGeometryHandler>().get());
        const size_t nGeomSize = rkGeom->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nGeomSize; nIndex++) {
            rkGeom->ActiveGeometry(nIndex);
            IBound* pkBound = rkGeom->GetBound();
#if 0
            if (pkCam->IsInView(pkBound, pkCam->GetCameraNameHash())) {
#else
            if (pkCam->IsInView(pkBound)) {
#endif
                bRet |= true;
                pkSceneObject->SetDataQueryed(m_nRevision);
                m_kTestedObject[nFace].push_back(kEntity);
                break;
            }
        }
    }
    return bRet;
}

bool CDeferredRenderPipeline::operator()(const TBinTreeNode<3>& _rkSceneNode)
{
    if (!m_pkMainCamera)
        return true;

    if (!_rkSceneNode.IsEnd()) {
        const TPoint <3>& rkBlockCenter = _rkSceneNode.GetBlockCenter();
        const TPoint <3>& rkLength = _rkSceneNode.GetLength();
        glm::vec3 kCenter(rkBlockCenter[0], rkBlockCenter[1], rkBlockCenter[2]);
        glm::vec3 kHalf(rkLength[0] / 2, rkLength[1] / 2, rkLength[2] / 2);
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
#if 0
                if (ms_bDrawBinTreeBox && !bRet) {
                    const TPoint <3>& rkBlockCenter = _rkSceneNode.GetBlockCenter();
                    const TPoint <3>& rkLength = _rkSceneNode.GetLength();
                    glm::vec3 kCenter(rkBlockCenter [0], rkBlockCenter [1], rkBlockCenter [2]);
                    glm::vec3 kHalf(rkLength [0] / 2, rkLength [1] / 2, rkLength [2] / 2);
                    AddRenderBoundBoxList(kCenter, kHalf);
                }
#endif
                bRet |= true;
                break;
            }
        }
    }
    return bRet;
}

bool CDeferredRenderPipeline::operator()(ISceneComponent* _pkDynamicObject)
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

void CDeferredRenderPipeline::Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    if (IsInited())
        return;
    CModelViewPipeline::Prepared(_nWidth, _nHeight, _rkEntityMgr, _rkEventMgr);
    SetRenderPassCount(ePT_Max);
    m_spkGBuffer = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth, _nHeight);
    m_spkGBuffer->SetRenderBufferColorAttactmentCount(3);

    std::shared_ptr <ITexture> spkTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    spkTexture->SetTextureFormat(gli::FORMAT_RGBA8_UNORM);
    spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    spkTexture->SetFilterMode(eFT_Min, eFM_Linear);
    spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    spkTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    spkTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
    m_spkGBuffer->SetTexture(spkTexture, 0);

    spkTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    spkTexture->SetTextureFormat(gli::FORMAT_RGBA32_SFLOAT);
    spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    spkTexture->SetFilterMode(eFT_Min, eFM_Linear);
    spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    spkTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    spkTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
    m_spkGBuffer->SetTexture(spkTexture, 1);

    spkTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    spkTexture->SetTextureFormat(gli::FORMAT_RGBA8_UNORM);
    spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    spkTexture->SetFilterMode(eFT_Min, eFM_Linear);
    spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    spkTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    spkTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
    m_spkGBuffer->SetTexture(spkTexture, 2);

    std::shared_ptr <ITexture> spkDepthTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    spkDepthTexture->SetTextureFormat(gli::FORMAT_D24_UNORM);
    spkDepthTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    spkDepthTexture->SetFilterMode(eFT_Min, eFM_Linear);
    spkDepthTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    spkDepthTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    spkDepthTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
    m_spkGBuffer->SetDepthTexture(spkDepthTexture);

    m_spkLightBuffer = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth, _nHeight);
    m_spkLightBuffer->SetRenderBufferColorAttactmentCount(1);
    m_spkLightBuffer->SetDepthTexture(spkDepthTexture);

    spkTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    spkTexture->SetTextureFormat(gli::FORMAT_RGBA8_UNORM);
    spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    spkTexture->SetFilterMode(eFT_Min, eFM_Linear);
    spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    spkTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    spkTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
    m_spkLightBuffer->SetTexture(spkTexture, 0);

    m_spkPostProcessBuffer [0] = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth / 2, _nHeight / 2);
    m_spkPostProcessBuffer [0]->SetRenderBufferColorAttactmentCount(1);
    m_spkPostProcessBuffer [1] = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth / 2, _nHeight / 2);
    m_spkPostProcessBuffer [1]->SetRenderBufferColorAttactmentCount(1);

    m_spkSecondPostProcessBuffer [0] = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth / 4, _nHeight / 4);
    m_spkSecondPostProcessBuffer [0]->SetRenderBufferColorAttactmentCount(1);
    m_spkSecondPostProcessBuffer [1] = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth / 4, _nHeight / 4);
    m_spkSecondPostProcessBuffer [1]->SetRenderBufferColorAttactmentCount(1);

    const unsigned int nBlurTextureCount = 2;
    for (unsigned int nIndex = 0; nIndex < nBlurTextureCount; nIndex++) {
        m_spkBlurTexture [nIndex] = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
        m_spkBlurTexture [nIndex]->SetTextureFormat(gli::FORMAT_RGBA8_UNORM);
        m_spkBlurTexture [nIndex]->SetTextureDimension(_nWidth / 2, _nHeight / 2, 1);
        m_spkBlurTexture [nIndex]->SetFilterMode(eFT_Min, eFM_Linear);
        m_spkBlurTexture [nIndex]->SetFilterMode(eFT_Mag, eFM_Linear);
        m_spkBlurTexture [nIndex]->SetClampMode(eCT_U, eCM_ClampToEdge);
        m_spkBlurTexture [nIndex]->SetClampMode(eCT_V, eCM_ClampToEdge);
        m_spkPostProcessBuffer [nIndex]->SetTexture(m_spkBlurTexture [nIndex], 0);

        m_spkSecondBlurTexture [nIndex] = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
        m_spkSecondBlurTexture [nIndex]->SetTextureFormat(gli::FORMAT_RGBA8_UNORM);
        m_spkSecondBlurTexture [nIndex]->SetTextureDimension(_nWidth / 4, _nHeight / 4, 1);
        m_spkSecondBlurTexture [nIndex]->SetFilterMode(eFT_Min, eFM_Linear);
        m_spkSecondBlurTexture [nIndex]->SetFilterMode(eFT_Mag, eFM_Linear);
        m_spkSecondBlurTexture [nIndex]->SetClampMode(eCT_U, eCM_ClampToEdge);
        m_spkSecondBlurTexture [nIndex]->SetClampMode(eCT_V, eCM_ClampToEdge);
        m_spkSecondPostProcessBuffer [nIndex]->SetTexture(m_spkSecondBlurTexture [nIndex], 0);
    }

    m_spkContrastTexture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    m_spkContrastTexture->SetTextureFormat(gli::FORMAT_RGBA8_UNORM);
    m_spkContrastTexture->SetTextureDimension(_nWidth, _nHeight, 1);
    m_spkContrastTexture->SetFilterMode(eFT_Min, eFM_Linear);
    m_spkContrastTexture->SetFilterMode(eFT_Mag, eFM_Linear);
    m_spkContrastTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
    m_spkContrastTexture->SetClampMode(eCT_V, eCM_ClampToEdge);

    m_spkContrastPassBuffer = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Genernal, _nWidth, _nHeight);
    m_spkContrastPassBuffer->SetRenderBufferColorAttactmentCount(1);
    m_spkContrastPassBuffer->SetTexture(m_spkContrastTexture, 0);

    InitMaterial();

    m_spkRenderLightState = IRenderSystem::CreateDefaultRenderState();
    m_spkRenderLightState->SetFrontFace(eFF_CounterClockWise);
    m_spkRenderLightState->SetCullMode(eCM_Front);
    m_spkRenderLightState->SetDepthTestEnable(false);
    m_spkRenderLightState->SetDepthOpMask(false);
    m_spkRenderLightState->SetBlendEnable(true);
    m_spkRenderLightState->SetAlphaTest(false);
    m_spkRenderLightState->SetBlendFactor(eBF_One, true, false);
    m_spkRenderLightState->SetBlendFactor(eBF_One, false, false);
    m_spkRenderLightState->SetBlendFactor(eBF_One, true, true);
    m_spkRenderLightState->SetBlendFactor(eBF_One, false, true);

    m_spkRenderLightVolumeState = IRenderSystem::CreateDefaultRenderState();
    m_spkRenderLightVolumeState->SetPolygonMode(ePM_Line);
    m_spkRenderLightVolumeState->SetDepthOpMask(false);
    m_spkRenderLightVolumeState->SetDepthTestEnable(false);

    m_spkRenderLightBodyState = IRenderSystem::CreateDefaultRenderState();
    m_spkRenderLightBodyState->SetDepthTestEnable(true);
    m_spkRenderLightBodyState->SetDepthOpMask(true);

    m_spkPostProcessState = IRenderSystem::CreateDefaultRenderState();
    m_spkPostProcessState->SetDepthTestEnable(false);
    m_spkPostProcessState->SetDepthOpMask(false);
    m_spkPostProcessState->SetBlendEnable(true);
    m_spkPostProcessState->SetAlphaTest(false);
    m_spkPostProcessState->SetBlendFactor(eBF_One, true, false);
    m_spkPostProcessState->SetBlendFactor(eBF_One, false, false);
    m_spkPostProcessState->SetBlendFactor(eBF_One, true, true);
    m_spkPostProcessState->SetBlendFactor(eBF_One, false, true);

    m_spkDefaultState = IRenderSystem::CreateDefaultRenderState();

    // Test Light
    std::shared_ptr <IMeshBuffer> pkBoundMesh = CreateSphereData(300.0f, 5, 5);
    m_spkTestMeshInstanceVB = pkBoundMesh->CreateVertexBuffer();
    m_spkTestMeshInstanceIB = pkBoundMesh->CreateIndexBuffer();

    float nLeft, nRight, nTop, nBottom;
    nLeft = (float)((_nWidth / 2) * -1);
    nRight = nLeft + (float)_nWidth;
    nTop = (float)(_nHeight / 2);
    nBottom = nTop - (float)_nHeight;
    std::shared_ptr <IMeshBuffer> spkScreenRectMesh = CreateRectData(nLeft, nRight, nTop, nBottom);
    m_spkScreenRectVB = spkScreenRectMesh->CreateVertexBuffer();
    m_spkScreenRectIB = spkScreenRectMesh->CreateIndexBuffer();
}

void CDeferredRenderPipeline::SetScreenText()
{
    AppendSceenText("BloomBrightness: %f\n", m_fBrightness);
    AppendSceenText("BloomContrast: %f\n", m_fContrast);
    AppendSceenText("BrightnessThreshold: %f\n", m_fBrightnessThreshold);
    AppendSceenText("BlurWeight: %f\n", m_fBlurWeight);
    AppendSceenText("RenderLightVolume: %s\n", (m_bRenderLightVolume) ? "ON" : "OFF");
    AppendSceenText("EnableBloom: %s\n", (m_bEnableBloom) ? "ON" : "OFF");
    AppendSceenText("UseBinTreeTest: %s\n", (m_bUseBinTreeTest) ? "ON" : "OFF");
    CModelViewPipeline::SetScreenText();
}

void CDeferredRenderPipeline::InitMaterial()
{
    CGLSLMaterialCreateInstanceFromFile kArgs;
    std::string kFileName = "data/GbufferShader.vs";
    kArgs.SetShaderName("GBufferVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/GbufferShader.fs";
    kArgs.SetShaderName("GBufferPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkGBufferMaterial = kArgs.GetMaterialInstance();

    kFileName = "data/DeferredRenderingShader.vs";
    kArgs.SetShaderName("DeferredRenderingVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/DeferredRenderingShader.fs";
    kArgs.SetShaderName("DeferredRenderingPixelShader", eShader_Pixel);
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

void CDeferredRenderPipeline::UpdateLightUniform(IRenderHandler& _rkRenderComp,
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
    if (pkLightSceneComp) {
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightPos", pkLightSceneComp->GetPosition(), eConstant_floatVec3);
    }
    if (pkCameraSceneComp) {
        AssignConstant <glm::vec3>(_rkRenderComp, "uCameraPos", pkCameraSceneComp->GetPosition(), eConstant_floatVec3);
    }
#if ENABLE_CUBE_SHADOW
    if (pkCubeMapCamComp) {
#else
    if (false) {
#endif
        IFrameBuffer* pkCubeFrameBuffer = pkCubeMapCamComp->GetTargetFrameBuffer();
        AssignConstant <bool>(_rkRenderComp, "uEnableCubeDepthMap", true, eConstant_bool);
        AssignConstant <ITexture*>(_rkRenderComp, "uCubeDepthMap", pkCubeFrameBuffer->GetTexture(0), eConstant_Texture);
    }
    else {
        AssignConstant <bool>(_rkRenderComp, "uEnableCubeDepthMap", false, eConstant_bool);
        AssignConstant <ITexture*>(_rkRenderComp, "uCubeDepthMap", nullptr, eConstant_Texture);
    }
    if (_pkLight) {
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightDiffuse",(const glm::vec3&)_pkLight->GetDiffuseColor(), eConstant_floatVec3);
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightAmbient",(const glm::vec3&)_pkLight->GetAmbientColor(), eConstant_floatVec3);
        AssignConstant <glm::vec3>(_rkRenderComp, "uLightSpecular",(const glm::vec3&)_pkLight->GetSpecularColor(), eConstant_floatVec3);
        AssignConstant <float>(_rkRenderComp, "uLightDimmer", _pkLight->GetLightDimmer(), eConstant_float);
        CPointLightComponent* pkPointLight = dynamic_cast <CPointLightComponent*>(_pkLight);
        if (pkPointLight) {
            AssignConstant <float>(_rkRenderComp, "uLightRange", pkPointLight->GetRadius(), eConstant_float);
        }
    }
}

void CDeferredRenderPipeline::RenderGbuffer(IRenderer& _rkRenderer)
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

            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;

            _rkRenderer.OnDraw(m_rkEventMgr, kRenderHandler);
            if (ms_bDrawObjectBound)
                AddRenderBoundBoxList(pkBound->GetWorldCenter(), pkBound->GetBoxVector());
        }
    }
}

void CDeferredRenderPipeline::RenderLightBody(IRenderer& _rkRenderer)
{
    if (!m_pkLightBodyMaterial)
        return;

    CGLMBaseCameraComponent* pkCamera = nullptr;
    if (!m_kCameraEntity)
        return;
    if (!m_kCameraEntity.has_component <ICameraHandler>())
        return;
    if (!m_kCameraEntity.has_component <ISceneHandler>())
        return;

    pkCamera = static_cast<CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());
    ISceneHandler& rkCameraSceneComp = (*m_kCameraEntity.component <ISceneHandler>().get());
    IMaterial* pkMaterial = m_pkLightBodyMaterial;

    for (auto kEntity : m_kLightEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;

        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        ILightHandler& rkLightHandler = (*kEntity.component<ILightHandler>().get());
        if (!rkLightHandler->IsHasLightBody())
            continue;
        bool bHandled = false;
        switch (rkLightHandler->GetLightType())
        {
            default:
                break;
            case eLT_Point:
            {
                CPointLightComponent* pkPointLight = dynamic_cast <CPointLightComponent*>(rkLightHandler.get());
                if (!pkPointLight)
                    continue;
                if (!kEntity.has_component <ISceneHandler>()) {
                    continue;
                }
                entityx::ComponentHandle <ISceneHandler> kSceneHandler = kEntity.component <ISceneHandler>();
                CGLMSceneComponent* pkGLMSceneComp = dynamic_cast <CGLMSceneComponent*>(kSceneHandler->get());
                if (!pkGLMSceneComp)
                    continue;
                glm::vec3 kOrgScale = pkGLMSceneComp->GetScale();
                float nLightBodyScale = pkPointLight->GetLightBodyRadius() / 300.f;
                pkGLMSceneComp->SetScale(glm::vec3(nLightBodyScale));
                if (!kRenderHandler->SetMaterial(pkMaterial))
                    continue;
                if (!kRenderHandler->SetAttribute(m_spkTestMeshInstanceVB.get(), m_spkTestMeshInstanceIB.get()))
                    continue;
                if (!kRenderHandler->SetRenderState(m_spkRenderLightBodyState.get()))
                    continue;
                if (!pkMaterial->ApplyCameraData(pkCamera))
                    continue;
                pkGLMSceneComp->UpdateMaterialConstant(pkMaterial);
                const glm::vec3& kColor = (const glm::vec3&)(pkPointLight->GetDiffuseColor());
                glm::vec4 kLightColor(kColor, 1.0f);
                AssignConstant <glm::vec4>(kRenderHandler, "uColor", kLightColor, eConstant_floatVec4);
                if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                    continue;

                _rkRenderer.OnDraw(m_rkEventMgr, kRenderHandler);
                pkGLMSceneComp->SetScale(kOrgScale);
                if (ms_bDrawObjectBound) {
                    IGeometryHandler& rkGeomHandler = (*kEntity.component<IGeometryHandler> ().get());
                    CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
                    if (pkBound) {
                        AddRenderBoundBoxList(pkBound->GetWorldCenter(), pkBound->GetBoxVector());
                    }
                }
                break;
            }
        }
    }
}

void CDeferredRenderPipeline::RenderBloom(IRenderer& _rkRenderer, ITexture* _pkTexture, bool _bVertical)
{
    if (!m_kTemplateRenderComp->SetMaterial(m_pkBloomMaterial))
        return;
    if (!m_kTemplateRenderComp->SetAttribute(m_spkScreenRectVB.get(), m_spkScreenRectIB.get()))
        return;
    if (!m_kTemplateRenderComp->SetRenderState(m_spkPostProcessState.get()))
        return;
    char nIndex = (_bVertical) ? 1 : 0;
    AssignConstant<glm::mat4>(m_kTemplateRenderComp, "uProjectionMatrix", m_kOrthoMatrix, eConstant_Matrix4x4);
    const glm::vec2 kResolution(m_spkPostProcessBuffer[nIndex]->GetViewPortWidth(), m_spkPostProcessBuffer[nIndex]->GetViewPortHeight());
    AssignConstant<glm::vec2>(m_kTemplateRenderComp, "uResolution", kResolution, eConstant_floatVec2);
    AssignConstant<float>(m_kTemplateRenderComp, "uWeight", m_fBlurWeight, eConstant_float);
    AssignConstant<float>(m_kTemplateRenderComp, "uRadius", 1.0f, eConstant_float);
    AssignConstant<int>(m_kTemplateRenderComp, "uBlurMode", 2, eConstant_int);
    AssignConstant<bool>(m_kTemplateRenderComp, "uHorizontal", !_bVertical, eConstant_bool);
    AssignConstant<ITexture*>(m_kTemplateRenderComp, "uDiffuse", _pkTexture, eConstant_Texture);
    AssignConstant<ITexture*>(m_kTemplateRenderComp, "uDepthWeight", m_spkLightBuffer->GetDepthTexture(), eConstant_Texture);    

    if (!m_kTemplateRenderComp->ValidatePipeline(_rkRenderer))
        return;

    _rkRenderer.OnDraw(m_rkEventMgr, m_kTemplateRenderComp);
}

void CDeferredRenderPipeline::RenderLight(IRenderer& _rkRenderer)
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
        if (kLightEntity.has_component <ICameraHandler>()) {
            pkCubeCam = dynamic_cast <CGLMCubeCameraComponent*>(kLightEntity.component <ICameraHandler>()->get());
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

void CDeferredRenderPipeline::RenderContrast(IRenderer& _rkRenderer)
{
    if (!m_kTemplateRenderComp->SetMaterial(m_pkContrastMaterial))
        return;
    if (!m_kTemplateRenderComp->SetAttribute(m_spkScreenRectVB.get(), m_spkScreenRectIB.get()))
        return;
    if (!m_kTemplateRenderComp->SetRenderState(m_spkPostProcessState.get()))
        return;
    AssignConstant<glm::mat4>(m_kTemplateRenderComp, "uProjectionMatrix", m_kOrthoMatrix, eConstant_Matrix4x4);

    const glm::vec2 kBrightnessContrast(m_fBrightness, m_fContrast);
    AssignConstant <glm::vec2> (m_kTemplateRenderComp, "uBrightnessContrast", kBrightnessContrast, eConstant_floatVec2);
    AssignConstant <float> (m_kTemplateRenderComp, "uBrightnessThreshold", m_fBrightnessThreshold, eConstant_float);
    ITexture* pkTexture = m_spkLightBuffer->GetTexture(0);
    AssignConstant<ITexture*>(m_kTemplateRenderComp, "uDiffuse", pkTexture, eConstant_Texture);

    if (!m_kTemplateRenderComp->ValidatePipeline(_rkRenderer))
        return;

    _rkRenderer.OnDraw(m_rkEventMgr, m_kTemplateRenderComp);
}

void CDeferredRenderPipeline::RenderCubeMap(IRenderer& _rkRenderer, int _nPass)
{
    if (m_kEnvCubeEntity.empty())
        return;
#if ENABLE_CUBE_SHADOW
    IMaterial* pkMaterial = m_pkWriteDepthMaterial;
#else
    IMaterial* pkMaterial = m_pkBasicTexturedMaterial;
#endif
    if (!m_kTemplateRenderComp->SetMaterial(pkMaterial))
        return;
    if (!m_kTemplateRenderComp->SetRenderState(m_spkDefaultState.get()))
        return;

    SCubeMapContext& rkContext = m_kEnvCubeEntity [_nPass];
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
        for (auto kDrawEntity : rkContext.m_kTestedObject[nFace]) {
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

void CDeferredRenderPipeline::RenderScreen(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha)
{
    CModelViewPipeline::RenderScreen(_rkRenderer, _rkEventMgr, _pkCamera, _rkOrtha);

    // Render screen space Bloom
    if (!m_kTemplateRenderComp->SetMaterial(m_pkScreenSpaceMaterial))
        return;
    if (!m_kTemplateRenderComp->SetAttribute(m_spkScreenRectVB.get(), m_spkScreenRectIB.get()))
        return;
    if (!m_kTemplateRenderComp->SetRenderState(m_spkPostProcessState.get()))
        return;
    AssignConstant<glm::mat4>(m_kTemplateRenderComp, "uProjectionMatrix", m_kOrthoMatrix, eConstant_Matrix4x4);
    // 2 Pass with Bloom & Orgin
    const unsigned int nMaxPass = (m_bEnableBloom) ? 2 : 1;
    for (unsigned int nPass = 0; nPass < nMaxPass; nPass++) {
        ITexture* pkTexture = (nPass) ? m_spkSecondBlurTexture [1].get() : m_spkLightBuffer->GetTexture(0);
        AssignConstant<ITexture*>(m_kTemplateRenderComp, "uDiffuse", pkTexture, eConstant_Texture);
        if (!m_kTemplateRenderComp->ValidatePipeline(_rkRenderer))
            return;

        _rkRenderer.OnDraw(m_rkEventMgr, m_kTemplateRenderComp);
    }

}

IFrameBuffer* CDeferredRenderPipeline::GetFrameBuffer(int _nCurrentPass)
{
    int nPreRenderCount = m_kEnvCubeEntity.size();
    if (_nCurrentPass < nPreRenderCount) {
        CGLMCubeCameraComponent* pkCubeCam = static_cast <CGLMCubeCameraComponent*>(m_kEnvCubeEntity[_nCurrentPass].m_kEntity.component<ICameraHandler>()->get());
        if (!pkCubeCam)
            return nullptr;
        return pkCubeCam->GetTargetFrameBuffer();
    }
    int nPostRenderPass = _nCurrentPass - nPreRenderCount;
    switch (nPostRenderPass)
    {
        case CDeferredRenderPipeline::ePT_GBuffer:
        {
            return m_spkGBuffer.get();
        }
        case CDeferredRenderPipeline::ePT_LightPass:
        case CDeferredRenderPipeline::ePT_Emissive:
        {
            return m_spkLightBuffer.get();
        }
        case CDeferredRenderPipeline::ePT_RenderScreen:
        {
            return m_spkDefaultBuffer.get();
        }
        case CDeferredRenderPipeline::ePT_ContrastProc:
        {
            return m_spkContrastPassBuffer.get();
        }
        case CDeferredRenderPipeline::ePT_BlurV:
        {
            //m_spkPostProcessBuffer [0]->SetTexture(m_spkBlurTexture[0], 0);
            return m_spkPostProcessBuffer [0].get();
        }
        case CDeferredRenderPipeline::ePT_BlurV2:
        {
            return m_spkSecondPostProcessBuffer [0].get();
        }
        case CDeferredRenderPipeline::ePT_BlurH:
        {
            return m_spkPostProcessBuffer [1].get();
        }
        case CDeferredRenderPipeline::ePT_BlurH2:
        {
            return m_spkSecondPostProcessBuffer [1].get();
        }
        default:
            break;
    }
    return nullptr;
}

void CDeferredRenderPipeline::SetPassObject(entityx::EntityManager& _rkMgr)
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
                 IGeometryHandler& rkGeometry = (*kEntity.component <IGeometryHandler>().get());
                 if (!rkGeometry->IsCastLighting())
                     continue;
                 bool bTested = false;
                 const float fRadius = pkPointLight->GetRadius();
                 const size_t nGeomCount = rkGeometry->GetGeometryCount();
                 for (size_t nIndex = 0; nIndex < nGeomCount; nIndex++)
                 {
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
                 // Make cube map shadow
                 if (kEntity.has_component <ICameraHandler>()) {
                     CGLMCubeCameraComponent* pkCubeCamera = dynamic_cast <CGLMCubeCameraComponent*>(kEntity.component <ICameraHandler>()->get());
                     if (pkCubeCamera && pkCubeCamera->IsCameraActive()) {
                         m_kEnvCubeEntity.emplace_back();
                         SCubeMapContext& rkContext = *m_kEnvCubeEntity.rbegin();
                         rkContext.m_kEntity = kEntity;
                     }
                 }
                 break;
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
                        kCubeObejct.m_kTestedObject[nFace].push_back(kDrawEntity);
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
    rkSceneSystem.PreOrderTestOctree <CDeferredRenderPipeline>(*this);
    rkSceneSystem.TestDynamicObjects <CDeferredRenderPipeline>(*this);

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
            // Make cube map shadow
            if (kEntity.has_component <ICameraHandler>()) {
                CGLMCubeCameraComponent* pkCubeCamera = dynamic_cast <CGLMCubeCameraComponent*>(kEntity.component <ICameraHandler>()->get());
                if (pkCubeCamera && pkCubeCamera->IsCameraActive()) {
                    m_kEnvCubeEntity.emplace_back();
                    SCubeMapContext& rkContext = *m_kEnvCubeEntity.rbegin();
                    rkContext.m_kEntity = kEntity;
                }
            }
            break;
        }
    }

    unsigned short nPointLightIndex = 0;
    for (auto& kCubeObejct : m_kEnvCubeEntity) {
        CGLMCubeCameraComponent* pkCam = static_cast <CGLMCubeCameraComponent*>(kCubeObejct.m_kEntity.component <ICameraHandler>()->get());
        ISceneComponent* pkSceneComp = kCubeObejct.m_kEntity.component <ISceneHandler>()->get();
        const char nTotalFace = 6;
        for (char nFace = 0; nFace < nTotalFace; ++nFace) {
            pkCam->SetCurrentFace(nFace);
            unsigned int nRevision = m_nFrameRevision;
            nRevision <<= 3;
            nRevision += nFace;
            nRevision <<= 16;
            nRevision += nPointLightIndex++;
            kCubeObejct.m_nRevision = nRevision;
            CModelViewSceneSystem::GetSingleton().PreOrderTestOctree <CDeferredRenderPipeline::SCubeMapContext>(kCubeObejct);
        }
    }

    unsigned int nTotalPassCount = m_kEnvCubeEntity.size() + ePT_Max;
    SetRenderPassCount(nTotalPassCount);
}

void CDeferredRenderPipeline::OnClear(IRenderer& _rkRenderer, int _nCurrentPass)
{
    int nPreRenderCount = m_kEnvCubeEntity.size();
    // Clear By Cube map proc
    if (_nCurrentPass < nPreRenderCount)
        return;
    int nClearBit = 0;
    int nPostRender = _nCurrentPass - nPreRenderCount;
    if (nPostRender == CDeferredRenderPipeline::ePT_Emissive) {
        //nClearBit = IRenderer::eBCB_Color;
        return;
    }
    else if (nPostRender == CDeferredRenderPipeline::ePT_LightPass) {
        nClearBit = IRenderer::eBCB_Color;
    }
#if 0
    else if (nPostRender == CDeferredRenderPipeline::ePT_RenderScreen) {
        nClearBit = IRenderer::eBCB_Color;
        //return;
    }
#endif
    else {
        nClearBit = (IRenderer::eBCB_Color | IRenderer::eBCB_Depth);
    } 

    _rkRenderer.SetClearBufferBit(nClearBit);
    _rkRenderer.SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    _rkRenderer.ClearBuffer();
}

void CDeferredRenderPipeline::PassBegin(IRenderer& _rkRenderer, int _nCurrent)
{

}

void CDeferredRenderPipeline::OnDraw(IRenderer& _rkRenderer, entityx::EventManager& rkEventMgr, int _nCurrent)
{
    int nPreRenderCount = m_kEnvCubeEntity.size();
    if (_nCurrent < nPreRenderCount) {
        RenderCubeMap(_rkRenderer, _nCurrent);
        return;
    }
    int nPostRender = _nCurrent - nPreRenderCount;
    switch (nPostRender)
    {
        case CDeferredRenderPipeline::ePT_GBuffer:
        {
            RenderGbuffer(_rkRenderer);
            return;
        }
        case CDeferredRenderPipeline::ePT_Emissive:
        {
            RenderLightBody(_rkRenderer);
            return;
        }
        case CDeferredRenderPipeline::ePT_LightPass:
        {
            RenderLight(_rkRenderer);
            return;
        }
        case  CDeferredRenderPipeline::ePT_ContrastProc:
        {
            if (m_bEnableBloom)
                RenderContrast(_rkRenderer);
            return;
        }
        case CDeferredRenderPipeline::ePT_BlurV:
        case CDeferredRenderPipeline::ePT_BlurH:
        {
            if (m_bEnableBloom) {
                bool bVertical = (nPostRender == CDeferredRenderPipeline::ePT_BlurV);
                ITexture* pkTexture = (bVertical) ? m_spkContrastTexture.get () : m_spkBlurTexture [0].get ();
                RenderBloom(_rkRenderer, pkTexture, bVertical);
            }
            return;
        }
        case CDeferredRenderPipeline::ePT_BlurV2:
        case CDeferredRenderPipeline::ePT_BlurH2:
        {
            if (m_bEnableBloom) {
                bool bVertical = (nPostRender == CDeferredRenderPipeline::ePT_BlurV2);
                ITexture* pkTexture = (bVertical) ? m_spkBlurTexture [1].get () : m_spkSecondBlurTexture [0].get ();
                RenderBloom(_rkRenderer, pkTexture, bVertical);
            }
            return;
        }
        case CDeferredRenderPipeline::ePT_RenderScreen:
        {
            CGLMBaseCameraComponent* pkCamera = nullptr;
            if (!m_kCameraEntity)
                return;
            if (!m_kCameraEntity.has_component <ICameraHandler>())
                return;
            if (!m_kCameraEntity.has_component <ISceneHandler>())
                return;
            pkCamera = static_cast<CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());
            RenderScreen(_rkRenderer, m_rkEventMgr, pkCamera, m_kOrthoMatrix);
            return;
        }
        default:
            break;
    }
}

void CDeferredRenderPipeline::PassEnd(IRenderer& _rkRenderer, int _nCurrent)
{
    //SwapBuffer from Render system base flow
#if 0
    int nTotalCount = GetRenderPassCount();
    if ((nTotalCount - _nCurrent - 1) > 0) {
        return;
    }
    _rkRenderer.OnSwapBuffers();
#endif
}

void CDeferredRenderPipeline::OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight)
{
    m_nScreenWidth = _nWidth;
    m_nScreenHeight = _nHeight;
    CModelViewPipeline::OnResize(_rkRenderer, _nWidth, _nHeight);
    m_spkGBuffer->Resize(_nWidth, _nHeight);
    m_spkLightBuffer->Resize(_nWidth, _nHeight);
    m_spkContrastPassBuffer->Resize(_nWidth, _nHeight);
    for (unsigned int nIndex = 0; nIndex < 2; nIndex++) {
        m_spkPostProcessBuffer[nIndex]->Resize(_nWidth / 2, _nHeight / 2);
        m_spkSecondPostProcessBuffer[nIndex]->Resize(_nWidth / 4, _nHeight / 4);
        m_spkBlurTexture [nIndex]->SetTextureDimension(_nWidth / 2, _nHeight / 2, 1);
        m_spkSecondBlurTexture [nIndex]->SetTextureDimension(_nWidth / 4, _nHeight / 4, 1);
    }
}