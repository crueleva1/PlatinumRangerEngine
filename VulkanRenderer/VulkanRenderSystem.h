#pragma once

#include "RenderSystem.h"
#include "IMaterialComponent.h"
#include "IRenderComponent.h"
#include "IGeometryComponent.h"
#include "DataModifier.h"
#include "VulkanSampler.h"
#include "VulkanResource.h"
#include <map>

class CVulkanRenderer : public IRenderer
{
    std::queue <std::shared_ptr <CVulkanCmdBufferResource> > m_kReleaseQueue;
    VkPhysicalDeviceLimits m_kLimit;
    VkPhysicalDeviceMemoryProperties m_kDeviceMemoryProperties;

    std::unique_ptr<CVulkanImporter> m_spkLibImporter;
    static std::shared_ptr <CVulkanSampler> ms_spkGlobalSampler;
    entityx::EntityManager* m_pkEntityMgr;
    entityx::EventManager* m_pkEventMgr;
    HDC m_hDeviceContext;
    HGLRC m_hRenderingContext;
    HWND m_hWND;
    VkDevice m_kDevice;
    VkInstance m_kInstance;
    VkPipelineCache m_kPipelineCache;
    int m_nClearMask;
    int m_nScreenWidth;
    int m_nScreenHeight;
    float m_fScreenDepth;
    float m_fScreenNear;
    float m_fDegree;
    bool m_bVsync;
    bool m_bValidationLayer;
public:
    enum ECommandBufferType
    {
        eCBT_Resource,
        eCBT_Draw,
    };

    CVulkanRenderer(HWND _hWnd, int _nScreenWidth, int _nScreenHeight, float _fScreenDepth, float _fScreenNear, bool _bVsync, bool _bValidationLayer);

    virtual ~CVulkanRenderer();

    bool CreateSurface(void* _hWindow);

    void DestroySurface();

    virtual void ReSizeFrameBuffer(IFrameBuffer& _rkFameBuffer, int _nWidth, int _nHeight);

    virtual void ReSizeBackBuffer(int _nWidth, int _nHeight);

    virtual bool PreparePipeline(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, IMaterialHandler& _rkMaterial, IGeometryHandler& _rkGeometry, int _nPass = 0);

    virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderPipeline* pkPipeline, int _nPass = 0);

    virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass = 0);

    virtual void BeginFrame(IRenderPipeline* pkPipeline, int _nPass);

    virtual void EndFrame(IRenderPipeline* pkPipeline, int _nPass);

    virtual void SetViewPort(int _nTop, int _nBottom, int _nLeft, int _nRight);

    virtual void SetClearBufferBit(int _nMask);

    virtual void ClearBuffer();

    virtual void SetClearColor(float _fR, float _fG, float _fB, float _fA);

    virtual void OnSwapBuffers();

    virtual void BuildPerspectiveProjectMatrix(float* _afMatrix [16], float _fNear, float _fFar, int _nWidth = 0, int _nHeight = 0, float _fDegree = 0.0f);

    virtual void BuildOrthProjectMatrix(float* _afMatrix [16], float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar);

    virtual void BuildViewMatrix(float* _afMatrix [16], float _afLoc [3], float _afDir [3], float _afUp [3]);

    virtual void SetFOV(float _fDegree);

    virtual void receive(const IRenderContextArg&);

    virtual std::shared_ptr <IRenderState> OnCreateDefaultRenderState();

    virtual std::shared_ptr <IFrameBuffer> OnCreateFrameBuffer(int _nType, int _nWidth, int _nHeight);

    virtual std::shared_ptr <ITexture> OnCreateTexture(int _nTextureType);

    virtual std::shared_ptr <ICameraComponent> OnCreateCamera(ECameraType _nTextureType);

    virtual std::shared_ptr <IRenderComponent> OnCreateDefaultRenderCompoenet();

    virtual std::shared_ptr <IMeshBuffer> OnCreateRenderMeshBuffer();

    int GetScreenWidth()
    {
        return m_nScreenWidth;
    }

    int GetScreenHeight()
    {
        return m_nScreenHeight;
    }

    inline float GetFar()
    {
        return m_fScreenDepth;
    }

    inline float GetNear()
    {
        return m_fScreenNear;
    }

    inline float GetAspective()
    {
        return m_fDegree;
    }

    inline bool IsVsync()
    {
        return m_bVsync;
    }

    inline VkDevice GetVkDevice()
    {
        return m_kDevice;
    }

    inline VkInstance GetVkInstance()
    {
        return m_kInstance;
    }

    inline const VkPhysicalDeviceLimits& GetDeviceSpec()
    {
        return m_kLimit;
    }

    inline const VkPhysicalDeviceMemoryProperties& GetDeviceMemoryProp()
    {
        return m_kDeviceMemoryProperties;
    }

    inline CVulkanSampler* GetGlobalSampler()
    {
        return ms_spkGlobalSampler.get();
    }

    inline VkPipelineCache GetPipelineCache()
    {
        return m_kPipelineCache;
    }

    virtual void ReleaseResource(std::shared_ptr <CVulkanCmdBufferResource> spkResource);

    CVulkanCommandBuffer& GetActiveCmdBuffer(ECommandBufferType _eCmdBufferType);
};

class CVulkanRenderSystem : public IRenderSystem
{
    entityx::EntityManager* m_pkEntityMgr;
    entityx::EventManager* m_pkEventMgr;
    HWND m_hWND;
protected:
    std::shared_ptr<IRenderPipeline> m_spkDefaultPipeline;

    virtual void OnDraw(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);
public:


    CVulkanRenderSystem(HWND _hWnd, int _nScreenWidth, int _nScreenHeight);

    virtual ~CVulkanRenderSystem();

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp);

    virtual bool PreparePipeline(IRenderHandler& _rkRenderComp, IMaterialHandler& _rkMaterial, IGeometryHandler& _rkGeometry);

    virtual void BeginFrame();

    virtual void EndFrame();

    virtual const char* GetRendererName()
    {
        return "Vulkan";
    }
};