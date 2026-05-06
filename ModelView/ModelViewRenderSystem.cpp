#include "MainPCH.h"
#include "ModelViewRenderSystem.h"
#include "PhysicalBasedRenderPipeline.h"
#include "DeferredRenderPipeline.h"
#include "ForwardRenderPipeline.h"


CModelViewRenderSystem::CModelViewRenderSystem(HWND _hWnd, int _nScreenWidth, int _nScreenHeight, float _fScreenDepth, float _fScreenNear, bool _bVsync)
    : COpenGLRenderSystem(_hWnd, _nScreenWidth, _nScreenHeight)
{
    std::shared_ptr <COpenGLRenderer> spkRenderer = std::make_shared<COpenGLRenderer>(_nScreenWidth, _nScreenHeight, _fScreenDepth, _fScreenNear, _bVsync);
    SetRenderer(spkRenderer);
}

CModelViewRenderSystem::~CModelViewRenderSystem()
{
}

void CModelViewRenderSystem::configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    IRenderer* pkRenderer = GetRenderer();
    int nScreenWidth = pkRenderer->GetScreenWidth();
    int nScreenHeight = pkRenderer->GetScreenHeight();
    COpenGLRenderSystem::configure(_rkEntityMgr, _rkEventMgr);
    m_spkPipeline[eRT_Forward] = std::make_shared <CForwardRenderPipeline>(_rkEventMgr, nScreenWidth, nScreenHeight);
    m_spkPipeline[eRT_Deferred] = std::make_shared <CDeferredRenderPipeline>(_rkEventMgr, nScreenWidth, nScreenHeight);
    m_spkPipeline[eRT_PhysicalBaseDeferred] = std::make_shared <CPhysicalBasedRenderPipeline>(_rkEventMgr, nScreenWidth, nScreenHeight);
    //SetRenderPipeline(m_spkPipeline[eRT_Deferred].get());
}

void CModelViewRenderSystem::update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
{
    COpenGLRenderSystem::update(_rkEntityMgr, _rkEventMgr, dt);
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline[eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->UpdateContrastStep();
}

void CModelViewRenderSystem::SetRender(ERenderType _eType)
{
    //SetRenderPipeline(m_spkPipeline[_eType].get());
    SetSubRenderPipeline(1000, m_spkPipeline[_eType].get());
}

void CModelViewRenderSystem::SetRenderLightVolume(bool _bEnable)
{
    if (!m_spkPipeline)
        return;
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetRenderLightVolume(_bEnable);
}

void CModelViewRenderSystem::SetEnableBloom(bool _bEnable)
{
    if (!m_spkPipeline)
        return;

#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetEnableBloom(_bEnable);
}

void CModelViewRenderSystem::SetEnableDrawBinTreeBox(bool _bEnable)
{
    CModelViewPipeline::SetDrawBinTreeBox(_bEnable);
}

void CModelViewRenderSystem::SetEnableDrawObjectBox(bool _bEnable)
{
    CModelViewPipeline::SetDrawObjectBound(_bEnable);
}

void CModelViewRenderSystem::SetTestLight(bool _bEnable)
{
    if (!m_spkPipeline)
        return;
}

void CModelViewRenderSystem::SetUseBinTreeTest(bool _bEnable)
{
    if (!m_spkPipeline)
        return;

#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetUseBinTreeTest(_bEnable);
}

void CModelViewRenderSystem::ReSizeBackBuffer(int _nWidth, int _nHeight)
{
    IRenderer* pkRenderer = GetRenderer();
    COpenGLRenderSystem::ReSizeBackBuffer(_nWidth, _nHeight);

    for (char nIndex = eRT_Forward; nIndex < eRT_Max; nIndex++) {
        if (m_spkPipeline[nIndex].get() != GetRenderPipeline())
            if (!m_spkPipeline[nIndex]->IsInited())
                continue;
            m_spkPipeline[nIndex]->OnResize(*pkRenderer, _nWidth, _nHeight);
    }
}

void CModelViewRenderSystem::SetContrastStep(float _fStep)
{
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetContrastStep(_fStep);
}

void CModelViewRenderSystem::SetBrightnessStep(float _fStep)
{
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetBrightnessStep(_fStep);
}

void CModelViewRenderSystem::SetBrightnessThresholdStep(float _fStep)
{
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetBrightnessThresholdStep(_fStep);
}

void CModelViewRenderSystem::SetBlurWeightStep(float _fStep)
{
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Deferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Deferred].get());
#endif
    CDeferredRenderPipeline* pkDeferredPipeline = static_cast <CDeferredRenderPipeline*>(pkPipeline);
    pkDeferredPipeline->SetBlurWeightStep(_fStep);
}

void CModelViewRenderSystem::SetPBRRimLightingStep(float _fStep)
{
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_PhysicalBaseDeferred]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_PhysicalBaseDeferred].get());
#endif
    CPhysicalBasedRenderPipeline* pkPBRPipeline = static_cast <CPhysicalBasedRenderPipeline*>(pkPipeline);
    if (!pkPBRPipeline)
        return;

    pkPBRPipeline->SetRinLightingStep(_fStep);
}

void CModelViewRenderSystem::SetDrawDebugNormal(bool _bEnable)
{
#if 0
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(GetRenderPipeline());
    if (!pkPipeline->IsDeferred())
        return;
#else
    if (!m_spkPipeline [eRT_Forward]->IsActived())
        return;
    CModelViewPipeline* pkPipeline = static_cast <CModelViewPipeline*>(m_spkPipeline [eRT_Forward].get());
#endif
    CForwardRenderPipeline* pkForwradPipeline = static_cast <CForwardRenderPipeline*>(pkPipeline);
    pkForwradPipeline->SetDrawDebugNormal(_bEnable);
}