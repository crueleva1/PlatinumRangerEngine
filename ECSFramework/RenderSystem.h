#pragma once


#include "IRenderComponent.h"
#include "ICameraComponent.h"
#include "IMaterialComponent.h"

struct IBound;
class IMeshBuffer
{
public:
    virtual ~IMeshBuffer()
    {
    }
    // temporary
    virtual void CleanMeshBuffer() = 0;

    virtual std::shared_ptr <IBuffer> CreateVertexBuffer() = 0;
    virtual std::shared_ptr <IBuffer> CreateIndexBuffer() = 0;
    virtual std::shared_ptr <IBound> CreateBoundBox() = 0;

    virtual bool UpdateVertexBuffer(IBuffer* _pkBuffer, bool _bInit = false) = 0;
    virtual bool UpdateIndexBuffer(IBuffer* _pkBuffer, bool _bInit = false) = 0;
    virtual bool UpdateBoundBox(IBound* _pkBound) = 0;

    virtual EPrimitiveType PrimitiveType() = 0;
};

struct ITexture;
class IRenderer;
class IRenderSystem;
class ECS_API IRenderEventArg
{
    IRenderer* m_pkRenderer;
public:
    IRenderEventArg(IRenderer* _pkRenderer)
        : m_pkRenderer(_pkRenderer)
    {
    }

    virtual ~IRenderEventArg()
    {
    }

    virtual int GetRenderEventArgType() const = 0;

    virtual int GetRenderPass() const = 0;

    inline IRenderer* GetRenderer() const
    {
        return m_pkRenderer;
    }
};

class ECS_API IRenderListener : virtual public entityx::Receiver <IRenderListener>
{
public:
    virtual ~IRenderListener()
    {
    }

    virtual void receive(const IRenderEventArg& _rkEventArg) = 0;

    virtual void BeginFrame(IRenderer& _rkRenderer, int _nPass) = 0;

    virtual void PrepareState(IRenderer& _rkRenderer, int _nPass) = 0;

    virtual void EndFrame(IRenderer& _rkRenderer, int _nPass) = 0;

    virtual void PreDraw(IRenderer& _rkRenderer, int _nPass) = 0;

    virtual void PostDraw(IRenderer& _rkRenderer, int _nPass) = 0;

    virtual void Swap(IRenderer& _rkRenderer) = 0;
};

class ECS_API IRenderContextArg
{
public:
    virtual ~IRenderContextArg()
    {
    }

    virtual int GetContextType() const = 0;
};

class ECS_API IFrameBuffer
{
protected:
    int m_nViewportWidth;
    int m_nViewportHeight;
public:
    IFrameBuffer(int _nWidth, int _nHeight)
        : m_nViewportWidth(_nWidth)
        , m_nViewportHeight(_nHeight)
    {

    }

    virtual ~IFrameBuffer()
    {}

    virtual void SetRenderBufferColorAttactmentCount(int _nCount) = 0;

    virtual int GetRenderBufferColorAttachmentCount() = 0;

    virtual bool SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment) = 0;

    virtual bool SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue) = 0;

    virtual ITexture* GetTexture(int _nAttachment) = 0;

    virtual ITexture* GetDepthTexture() = 0;

    virtual bool Active() = 0;

    virtual bool Deactive() = 0;

    virtual void Resize(int _nWidth, int _nHeight) = 0;

    virtual int GetWidth(int _nAttachment) = 0;

    virtual int GetHeight(int _nAttachment) = 0;

    inline int GetViewPortWidth()
    {
        return m_nViewportWidth;
    }

    inline int GetViewPortHeight()
    {
        return m_nViewportHeight;
    }
};

class ECS_API IRenderPipeline
{
    int m_nCount;
    bool m_bActived;
    bool m_bInited;
public:
    IRenderPipeline()
        : m_nCount(0)
        , m_bInited(false)
    {}

    virtual ~IRenderPipeline()
    {}

    void SetRenderPassCount(int _nCount)
    {
        m_nCount = _nCount;
    }

    int GetRenderPassCount()
    {
        return m_nCount;
    }

    bool IsInited()
    {
        return m_bInited;
    }

    inline bool IsActived()
    {
        return m_bActived;
    }

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkMgr, entityx::EventManager& _rkEventMgr)
    {
        m_bInited = true;
    }

    virtual void Active(entityx::EventManager& _rkEventMgr)
    {
        m_bActived = true;
    }

    virtual void Deactive(entityx::EventManager& _rkEventMgr)
    {
        m_bActived = false;
    }

    virtual IFrameBuffer* GetFrameBuffer(int _nCurrentPass) = 0;

    virtual void SetPassObject(entityx::EntityManager& _rkMgr) = 0;

    virtual void OnClear(class IRenderer& _rkRenderer, int _nCurrentPass) = 0;

    virtual void PassBegin(class IRenderer& _rkRenderer, int _nCurrentPass) = 0;

    virtual void OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrentPass) = 0;

    virtual void PassEnd(class IRenderer& _rkRenderer, int _nCurrentPass) = 0;

    virtual void OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight) = 0;
};

