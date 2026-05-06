#pragma once

#include "DeferredRenderPipeline.h"
#include "IInputControlComponent.h"

class ECS_MODELVIEW_API CPhysicalBasedRenderPipeline 
    : public CDeferredRenderPipeline
    , public IInputEventListener
{
    enum
    {
        eSpecularPipeline_Phong = 0x00000001,
        eSpecularPipeline_Blinn = 0x00000002,
        eSpecularPipeline_Cook  = 0x00000004,

        eCookPipeline_Blinn     = 0x00000008,
        eCookPipeline_Beckann   = 0x00000010,
        eCookPipeline_GGX       = 0x00000020,
    };
    char m_nSpecBit;
    char m_nCookBit;
    int m_nPipelineSwitch;
    float m_fRimLightingStep;
    float m_fRimLighting;

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void InitMaterial();

    virtual void UpdateLightUniform(IRenderHandler& _rkRenderComp, ICameraComponent* _pkCameraComp, ICameraComponent* _pkCubeMapCam, ISceneComponent* _pkLightSceneComp, ISceneComponent* _pkCameraSceneComp, ILightComponent* _pkLight);

    void UpdatePBRParam(IRenderHandler& _rkRenderComp, class IMaterialComponent* _pkMaterial);

    virtual void RenderGbuffer(class IRenderer& _rkRenderer);

    virtual void RenderLightBody(class IRenderer& _rkRenderer);

    virtual void RenderBloom(class IRenderer& _rkRenderer, ITexture* pkTexture, bool _bVertical);

    virtual void RenderLight(class IRenderer& _rkRenderer);

    virtual void RenderCubeMap(class IRenderer& _rkRenderer, int _nPass);

    virtual void RenderContrast(class IRenderer& _rkRenderer);

    virtual void RenderScreen(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, class ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha);

    virtual void SetScreenText();

    SCubeMapContext* GetNearestEnvMap(class ISceneComponent* pkSceneComp, float _fRadius);

    virtual void Active(entityx::EventManager& _rkEventMgr);

    virtual void Deactive(entityx::EventManager& _rkEventMgr);

    virtual void receive(const IInputEvent& _rkEvent);
public:
    CPhysicalBasedRenderPipeline(entityx::EventManager& _rkEventMgr, int _nWidth, int nHeight);

    virtual ~CPhysicalBasedRenderPipeline();

    virtual IFrameBuffer* GetFrameBuffer(int _nCurrentPass);

    virtual void SetPassObject(entityx::EntityManager& _rkMgr);

    virtual void OnClear(class IRenderer& _rkRenderer, int _nCurrentPass);

    virtual void PassBegin(class IRenderer& _rkRenderer, int _nCurrent);

    virtual void OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& rkEventMgr, int _nCurrent);

    virtual void PassEnd(class IRenderer& _rkRenderer, int _nCurrent);

    virtual void OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight);

    bool operator()(const TBinTreeNode<3>& _rkSceneNode);

    bool operator()(ISceneComponent* _pkDynamicObject);

    inline void SetRinLightingStep(float _fStep)
    {
        m_fRimLightingStep = _fStep;
        ILogger::Log(eLL_Info, "RinLight:%f\n", m_fRimLighting);
    }

    void UpdateContrastStep()
    {
        CDeferredRenderPipeline::UpdateContrastStep();
        m_fRimLighting += m_fRimLightingStep;
    }
};