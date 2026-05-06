#pragma once

#include "OpenGLRenderSystem.h"
#include "ModelViewPipeline.h"

class ECS_MODELVIEW_API CModelViewRenderSystem : public COpenGLRenderSystem
{
public:
    enum ERenderType
    {
        eRT_Forward,
        eRT_Deferred,
        eRT_PhysicalBaseDeferred,
        eRT_Max,
    };
private:
    std::shared_ptr <CModelViewPipeline> m_spkPipeline[eRT_Max];
public:
    CModelViewRenderSystem(HWND _hWnd, int _nScreenWidth, int _nScreenHeight, float _fScreenDepth, float _fScreenNear, bool _bVsync);

    virtual ~CModelViewRenderSystem();

    void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt);

    void SetRender(ERenderType _eType);

    void SetRenderLightVolume(bool _bEnable);

    void SetEnableBloom(bool _bEnable);

    void SetEnableDrawBinTreeBox(bool _bEnable);

    void SetEnableDrawObjectBox(bool _bEnable);

    void SetTestLight(bool _bEnable);

    void SetUseBinTreeTest(bool _bEnable);

    void ReSizeBackBuffer(int _nWidth, int _nHeight);

    void SetContrastStep(float _fStep);

    void SetBrightnessStep(float _fStep);

    void SetBrightnessThresholdStep(float _fStep);

    void SetBlurWeightStep(float _fStep);

    void SetPBRRimLightingStep(float _fStep);

    void SetDrawDebugNormal(bool _bEnable);
};