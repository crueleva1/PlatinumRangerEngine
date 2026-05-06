#include "GLMMainPCH.h"
#include "GLMPipeline.h"
#include "GLMMainCameraComponent.h"
#include "GLMSceneComponent.h"
#include "GLMBoxBound.h"
#include "GeomertryComponent.h"

CGLMDefaultPipeline::CGLMDefaultPipeline()
{

}

CGLMDefaultPipeline::CGLMDefaultPipeline(int _nWidth, int _nHeight)
{
    
}

CGLMDefaultPipeline::~CGLMDefaultPipeline()
{

}

void CGLMDefaultPipeline::Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    if (IsInited())
        return;

    IRenderPipeline::Prepared(_nWidth, _nHeight, _rkEntityMgr, _rkEventMgr);
    m_spkDefaultBuffer = IRenderSystem::CreateFrameBuffer(IRenderer::eFBT_Default, _nWidth, _nHeight);
    SetRenderPassCount(1);
    UpdateOrthoMatrix(_nWidth, _nHeight);
}

void CGLMDefaultPipeline::Active(entityx::EventManager& _rkEventMgr)
{
    IRenderPipeline::Active(_rkEventMgr);
}

void CGLMDefaultPipeline::Deactive(entityx::EventManager& _rkEventMgr)
{
    IRenderPipeline::Deactive(_rkEventMgr);
}

IFrameBuffer* CGLMDefaultPipeline::GetFrameBuffer(int _nCurrentPass)
{
    return m_spkDefaultBuffer.get();
}

void CGLMDefaultPipeline::SetPassObject(entityx::EntityManager& _rkMgr)
{
    m_kDrawEntity.clear();
    m_kTranspanecyEntity.clear();
    m_kScreenEntity.clear();
    m_kCameraEntity.invalidate();
    for (entityx::Entity kEntity : _rkMgr.entities_with_components <ICameraHandler> ()) {
        entityx::ComponentHandle <ICameraHandler> kHandler = kEntity.component <ICameraHandler> ();
        ICameraComponent* pkIComp = kHandler->get ();
        if (!pkIComp->IsCameraActive ())
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
    ISceneComponent* pkCameraScenComp = m_kCameraEntity.component <ISceneHandler>()->get();
    for (entityx::Entity kEntity : _rkMgr.entities_with_components <IRenderHandler, IGeometryHandler>()) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler> ().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        bool bScreenSpace = false;
        if (kEntity.has_component <ISceneHandler> ()) {
            ISceneHandler& rkSceneHandler = (*kEntity.component<ISceneHandler> ().get ());
            CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(rkSceneHandler.get ());
            if (pkSceneComp) {
                bScreenSpace |= pkSceneComp->IsScreenSpace ();
            }
        }
        if (bScreenSpace) {
            m_kScreenEntity.push_back(kEntity);
            continue;
        }
        bool bIsInView = false;
        IGeometryHandler& rkGeometryHandler = (*kEntity.component<IGeometryHandler>().get());
        const size_t nCount = rkGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
            rkGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(rkGeometryHandler->GetBound());
            // Make sure all bound signed test result
            bIsInView |= pkCamera->IsInView(pkBound, pkCamera->GetCameraNameHash());
        }

        if (kEntity.has_component <ILightHandler>()) {
            continue;
        }

        if (!bIsInView)
            continue;

        if (rkGeometryHandler->GetTextureDescByName("uOpacity")) {
            m_kTranspanecyEntity.push_back(kEntity);
            continue;
        }

        m_kDrawEntity.push_back(kEntity);
    }
}

void CGLMDefaultPipeline::OnClear(IRenderer& _rkRenderer, int _nCurrentPass)
{
    _rkRenderer.SetClearBufferBit(IRenderer::eBCB_Color | IRenderer::eBCB_Depth);
    _rkRenderer.SetClearColor(0.1f, 0.6f, 0.5f, 1.0f);
    _rkRenderer.ClearBuffer();
}

void CGLMDefaultPipeline::PassBegin(IRenderer& _rkRenderer, int _nCurrent)
{

}

