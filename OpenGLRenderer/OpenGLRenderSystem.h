#pragma once

#include "RenderSystem.h"
#include "IMaterialComponent.h"
#include "IRenderComponent.h"
#include "IGeometryComponent.h"
#include "DataModifier.h"
#include <map>


class COpenGLRenderSystem;
class ECS_OGL_API IOpenGLRenderListener : public IRenderListener
{
public:
    virtual ~IOpenGLRenderListener()
    {
    }

    void receive(const IRenderEventArg& _rkEventArg);
};

enum ERenderContextType
{
    eRCT_None,
    eRCT_Create,
};

class ECS_OGL_API COpenGLRenderContextArg : public IRenderContextArg
{
    ERenderContextType m_eType;
public:

    COpenGLRenderContextArg(ERenderContextType _eType)
        : m_eType(_eType)
    {
    }

    virtual ~COpenGLRenderContextArg()
    {
    }

    virtual int GetContextType() const
    {
        return m_eType;
    }
};

class ECS_OGL_API COpenGLRenderCreateContextArg : public COpenGLRenderContextArg
{
    static std::string m_kEmpty;
    std::map <std::string, std::string> m_kRenderProperty;
    entityx::Entity& m_rkEntity;
public:
    COpenGLRenderCreateContextArg(entityx::Entity& _rkEntity)
        : COpenGLRenderContextArg(eRCT_Create)
        , m_rkEntity(_rkEntity)
    {
    }

    virtual ~COpenGLRenderCreateContextArg()
    {
    }

    entityx::Entity GetEntity() const
    {
        return m_rkEntity;
    }

    void SetProperty(const std::string& _rkKey, std::string& _rkValue);

    void SetProperty(const char* _pcKey, const char* _pcValue);

    const std::string& GetProperty(std::string _rkKey) const;
};

class ECS_OGL_API COpenGLRenderer : public IRenderer
{
    HDC m_hDeviceContext;
    HGLRC m_hRenderingContext;
    HWND m_hWND;
    GLbitfield m_nClearMask;
    int m_nScreenWidth;
    int m_nScreenHeight;
    float m_fScreenDepth;
    float m_fScreenNear;
    float m_fDegree;
    bool m_bVsync;
public:
    COpenGLRenderer(int _nScreenWidth, int _nScreenHeight, float _fScreenDepth, float _fScreenNear, bool _bVsync);

    virtual ~COpenGLRenderer();

    static bool LoadExtension();

    bool CreateSurface(void* _hWindow);

    void DestroySurface();

    virtual void ReSizeFrameBuffer(IFrameBuffer& _rkFameBuffer, int _nWidth, int _nHeight);

    virtual void ReSizeBackBuffer(int _nWidth, int _nHeight);

    virtual bool PreparePipeline(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, IMaterialHandler& _rkMaterial, IGeometryHandler& _rkGeometry, int _nPass = 0);

    virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderPipeline* pkPipeline, int _nPass = 0);

    virtual void OnDraw(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass = 0);

	virtual void OnDrawOffset(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass = 0, int _nCount = 0, int _nIOffset = 0, int _nVOffset = 0);

    virtual void BeginFrame(IRenderPipeline* pkPipeline, int _nPass);

    virtual void EndFrame(IRenderPipeline* pkPipeline, int _nPass);

    virtual void SetViewPort(int _nTop, int _nBottom, int _nLeft, int _nRight);

	virtual void SetScissor(int _nX, int _nY, int _nW, int _nH);

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
};

class ECS_OGL_API COpenGLRenderSystem : public IRenderSystem
{
    entityx::EntityManager* m_pkEntityMgr;
    entityx::EventManager* m_pkEventMgr;
    HWND m_hWND;
protected:
    std::shared_ptr<IRenderPipeline> m_spkDefaultPipeline;

    virtual void SetRenderPipeline(IRenderPipeline* _pkPipeline);

    virtual void SetSubRenderPipeline(short _nPriority, IRenderPipeline* _pkPipeline);
public:

    COpenGLRenderSystem(HWND _hWnd, int _nScreenWidth, int _nScreenHeight);

    virtual ~COpenGLRenderSystem();

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void ReSizeBackBuffer(int _nWidth, int _nHeight);

    virtual void BeginFrame();

    virtual void EndFrame();

    virtual void SwapBuffer();

    virtual std::shared_ptr <ICameraComponent> OnCreateCamera(IRenderer::ECameraType _eType);

    virtual const char* GetRendererName()
    {
        return "OpenGL";
    }
};