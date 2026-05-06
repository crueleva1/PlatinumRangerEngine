#pragma once

#include "ModelViewPipeline.h"
#include "RenderSystem.h"
#include "ILightComponent.h"
#include "ModelViewSceneSystem.h"

class ECS_MODELVIEW_API CDeferredRenderPipeline : public CModelViewPipeline
{
protected:
    enum PassType
    {
        ePT_GBuffer,
        ePT_LightPass,
        ePT_Emissive,
        ePT_ContrastProc,
        ePT_BlurV,
        ePT_BlurH,
        ePT_BlurV2,
        ePT_BlurH2,
        ePT_RenderScreen,
        ePT_Max,
    };
    struct SCubeMapContext
    {
        std::vector <entityx::Entity> m_kTestedObject[6];
        entityx::Entity m_kEntity;
        unsigned int m_nRevision;
        bool m_bHasRefObject;
        bool operator()(const TBinTreeNode<3>& _rkSceneNode);

        SCubeMapContext()
            :m_nRevision(-1)
            ,m_bHasRefObject(false)
        { }
    };
    ILightHandler m_kTemplateLight;
    std::vector <SCubeMapContext> m_kEnvCubeEntity;
    std::shared_ptr <IFrameBuffer> m_spkGBuffer;
    std::shared_ptr <IFrameBuffer> m_spkLightBuffer;
    std::shared_ptr <IFrameBuffer> m_spkContrastPassBuffer;
    std::shared_ptr <IFrameBuffer> m_spkPostProcessBuffer[2];
    std::shared_ptr <IFrameBuffer> m_spkSecondPostProcessBuffer [2];
    std::shared_ptr <IBuffer> m_spkTestMeshInstanceVB;
    std::shared_ptr <IBuffer> m_spkTestMeshInstanceIB;
    std::shared_ptr <IBuffer> m_spkScreenRectVB;
    std::shared_ptr <IBuffer> m_spkScreenRectIB;
    std::shared_ptr <struct IRenderState> m_spkRenderLightState;
    std::shared_ptr <struct IRenderState> m_spkRenderLightVolumeState;
    std::shared_ptr <struct IRenderState> m_spkRenderLightBodyState;
    std::shared_ptr <struct IRenderState> m_spkPostProcessState;
    std::shared_ptr <struct IRenderState> m_spkDefaultState;
    std::shared_ptr <ITexture> m_spkBlurTexture [2];    // Vertical 0, Horizon 1;
    std::shared_ptr <ITexture> m_spkSecondBlurTexture [2];
    std::shared_ptr <ITexture> m_spkContrastTexture;
    IMaterial* m_pkGBufferMaterial;
    IMaterial* m_pkDeferredShadingMaterial;
    IMaterial* m_pkBasicTexturedMaterial;
    IMaterial* m_pkWriteDepthMaterial;
    IMaterial* m_pkLightBodyMaterial;
    IMaterial* m_pkBloomMaterial;
    IMaterial* m_pkContrastMaterial;
    IMaterial* m_pkScreenSpaceMaterial;
    class CGLMBaseCameraComponent* m_pkMainCamera;
    int m_nScreenWidth;
    int m_nScreenHeight;
    static float m_fBrightness;
    static float m_fContrast;
    static float m_fBrightnessThreshold;
    static float m_fBlurWeight;
    static float m_fBrightnessStep;
    static float m_fContrastStep;
    static float m_fBrightnessThresholdStep;
    static float m_fBlurWeightStep;
    static bool m_bRenderLightVolume;
    static bool m_bEnableBloom;
    static bool m_bUseBinTreeTest;

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void SetScreenText();

    virtual void InitMaterial();

    virtual void UpdateLightUniform(IRenderHandler& _rkRenderComp, ICameraComponent* _pkCameraComp, ICameraComponent* _pkCubeMapCam, ISceneComponent* _pkLightSceneComp, ISceneComponent* _pkCameraSceneComp, ILightComponent* _pkLight);

    virtual void RenderGbuffer(class IRenderer& _rkRenderer);

    virtual void RenderLightBody(class IRenderer& _rkRenderer);

    virtual void RenderBloom(class IRenderer& _rkRenderer, ITexture* pkTexture, bool _bVertical);

    virtual void RenderLight(class IRenderer& _rkRenderer);

    virtual void RenderCubeMap(class IRenderer& _rkRenderer, int _nPass);

    virtual void RenderContrast(class IRenderer& _rkRenderer);

    virtual void RenderScreen(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, class ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha);
public:
    CDeferredRenderPipeline(entityx::EventManager& _rkEventMgr, int _nWidth, int nHeight);

    virtual ~CDeferredRenderPipeline();

    virtual IFrameBuffer* GetFrameBuffer(int _nCurrentPass);

    virtual void SetPassObject(entityx::EntityManager& _rkMgr);

    virtual void OnClear(class IRenderer& _rkRenderer, int _nCurrentPass);

    virtual void PassBegin(class IRenderer& _rkRenderer, int _nCurrent);

    virtual void OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& rkEventMgr, int _nCurrent);

    virtual void PassEnd(class IRenderer& _rkRenderer, int _nCurrent);

    virtual void OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight);

    virtual bool operator()(const TBinTreeNode<3>& _rkSceneNode);

    virtual bool operator()(ISceneComponent* _pkDynamicObject);

    inline void SetRenderLightVolume(bool _bEnable)
    {
        m_bRenderLightVolume = _bEnable;
    }

    inline void SetEnableBloom(bool _bEnable)
    {
        m_bEnableBloom = _bEnable;
    }

    inline void SetUseBinTreeTest(bool _bEnable)
    {
        m_bUseBinTreeTest = _bEnable;
    }

    inline void SetContrastStep(float _fStep)
    {
        m_fContrastStep = _fStep;
        ILogger::Log(eLL_Info, "Contrast : %f\n", m_fContrast);
    }

    inline void SetBrightnessStep(float _fStep)
    {
        m_fBrightnessStep = _fStep;
        ILogger::Log(eLL_Info, "Brightness : %f\n", m_fBrightness);
    }

    inline void SetBrightnessThresholdStep(float _fStep)
    {
        m_fBrightnessThresholdStep = _fStep;
        ILogger::Log(eLL_Info, "BrightnessThreshold : %f\n", m_fBrightnessThreshold);
    }

    inline void SetBlurWeightStep(float _fStep)
    {
        m_fBlurWeightStep = _fStep;
        ILogger::Log(eLL_Info, "BlurWeight : %f\n", m_fBlurWeight);
    }

    virtual void UpdateContrastStep()
    {
        m_fBrightness += m_fBrightnessStep;
        m_fContrast += m_fContrastStep;
        m_fBrightnessThreshold += m_fBrightnessThresholdStep;
        m_fBlurWeight += m_fBlurWeightStep;
    }

    virtual bool IsDeferred()
    {
        return true;
    }
};