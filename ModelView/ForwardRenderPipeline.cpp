#include "MainPCH.h"
#include "ForwardRenderPipeline.h"
#include "GLMMainCameraComponent.h"
#include "GLMSceneComponent.h"
#include "IGeometryComponent.h"
#include "ModelViewSceneSystem.h"
#include "DefaultLightComponent.h"
#include "GLMBoxBound.h"
#include "GLSLMaterialSystem.h"

CForwardRenderPipeline::CForwardRenderPipeline(entityx::EventManager& _rkEventMgr, int _nWidth, int _nHeight)
    :CModelViewPipeline(_rkEventMgr, _nWidth, _nHeight)
    , m_bDrawDebugNormal(false)
{
}

CForwardRenderPipeline::~CForwardRenderPipeline()
{

}

void CForwardRenderPipeline::Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    m_spkDebugZTestState = IRenderSystem::CreateDefaultRenderState();
    m_spkDebugZTestState->SetDepthTestEnable(true);

    CModelViewPipeline::Prepared(_nWidth, _nHeight, _rkEntityMgr, _rkEventMgr);
    CGLSLMaterialCreateInstanceFromFile kArgs;
    std::string kFileName = "data/NormalDebugShader.vs";
    kArgs.SetShaderName("NormalDebugVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/NormalDebugShader.gs";
    kArgs.SetShaderName("NormalDebugGeometryShader", eShader_Geomtery);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Geomtery);
    kFileName = "data/NormalDebugShader.fs";
    kArgs.SetShaderName("NormalDebugPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    SetCreateMaterialEvent(kArgs);
    m_pkNormalDebugMaterial = kArgs.GetMaterialInstance();
}

void CForwardRenderPipeline::SetPassObject(entityx::EntityManager& _rkMgr)
{
    CModelViewPipeline::SetPassObject(_rkMgr);
    m_kDrawEntity.clear();
    m_kTranspanecyEntity.clear();
    m_kScreenEntity.clear();
    m_kCameraEntity.invalidate();
    m_pkMainCamera = nullptr;
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

    if (!m_kCameraEntity)
        return;
    CGLMMainCameraComponent* pkCamera = dynamic_cast <CGLMMainCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get());
    if (!pkCamera)  // no Camera 
        return;
    m_pkMainCamera = pkCamera;
    ISceneComponent* pkCameraScenComp = m_kCameraEntity.component <ISceneHandler>()->get();

    // Query Static Object
    CModelViewSceneSystem& rkSceneSystem = CModelViewSceneSystem::GetSingleton();
    rkSceneSystem.PreOrderTestOctree <CForwardRenderPipeline>(*this);
    rkSceneSystem.TestDynamicObjects <CForwardRenderPipeline>(*this);

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
}