void CGLMDefaultPipeline::OnDraw(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrent)
{
    IRenderer& rkRenderer = _rkRenderer;
    CGLMBaseCameraComponent* pkCamera = (m_kCameraEntity) ? static_cast <CGLMBaseCameraComponent*>(m_kCameraEntity.component <ICameraHandler>()->get()) : nullptr;
    ISceneComponent* pkSceneComp = m_kCameraEntity.component <ISceneHandler>()->get();
    for (entityx::Entity kEntity : m_kDrawEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        if (!kEntity.has_component <IMaterialHandler>())
            continue;
        IMaterialHandler& kMaterialHandler = *(kEntity.component<IMaterialHandler>().get());

        if (!rkRenderer.PreparePipeline(_rkEventMgr, kRenderHandler, kMaterialHandler, kGeometryHandler)) {
            continue;
        }
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound());
            if (!pkBound) {
                continue;
            }

            if (!pkBound->IsTested(pkCamera->GetCameraNameHash()))
                continue;

            if (!kRenderHandler->SetAttribute(kGeometryHandler->GetVertexBuffer(), kGeometryHandler->GetIndiceBuffer()))
                continue;

            IMaterial* pkMaterial = kMaterialHandler->GetMaterialInstance();
            if (!pkMaterial)
                continue;

            if (!pkMaterial->ApplyCameraData(pkCamera))
                continue;

            if (kEntity.has_component <ISceneHandler> ()) {
                entityx::ComponentHandle <ISceneHandler> kSceneHandler = kEntity.component <ISceneHandler>();
                CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kSceneHandler->get());
                if (pkSceneComp) {
                    pkSceneComp->UpdateMaterialConstant (pkMaterial);
                }
            }

            if (!pkMaterial->ApplyGeometryTexture (kGeometryHandler.get()))
                continue;

            const bool bHasMask = false;
            AssignConstant <bool> (kRenderHandler, "uHasMask", bHasMask, eConstant_bool);

            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;

            rkRenderer.OnDraw(_rkEventMgr, kRenderHandler);
        }
    }

    for (entityx::Entity kEntity : m_kTranspanecyEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        IMaterialHandler& kMaterialHandler = *(kEntity.component<IMaterialHandler>().get());

        if (!rkRenderer.PreparePipeline(_rkEventMgr, kRenderHandler, kMaterialHandler, kGeometryHandler)) {
            continue;
        }
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);
            CGLMBoundBox* pkBound = dynamic_cast<CGLMBoundBox*>(kGeometryHandler->GetBound ());
            if (!pkBound) {
                continue;
            }

            if (!pkCamera->IsInView(pkBound))
                continue;

            if (!kRenderHandler->SetAttribute(kGeometryHandler->GetVertexBuffer(), kGeometryHandler->GetIndiceBuffer()))
                continue;

            IMaterial* pkMaterial = kMaterialHandler->GetMaterialInstance();
            if (!pkMaterial)
                continue;

            if (!pkMaterial->ApplyCameraData(pkCamera))
                continue;

            if (kEntity.has_component <ISceneHandler> ()) {
                entityx::ComponentHandle <ISceneHandler> kSceneHandler = kEntity.component <ISceneHandler>();
                CGLMSceneComponent* pkSceneComp = dynamic_cast <CGLMSceneComponent*>(kSceneHandler->get());
                if (pkSceneComp) {
                    pkSceneComp->UpdateMaterialConstant (pkMaterial);
                }
            }

            if (!pkMaterial->ApplyGeometryTexture (kGeometryHandler.get()))
                continue;

            const bool bHasMask = true;
            AssignConstant <bool> (kRenderHandler, "uHasMask", bHasMask, eConstant_bool);

            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;

            rkRenderer.OnDraw(_rkEventMgr, kRenderHandler);
        }
    }

    RenderScreen(_rkRenderer, _rkEventMgr, pkCamera, m_kOrthoMatrix);
}

void CGLMDefaultPipeline::PassEnd(IRenderer& _rkRenderer, int _nCurrent)
{
    //SwapBuffer from Render system base flow
    //_rkRenderer.OnSwapBuffers();
}

void CGLMDefaultPipeline::RenderScreen(IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, ICameraComponent* _pkCamera, const  glm::mat4& _rkOrtha)
{
    // Render Screen element
    for (auto kEntity : m_kScreenEntity) {
        IRenderHandler& kRenderHandler = *(kEntity.component <IRenderHandler>().get());
        if (!kRenderHandler->IsRenderable())
            continue;
        IGeometryHandler& kGeometryHandler = *(kEntity.component<IGeometryHandler>().get());
        IMaterialHandler& kMaterialHandler = *(kEntity.component<IMaterialHandler>().get());
        IMaterial* pkMaterial = kMaterialHandler->GetMaterialInstance();
        if (!kRenderHandler->SetMaterial(pkMaterial))
            continue;
        const size_t nActiveCount = kGeometryHandler->GetGeometryCount();
        for (size_t nIndex = 0; nIndex < nActiveCount; nIndex++) {
            kGeometryHandler->ActiveGeometry(nIndex);

            if (!kRenderHandler->SetAttribute(kGeometryHandler->GetVertexBuffer(), kGeometryHandler->GetIndiceBuffer()))
                continue;
            kRenderHandler->SetRenderState(kGeometryHandler->GetRenderState());

            if (!pkMaterial->ApplyCameraData(_pkCamera))
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

            AssignConstant<glm::mat4>(kRenderHandler, "ProjectionMatrix", _rkOrtha, eConstant_Matrix4x4);

            if (!kRenderHandler->ValidatePipeline(_rkRenderer))
                continue;

            _rkRenderer.OnDraw(_rkEventMgr, kRenderHandler);
        }
    }
}

void CGLMDefaultPipeline::OnResize(IRenderer& _rkRenderer, int _nWidth, int _nHeight)
{
    m_spkDefaultBuffer->Resize(_nWidth, _nHeight);
}

void CGLMDefaultPipeline::UpdateOrthoMatrix(int _nWidth, int _nHeight, int _nXOffset, int _nYOffset, bool _bInv)
{
    float nLeft, nRight, nTop, nBottom;
    nLeft = (float)((_nWidth / 2) * -1);
    nLeft += _nXOffset;
    nRight = nLeft + (float)_nWidth;
    nTop = (float)(_nHeight / 2);
    nTop += _nYOffset;
    nBottom = nTop - (float)_nHeight;
    IRenderer* pkRenderer = IRenderSystem::GetRenderer();
    float* pkOrthoMatrix = (float*)&m_kOrthoMatrix;
    nTop = (_bInv) ? -nTop : nTop;
    nBottom = (_bInv) ? -nBottom : nBottom;
    pkRenderer->BuildOrthProjectMatrix(&pkOrthoMatrix, nTop, nBottom, nRight, nLeft, 0.0f, 1.0f);
}