class ECS_API IRenderer
{
    short m_nMajorVersion;
    short m_nMinorVersion;
public:
    enum ETextureType
    {
        eTT_2D,
        eTT_3D,
        eTT_Cube,
    };

    enum EFrameBufferType
    {
        eFBT_Default,
        eFBT_Genernal,
        eFBT_Cube,
    };

    enum ECameraType
    {
        eCT_Main,
        eCT_Cube,
    };

    enum EBufferClearBit
    {
        eBCB_Color = 0x01,
        eBCB_Depth = 0x02,
        eBCB_Stencil = 0x04,
        eBCB_All = eBCB_Color | eBCB_Depth | eBCB_Stencil,
    };

    IRenderer(short _nMajorVersion, short _nMinorVersion)
        : m_nMajorVersion(_nMajorVersion)
        , m_nMinorVersion(_nMinorVersion)
    {}

    virtual ~IRenderer()
    {}

    virtual bool CreateSurface(void* _hWindow) = 0;

    virtual void DestroySurface() = 0;

    virtual void ReSizeFrameBuffer(IFrameBuffer& _rkFameBuffer, int _nWidth, int _nHeight) = 0;

    virtual void ReSizeBackBuffer(int _nWidth, int _nHeight) = 0;

    virtual bool PreparePipeline(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, IMaterialHandler& _rkMaterial, IGeometryHandler& _rkGeometry, int _nPass = 0) = 0;

    virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderPipeline* pkPipeline, int _nPass = 0) = 0;

	virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass = 0) = 0;

    virtual void OnDrawOffset(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass = 0, int _nCount = 0, int _nIOffset = 0, int _nVOffset = 0) = 0;

    virtual void BeginFrame(IRenderPipeline* pkPipeline, int _nPass) = 0;

    virtual void EndFrame(IRenderPipeline* pkPipeline, int _nPass) = 0;

    virtual void SetViewPort(int _nTop, int _nBottom, int _nLeft, int _nRight) = 0;

	virtual void SetScissor(int _nX, int _nY, int _nW, int _nH) = 0;

    virtual void SetClearBufferBit(int _nMask) = 0;

    virtual void ClearBuffer() = 0;

    virtual void SetClearColor(float _fR, float _fG, float _fB, float _fA) = 0;

    virtual void OnSwapBuffers() = 0;

    virtual void BuildPerspectiveProjectMatrix(float* _afMatrix [16], float _fNear, float _fFar, int _nWidth = 0, int _nHeight = 0, float _fDegree = 0.0f) = 0;

    virtual void BuildOrthProjectMatrix(float* _afMatrix [16], float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar) = 0;

    virtual void BuildViewMatrix(float* _afMatrix [16], float _afLoc [3], float _afDir [3], float _afUp [3]) = 0;

    virtual void SetFOV(float _fDegree) = 0;

    virtual void receive(const IRenderContextArg&) = 0;

    virtual std::shared_ptr <IRenderState> OnCreateDefaultRenderState() = 0;

    virtual std::shared_ptr <IFrameBuffer> OnCreateFrameBuffer(int _nType, int _nWidth, int _nHeight) = 0;

    virtual std::shared_ptr <ITexture> OnCreateTexture(int _nTextureType) = 0;

    virtual std::shared_ptr <ICameraComponent> OnCreateCamera(ECameraType _nTextureType) = 0;

    virtual std::shared_ptr <IRenderComponent> OnCreateDefaultRenderCompoenet() = 0;

    virtual std::shared_ptr <IMeshBuffer> OnCreateRenderMeshBuffer() = 0;

    virtual int GetScreenWidth() = 0;

    virtual int GetScreenHeight() = 0;

    inline short GetMajorVersion()
    {
        return m_nMajorVersion;
    }

    inline short GetMinorVersion()
    {
        return m_nMinorVersion;
    }
};