bool CForwardRenderPipeline::operator()(const TBinTreeNode<3>& _rkSceneNode)
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
        const TPoint <3>& rkBlockCenter = _rkSceneNode.GetBlockCenter();
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
        bool bIsTranspancy = (rkGeom->GetTextureDescByName("uOpacity") != nullptr);
        for (size_t nIndex = 0; nIndex < nGeomSize; nIndex++) {
            rkGeom->ActiveGeometry(nIndex);
            IBound* pkBound = rkGeom->GetBound();
#if 1
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


bool CForwardRenderPipeline::operator()(ISceneComponent* _pkDynamicObject)
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

void CForwardRenderPipeline::RenderDebugNormal(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr)
{
    CGLMBaseCameraComponent* pkCamera = (m_kCameraEntity) ? static_cast <CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get()) : nullptr;

    if (!pkCamera)
        return;

    const glm::mat4& rkView = pkCamera->GetViewMatrix();
    const glm::mat4& rkProject = pkCamera->GetProjectMatrix();
    glm::mat4 kVPMatrix = rkProject * rkView;

    for (entityx::Entity kEntity : m_kDrawEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        if (kEntity.has_component <ILightHandler>())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }
            if (!pkCamera->IsInView(pkBound))
                continue;

            IMaterial* pkMaterial = m_pkNormalDebugMaterial;
            if (!kRenderHandler->SetMaterial(pkMaterial))
                continue;

            glm::mat4 kWVPMatirx = kVPMatrix;
            if (kEntity.has_component <ISceneHandler>()) {
                CGLMSceneComponent* pkComp = dynamic_cast <CGLMSceneComponent*>(kEntity.component<ISceneHandler>()->get());
                if (pkComp) {
                    kWVPMatirx = kWVPMatirx * pkComp->GetTransform();
                }
            }
            AssignConstant <glm::mat4>(kRenderHandler, "uMVPMatrix", kWVPMatirx, eConstant_Matrix4x4);
            AssignConstant <float>(kRenderHandler, "uNormalLength", 10.0f, eConstant_float);

            kRenderHandler->SetRenderState(m_spkDebugZTestState.get());
            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;
            _rkRenderer.OnDraw(_rkEventMgr, kRenderHandler);
        }
    }

    for (entityx::Entity kEntity : m_kTranspanecyEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        if (kEntity.has_component <ILightHandler>())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }
            if (!pkCamera->IsInView(pkBound))
                continue;

            IMaterial* pkMaterial = m_pkNormalDebugMaterial;
            if (!kRenderHandler->SetMaterial(pkMaterial))
                continue;

            glm::mat4 kWVPMatirx = kVPMatrix;
            if (kEntity.has_component <ISceneHandler>()) {
                CGLMSceneComponent* pkComp = dynamic_cast <CGLMSceneComponent*>(kEntity.component<ISceneHandler>()->get());
                if (pkComp) {
                    kWVPMatirx *= pkComp->GetTransform();
                }
            }
            AssignConstant <glm::mat4>(kRenderHandler, "uMVPMatrix", kWVPMatirx, eConstant_Matrix4x4);
            AssignConstant <float>(kRenderHandler, "uNormalLength", 30.0f, eConstant_float);

            kRenderHandler->SetRenderState(m_spkDebugZTestState.get());
            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;
            _rkRenderer.OnDraw(_rkEventMgr, kRenderHandler);
        }
    }

    RenderScreen(_rkRenderer, _rkEventMgr, pkCamera, m_kOrthoMatrix);
}

void CForwardRenderPipeline::RenderDiffuse(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrent)
{
    CModelViewPipeline::OnDraw(_rkRenderer, _rkEventMgr, _nCurrent);
}

void CForwardRenderPipeline::RenderBound()
{
    if (!ms_bDrawObjectBound)
        return;
    CGLMBaseCameraComponent* pkCamera = (m_kCameraEntity) ? static_cast <CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get()) : nullptr;

    if (!pkCamera)
        return;

    for (entityx::Entity kEntity : m_kDrawEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        if (kEntity.has_component <ILightHandler>())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }
            if (!pkCamera->IsInView(pkBound))
                continue;

            AddRenderBoundBoxList(pkBound->GetWorldCenter(), pkBound->GetBoxVector());
        }
    }

    for (entityx::Entity kEntity : m_kTranspanecyEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        if (kEntity.has_component <ILightHandler>())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }
            if (!pkCamera->IsInView(pkBound))
                continue;

            AddRenderBoundBoxList(pkBound->GetWorldCenter(), pkBound->GetBoxVector());
        }
    }
}

void CForwardRenderPipeline::SetScreenText()
{
    AppendSceenText("DrawDebugNormal: %s\n", (m_bDrawDebugNormal) ? "ON" : "OFF");
    CModelViewPipeline::SetScreenText();
}

void CForwardRenderPipeline::OnDraw(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrent)
{
    RenderBound();
    RenderDiffuse(_rkRenderer, _rkEventMgr, _nCurrent);

    if (m_bDrawDebugNormal) {
        RenderDebugNormal(_rkRenderer, _rkEventMgr);
        return;
    }
}