class ECS_API IRenderSystem
    : public entityx::System <IRenderSystem>
    , public entityx::Receiver <IRenderSystem>
{
    typedef std::map <short, IRenderPipeline*> CPriorityRenderQueue;
    static std::shared_ptr <IRenderer> m_pkRenderer;
    static IRenderSystem* m_pkRenderSystem;
    CPriorityRenderQueue m_kSubPipelines;
    CPriorityRenderQueue::iterator m_kCurrent;
    IRenderPipeline* m_pkRenderPipeline;
    int m_nCurrentPass;

    bool GetNext()
    {
        ++m_kCurrent;
        return m_kCurrent != m_kSubPipelines.end();
    }

    IRenderPipeline* GetHead()
    {
        if (m_kSubPipelines.empty())
            return m_pkRenderPipeline;
        m_kCurrent = m_kSubPipelines.begin();
        return m_kCurrent->second;
    }
public:

    IRenderSystem()
        :m_pkRenderPipeline(nullptr)
        ,m_nCurrentPass(0)
    {
        assert(m_pkRenderSystem == nullptr);
        m_pkRenderSystem = this;
        m_kCurrent = m_kSubPipelines.end();
    }

    virtual ~IRenderSystem()
    {
    }

    virtual void SetRenderPipeline(IRenderPipeline* _pkPipeline)
    {
        m_pkRenderPipeline = _pkPipeline;
    }

    virtual void SetSubRenderPipeline(short _nPriority, IRenderPipeline* _pkPipeline)
    {
        assert(m_kSubPipelines.find(_nPriority) == m_kSubPipelines.end());
        m_kSubPipelines.insert(std::make_pair(_nPriority, _pkPipeline));
    }

    inline bool RemoveSubRenderPipeline(short _nPriority)
    {
        CPriorityRenderQueue::iterator kIt = m_kSubPipelines.find(_nPriority);
        if (kIt == m_kSubPipelines.end())
            return false;
        m_kSubPipelines.erase(kIt);
        m_kCurrent = m_kSubPipelines.begin();
        return true;
    }

    inline IRenderPipeline* GetSubRenderPipeline(short _nPriority)
    {
        CPriorityRenderQueue::iterator kIt = m_kSubPipelines.find(_nPriority);
        return (kIt != m_kSubPipelines.end()) ? kIt->second : nullptr;
    }

    inline void ResetSubPipeline()
    {
        m_kSubPipelines.clear();
    }

    inline bool IsSubPipelineValid()
    {
        return !m_kSubPipelines.empty();
    }

    inline IRenderPipeline* GetRenderPipeline()
    {
        if (m_kCurrent == m_kSubPipelines.end()) {
            return m_pkRenderPipeline;
        }
        return m_kCurrent->second;
    }

    inline int GetCurrentPass()
    {
        return m_nCurrentPass;
    }
    // If isn't catch return value to other shared_ptr will be auto released.
    inline std::shared_ptr <IRenderer> SetRenderer(std::shared_ptr <IRenderer> spkRenderer)
    {
        std::shared_ptr <IRenderer> spkOld = m_pkRenderer;
        m_pkRenderer = spkRenderer;
        return spkOld;
    }

    virtual void BeginFrame();

    virtual void EndFrame();

    virtual void SwapBuffer();

    virtual void shutdown(entityx::EventManager& events);

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt);

    virtual const char* GetRendererName() = 0;

    virtual std::shared_ptr <ICameraComponent> OnCreateCamera(IRenderer::ECameraType _eType)
    {
        return m_pkRenderer->OnCreateCamera(_eType);
    }

    static IRenderer* GetRenderer()
    {
        return m_pkRenderer.get();
    }

    static IRenderSystem* GetSystem()
    {
        return m_pkRenderSystem;
    }

    void receive(const IRenderContextArg& _rkArg);

    static std::shared_ptr <IRenderState> CreateDefaultRenderState()
    {
        return m_pkRenderer->OnCreateDefaultRenderState();
    }

    static std::shared_ptr <IFrameBuffer> CreateFrameBuffer(int _nType, int _nWidth, int _nHeight)
    {
        return m_pkRenderer->OnCreateFrameBuffer(_nType, _nWidth, _nHeight);
    }

    static std::shared_ptr <ITexture> CreateTexture(int _nTextureType)
    {
        return m_pkRenderer->OnCreateTexture(_nTextureType);
    }

    static std::shared_ptr <ICameraComponent> CreateCamera(IRenderer::ECameraType _eType)
    {
        // Because this flow will be used by ECS Event System.
        return m_pkRenderSystem->OnCreateCamera(_eType);
    }

    static std::shared_ptr <IRenderComponent> CreateDefaultRenderCompoenet()
    {
        return m_pkRenderer->OnCreateDefaultRenderCompoenet();
    }

    static std::shared_ptr <IMeshBuffer> CreateRenderMeshBuffer()
    {
        return m_pkRenderer->OnCreateRenderMeshBuffer();
    }
};