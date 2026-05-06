#include "ECSOpenGLPCH.h"
#include "OpenGLRenderSystem.h"
#include "OpenGLRenderComponent.h"
#include "GLMMainCameraComponent.h"
#include "OpenGLCubeCameraComponent.h"
#include "OpenGLMaterial.h"
#include "GeomertryComponent.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLMeshData.h"
#include "GLMPipeline.h"

#define FRAMEBUFFER_FLUSHMODE 0

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

static LRESULT CALLBACK DummyWndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
    return DefWindowProc(_hWnd, _nMessage, _wParam, _lParam);
}

class ECS_OGL_API COpenGLRenderEventArg : public IRenderEventArg
{
public:
    enum EEventType
    {
        eET_BeginFrame,
        eET_PrepareState,
        eET_PreDraw,
        eET_PostDraw,
        eET_EndFrame,
        eET_Swap,
    };
protected:
    EEventType m_eType;
    const int m_nPass;
public:
    COpenGLRenderEventArg(COpenGLRenderer* _pkRenderer, EEventType _eType, int _nPass);

    virtual ~COpenGLRenderEventArg();

    virtual int GetRenderEventArgType() const;

    virtual int GetRenderPass() const;
};

class ECS_OGL_API COpenGLRenderDrawingEventArg : public COpenGLRenderEventArg
{
    IRenderHandler& m_rkRenderComp;
public:
    COpenGLRenderDrawingEventArg(COpenGLRenderer* _pkRenderer, EEventType _eType, IRenderHandler& _rkRenderComp, int _nPass);

    virtual ~COpenGLRenderDrawingEventArg();

    const IRenderHandler& GetRenderComp() const;
};


COpenGLRenderEventArg::COpenGLRenderEventArg(COpenGLRenderer* _pkRenderer, COpenGLRenderEventArg::EEventType _eType, int _nPass)
    : IRenderEventArg(_pkRenderer)
    , m_eType(_eType)
    , m_nPass(_nPass)
{

}

COpenGLRenderEventArg::~COpenGLRenderEventArg()
{

}

int COpenGLRenderEventArg::GetRenderEventArgType() const
{
    return m_eType;
}

int COpenGLRenderEventArg::GetRenderPass() const
{
    return m_nPass;
}

void IOpenGLRenderListener::receive(const IRenderEventArg& _rkEventArg)
{
    COpenGLRenderer* pkRenderer = dynamic_cast<COpenGLRenderer*>(_rkEventArg.GetRenderer());
    if (!pkRenderer)
        return;

    COpenGLRenderEventArg::EEventType eType = (COpenGLRenderEventArg::EEventType)_rkEventArg.GetRenderEventArgType();
    switch(eType)
    {
        case COpenGLRenderEventArg::eET_BeginFrame:
            BeginFrame(*pkRenderer, _rkEventArg.GetRenderPass());
            break;
        case COpenGLRenderEventArg::eET_PrepareState:
            PrepareState(*pkRenderer, _rkEventArg.GetRenderPass());
            break;
        case COpenGLRenderEventArg::eET_PreDraw:
        {
            //const COpenGLRenderDrawingEventArg& rkRenderArg = static_cast <const COpenGLRenderDrawingEventArg&>(_rkEventArg);
            PreDraw(*pkRenderer, _rkEventArg.GetRenderPass());
        }
            break;
        case COpenGLRenderEventArg::eET_PostDraw:
        {
            //const COpenGLRenderDrawingEventArg& rkRenderArg = static_cast <const COpenGLRenderDrawingEventArg&>(_rkEventArg);
            PostDraw(*pkRenderer, _rkEventArg.GetRenderPass());
        }
            break;
        case COpenGLRenderEventArg::eET_EndFrame:
            EndFrame(*pkRenderer, _rkEventArg.GetRenderPass());
            break;
        case COpenGLRenderEventArg::eET_Swap:
            Swap(*pkRenderer);
            break;
        default:
            return;
    }
}

COpenGLRenderDrawingEventArg::COpenGLRenderDrawingEventArg(COpenGLRenderer* _pkRenderer, COpenGLRenderEventArg::EEventType _eType, IRenderHandler& _rkRenderComp, int _nPass)
    : COpenGLRenderEventArg(_pkRenderer, _eType, _nPass)
    , m_rkRenderComp(_rkRenderComp)
{

}

COpenGLRenderDrawingEventArg::~COpenGLRenderDrawingEventArg()
{

}

const IRenderHandler& COpenGLRenderDrawingEventArg::GetRenderComp() const
{
    return m_rkRenderComp;
}

std::string COpenGLRenderCreateContextArg::m_kEmpty;

void COpenGLRenderCreateContextArg::SetProperty(const std::string& _rkKey, std::string& _rkValue)
{
    m_kRenderProperty [_rkKey] = _rkValue;
}

void COpenGLRenderCreateContextArg::SetProperty(const char* _pcKey, const char* _pcValue)
{
    m_kRenderProperty [_pcKey] = _pcValue;
}

const std::string& COpenGLRenderCreateContextArg::GetProperty(std::string _rkKey) const
{
    auto kIt = m_kRenderProperty.find(_rkKey);
    if (kIt != m_kRenderProperty.end())
        return kIt->second;

    return m_kEmpty;
}

COpenGLRenderer::COpenGLRenderer(int _nScreenWidth, int _nScreenHeight, float _fScreenDepth, float _fScreenNear, bool _bVsync)
    : IRenderer(4, 0)
    , m_hWND(nullptr)
    , m_hDeviceContext(0)
    , m_hRenderingContext(0)
    , m_nScreenWidth(_nScreenWidth)
    , m_nScreenHeight(_nScreenHeight)
    , m_fScreenDepth(_fScreenDepth)
    , m_fScreenNear(_fScreenNear)
    , m_fDegree(70.0f)
    , m_bVsync(_bVsync)
    , m_nClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
{
    COpenGLRenderer::LoadExtension();
}

COpenGLRenderer::~COpenGLRenderer()
{
    DestroySurface();
}

bool COpenGLRenderer::LoadExtension()
{
#ifdef _WIN32
    const LPCWSTR WindowClassName = L"GLExtention";

    // Register a dummy window class.
    static bool bInitializedWindowClass = false;
    if (!bInitializedWindowClass) {
        WNDCLASS wc = {};

        bInitializedWindowClass = true;
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = DummyWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = nullptr;
        wc.hIcon = nullptr;
        wc.hCursor = nullptr;
        wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = WindowClassName;
        ATOM ClassAtom = ::RegisterClass(&wc);
        assert(ClassAtom);
    }

    // Create a dummy window.

    HDC kDeviceContext;
    PIXELFORMATDESCRIPTOR kPixelFormat;
    int nResult;
    HGLRC kRenderContext;
    bool bResult = false;

    HWND hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, WindowClassName, NULL, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);
    assert(hWnd);

    ShowWindow(hWnd, SW_HIDE);

    kDeviceContext = GetDC(hWnd);
    if (!kDeviceContext)
        return false;

    nResult = SetPixelFormat(kDeviceContext, 1, &kPixelFormat);
    if (nResult != 1)
        return false;

    kRenderContext = wglCreateContext(kDeviceContext);
    if (!kRenderContext)
        return false;

    nResult = wglMakeCurrent(kDeviceContext, kRenderContext);
    if (nResult != 1)
        return false;

    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    assert(wglChoosePixelFormatARB);

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    assert(wglCreateContextAttribsARB);

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    assert(wglSwapIntervalEXT);

#if 0
    bool bRet = glewInit() == GLEW_OK;
    assert(bRet);
#endif

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(kRenderContext);

    ReleaseDC(hWnd, kDeviceContext);

    // Release the temporary window now that the extensions have been initialized.
    DestroyWindow(hWnd);
    hWnd = nullptr;
#else
    // Linux not implement
#endif
    return true;
}

bool COpenGLRenderer::CreateSurface(void* _hWindow)
{
    m_hDeviceContext = GetDC((HWND)_hWindow);
    assert(m_hDeviceContext);
    if (m_hDeviceContext == nullptr)
        return false;

    int anAttrList [] =
    {
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,
        0,
    };

    int nPixelFormat;
    unsigned int nFormatCount;
    int nResult = wglChoosePixelFormatARB(m_hDeviceContext, anAttrList, NULL, 1, &nPixelFormat, &nFormatCount);
    assert(nResult == 1);
    if (!nResult)
        return false;

    PIXELFORMATDESCRIPTOR kPixelFormatDescriptor;
    nResult = SetPixelFormat(m_hDeviceContext, nPixelFormat, &kPixelFormatDescriptor);
    assert(nResult == 1);
    if (!nResult)
        return false;

    // Set Version
    int anVersions []
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, GetMajorVersion(),
        WGL_CONTEXT_MINOR_VERSION_ARB, GetMinorVersion(),
        WGL_CONTEXT_FLAGS_ARB,(WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB),
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    m_hRenderingContext = wglCreateContextAttribsARB(m_hDeviceContext, 0, anVersions);
    assert(m_hRenderingContext);
    if (!m_hRenderingContext)
        return false;

    // Set the rendering context to active.
    nResult = wglMakeCurrent(m_hDeviceContext, m_hRenderingContext);
    assert(nResult == 1);
    if (!nResult)
        return false;

    m_hWND = (HWND)_hWindow;

    bool bRet = glewInit() == GLEW_OK;
    assert(bRet);

    return true;
}

void COpenGLRenderer::DestroySurface()
{
#ifdef _WIN32
    wglMakeCurrent(nullptr, nullptr);
    if (m_hRenderingContext) {
        wglDeleteContext(m_hRenderingContext);
        m_hRenderingContext = nullptr;
    }

    if (m_hDeviceContext) {
        ReleaseDC(m_hWND, m_hDeviceContext);
        m_hWND = nullptr;
        m_hDeviceContext = nullptr;
    }
#else
#endif
}

void COpenGLRenderer::ReSizeFrameBuffer(IFrameBuffer& _rkFameBuffer, int _nWidth, int _nHeight)
{
    _rkFameBuffer.Resize(_nWidth, _nHeight);
}

void COpenGLRenderer::ReSizeBackBuffer(int _nWidth, int _nHeight)
{
    m_nScreenWidth = _nWidth;
    m_nScreenHeight = _nHeight;
}

bool COpenGLRenderer::PreparePipeline(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, IMaterialHandler& _rkMaterial, IGeometryHandler& _rkGeometry, int _nPass)
{
    if (!_rkRenderComp->SetRenderState(_rkGeometry->GetRenderState()))
        return false;
#if 0
    if (!_rkRenderComp->SetAttribute(_rkGeometry->GetVertexBuffer(), _rkGeometry->GetIndiceBuffer()))
        return false;
#endif
    if (!_rkRenderComp->SetMaterial(_rkMaterial->GetMaterialInstance()))
        return false;

    COpenGLRenderEventArg kEventArg(this, COpenGLRenderEventArg::eET_PrepareState, _nPass);
    const IRenderEventArg& kArg = kEventArg;
    _rkEventMgr.emit <IRenderEventArg>(kArg);
    return true;
}

void COpenGLRenderer::OnDraw(entityx::EventManager& _rkEventMgr, IRenderPipeline* pkPipeline, int _nPass)
{
    if (pkPipeline) {
        pkPipeline->OnDraw(*this, _rkEventMgr, _nPass);
    }
}

void COpenGLRenderer::OnDraw(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass)
{
	{
		COpenGLRenderDrawingEventArg kEventArg(this, COpenGLRenderEventArg::eET_PreDraw, _rkRenderComp, _nPass);
		const IRenderEventArg& kArg = kEventArg;
		_rkEventMgr.emit <IRenderEventArg>(kArg);
	}

	const IRenderState* pkState = _rkRenderComp->GetRenderState();
	COpenGLVertexBuffer* pkVertexBuffer = dynamic_cast <COpenGLVertexBuffer*>(_rkRenderComp->GetVertexBuffer());
	if (!pkVertexBuffer)
		return;
	COpenGLIndiceBuffer* pkIndicesBuffer = dynamic_cast <COpenGLIndiceBuffer*>(_rkRenderComp->GetIndiceBuffer());
	GLenum ePrimitive = CovToOpenGLPrivitiveType(pkState->GetPrimitiveTopology());
	if (pkIndicesBuffer) {
		glDrawElements(ePrimitive, pkIndicesBuffer->GetSize() / pkIndicesBuffer->GetElementSize(), pkIndicesBuffer->GetIndicesType(), NULL);
	}
	else {
		glDrawArrays(ePrimitive, pkVertexBuffer->GetOffset(), pkVertexBuffer->GetVertexCount());
	}

	{
		COpenGLRenderDrawingEventArg kEventArg(this, COpenGLRenderEventArg::eET_PostDraw, _rkRenderComp, _nPass);
		const IRenderEventArg& kArg = kEventArg;
		_rkEventMgr.emit <IRenderEventArg>(kArg);
	}
}

void COpenGLRenderer::OnDrawOffset(entityx::EventManager& _rkEventMgr, IRenderHandler& _rkRenderComp, int _nPass, int _nCount, int _nIOffset, int _nVOffset)
{
	if (_nCount == 0)
		return;

    {
        COpenGLRenderDrawingEventArg kEventArg(this, COpenGLRenderEventArg::eET_PreDraw, _rkRenderComp, _nPass);
        const IRenderEventArg& kArg = kEventArg;
        _rkEventMgr.emit <IRenderEventArg>(kArg);
    }

    const IRenderState* pkState = _rkRenderComp->GetRenderState();
    COpenGLVertexBuffer* pkVertexBuffer = dynamic_cast <COpenGLVertexBuffer*>(_rkRenderComp->GetVertexBuffer());
    if (!pkVertexBuffer)
        return;
    COpenGLIndiceBuffer* pkIndicesBuffer = dynamic_cast <COpenGLIndiceBuffer*>(_rkRenderComp->GetIndiceBuffer());
    GLenum ePrimitive = CovToOpenGLPrivitiveType(pkState->GetPrimitiveTopology());
    if (pkIndicesBuffer) {
        glDrawElementsBaseVertex(ePrimitive, _nCount, pkIndicesBuffer->GetIndicesType(), (void*)(_nIOffset * pkIndicesBuffer->GetElementSize()), _nVOffset);
    }
    else {
        glDrawArrays(ePrimitive, _nVOffset, _nCount);
    }

    {
        COpenGLRenderDrawingEventArg kEventArg(this, COpenGLRenderEventArg::eET_PostDraw, _rkRenderComp, _nPass);
        const IRenderEventArg& kArg = kEventArg;
        _rkEventMgr.emit <IRenderEventArg>(kArg);
    }
}

void COpenGLRenderer::BeginFrame(IRenderPipeline* pkPipeline, int _nPass)
{
    int nCurrentPass = _nPass;
    if (pkPipeline) {
        IFrameBuffer* pkFrameBuffer = pkPipeline->GetFrameBuffer(nCurrentPass);
        if (pkFrameBuffer) {
            bool bRet = pkFrameBuffer->Active();
            assert(bRet);
        }
        pkPipeline->OnClear(*this, nCurrentPass);
    }
    else {
        SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(m_nClearMask);
    }
}

void COpenGLRenderer::EndFrame(IRenderPipeline* pkPipeline, int _nPass)
{
    if (pkPipeline) {
        IFrameBuffer* pkFrameBuffer = pkPipeline->GetFrameBuffer(_nPass);
        if (pkFrameBuffer)
            pkFrameBuffer->Deactive();
    }
    else {
        SwapBuffers(m_hDeviceContext);
    }
}

void COpenGLRenderer::SetViewPort(int _nTop, int _nBottom, int _nLeft, int _nRight)
{
    int nWidth = _nRight - _nLeft;
    int nHeight = _nBottom - _nTop;
    glViewport(_nLeft, _nBottom, nWidth, nHeight);
}

void COpenGLRenderer::SetScissor(int _nX, int _nY, int _nW, int _nH)
{
	glScissor(_nX, _nY, _nW, _nH);
}

void COpenGLRenderer::SetClearBufferBit(int _nMask)
{
    int nGLMask = 0;
    if (_nMask & IRenderer::eBCB_Color)
        nGLMask |= GL_COLOR_BUFFER_BIT;
    if (_nMask & IRenderer::eBCB_Depth)
        nGLMask |= GL_DEPTH_BUFFER_BIT;
    if (_nMask & IRenderer::eBCB_Stencil)
        nGLMask |= GL_STENCIL_BUFFER_BIT;

    m_nClearMask = nGLMask;
}

void COpenGLRenderer::ClearBuffer()
{
    bool bClearDS = (m_nClearMask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0;
    bool bLastState = COpenGLRenderComponent::m_kCoreState.GetDepthOpMask();
    if (bClearDS) {
        glDepthMask(GL_TRUE);
    }
    glClear(m_nClearMask);
    if (bClearDS) {
        glDepthMask(bLastState);
    }
}

void COpenGLRenderer::SetClearColor(float _fR, float _fG, float _fB, float _fA)
{
    glClearColor(_fR, _fG, _fB, _fA);
}

void COpenGLRenderer::OnSwapBuffers()
{
    SwapBuffers(m_hDeviceContext);
}

void COpenGLRenderer::BuildPerspectiveProjectMatrix(float* _afMatrix [16], float _fNear, float _fFar, int _nWidth, int _nHeight, float _fDegree)
{
    _nWidth = (_nWidth > 0) ? _nWidth : m_nScreenWidth;
    _nHeight = (_nHeight > 0) ? _nHeight : m_nScreenHeight;

    glm::mat4& kMat = (glm::mat4&)(**_afMatrix);
    float fAspect = (float)_nWidth / (float)_nHeight;
    kMat = glm::perspectiveRH(glm::radians((_fDegree == 0.0f) ? m_fDegree : _fDegree), fAspect, _fNear, _fFar);
}

void COpenGLRenderer::BuildOrthProjectMatrix(float* _afMatrix [16], float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar)
{
    glm::mat4& kMat = (glm::mat4&)(**_afMatrix);
    kMat = glm::orthoRH(_fLeft, _fRight, _fBottom, _fTop, _fNear, _fFar);
}

void COpenGLRenderer::BuildViewMatrix(float* _afMatrix [16], float _afLoc [3], float _afDir [3], float _afUp [3])
{
    glm::mat4& kMat = (glm::mat4&)(**_afMatrix);
    glm::vec3& kEye = (glm::vec3&)(*_afLoc);
    glm::vec3& kDir = (glm::vec3&)(*_afDir);
    glm::vec3& kUp = (glm::vec3&)(*_afUp);
    glm::vec3 kAt = kEye - kDir;
    kMat = glm::lookAtRH(kEye, kAt, kUp);
}

void COpenGLRenderer::SetFOV(float _fDegree)
{
    m_fDegree = _fDegree;
}

void COpenGLRenderer::receive(const IRenderContextArg& _rkArg)
{
    const COpenGLRenderContextArg* pkArg = dynamic_cast <const COpenGLRenderContextArg*>(&_rkArg);
    if (!pkArg)
        return;

    ERenderContextType eType = (ERenderContextType)pkArg->GetContextType();
    switch (eType) {
    case eRCT_Create:
    {
        const COpenGLRenderCreateContextArg* pkCreateContext = static_cast <const COpenGLRenderCreateContextArg*>(pkArg);
        if (!pkCreateContext)
            return;
        entityx::Entity kEntity = pkCreateContext->GetEntity();
        std::shared_ptr <COpenGLRenderComponent> spkOpenGLComp;
        if (kEntity.has_component <IRenderHandler>()) {
            entityx::ComponentHandle <IRenderHandler> kComp = kEntity.component <IRenderHandler>();
            IRenderComponent* pkCompIns = kComp->get();
            COpenGLRenderComponent* pkOpenGLComp = dynamic_cast <COpenGLRenderComponent*>(pkCompIns);
            if (!pkOpenGLComp && pkCompIns) {
                kEntity.remove <IRenderHandler>();
            }
        }
        if (!spkOpenGLComp) {
            spkOpenGLComp = std::make_shared<COpenGLRenderComponent>();
            kEntity.assign <IRenderHandler>(spkOpenGLComp);
        }
        {
            const std::string& rkValue = pkCreateContext->GetProperty("Visible");
            if (!rkValue.empty()) {
                spkOpenGLComp->SetRenderable((rkValue == "true"));
            }
        }

        break;
    }
    default:
        break;
    }
}

std::shared_ptr <IRenderState> COpenGLRenderer::OnCreateDefaultRenderState()
{
    return std::make_shared <CRenderState>();
}

std::shared_ptr <IFrameBuffer> COpenGLRenderer::OnCreateFrameBuffer(int _nType, int _nWidth, int _nHeight)
{
    std::shared_ptr <IFrameBuffer> spkFrameBuffer;
    EFrameBufferType eType = (EFrameBufferType)_nType;
    switch (eType) {
    case IRenderer::eFBT_Default:
        spkFrameBuffer = std::make_shared <COpenGLDefaultFrameBuffer>(_nWidth, _nHeight);
        break;
    case IRenderer::eFBT_Genernal:
        spkFrameBuffer = std::make_shared <COpenGLFrameBuffer>(_nWidth, _nHeight);
        break;
    case IRenderer::eFBT_Cube:
        spkFrameBuffer = std::make_shared <COpenGLCubeFrameBuffer>(_nWidth);
        break;
    default:
        break;
    }
    return spkFrameBuffer;
}

std::shared_ptr <ITexture> COpenGLRenderer::OnCreateTexture(int _nTextureType)
{
    ETextureType eType = (ETextureType)_nTextureType;
    GLint nType = 0;
    switch (eType) {
    case IRenderer::eTT_2D:
        nType = GL_TEXTURE_2D;
        break;
    case IRenderer::eTT_3D:
        nType = GL_TEXTURE_3D;
        break;
    case IRenderer::eTT_Cube:
        nType = GL_TEXTURE_CUBE_MAP;
        break;
    default:
        break;
    }

    return std::make_shared <COpenGLTexture>(nType);
}

std::shared_ptr <ICameraComponent> COpenGLRenderer::OnCreateCamera(ECameraType _eType)
{
    std::shared_ptr <ICameraComponent> spkCamera;
    switch (_eType) {
    case IRenderer::eCT_Main:
        spkCamera = std::make_shared<CGLMMainCameraComponent>();
        break;
    case IRenderer::eCT_Cube:
    {
        std::shared_ptr <COpenGLCubeCameraComponent> spkCubeCam = std::make_shared<COpenGLCubeCameraComponent>();
        spkCamera = spkCubeCam;
        break;
    }
    default:
        break;
    }
    return spkCamera;
}

std::shared_ptr <IRenderComponent> COpenGLRenderer::OnCreateDefaultRenderCompoenet()
{
    return std::make_shared <COpenGLRenderComponent>();
}

std::shared_ptr <IMeshBuffer> COpenGLRenderer::OnCreateRenderMeshBuffer()
{
    return std::make_shared <COpenGLMeshBuffer>();
}

COpenGLRenderSystem::COpenGLRenderSystem(HWND _hWnd, int _nScreenWidth, int _nScreenHeight)
    : m_pkEntityMgr(nullptr)
    , m_pkEventMgr(nullptr)
    , m_hWND(_hWnd)
{
    m_spkDefaultPipeline = std::make_shared<CGLMDefaultPipeline>(_nScreenWidth, _nScreenHeight);
}

COpenGLRenderSystem::~COpenGLRenderSystem()
{
}

void COpenGLRenderSystem::configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    m_pkEntityMgr = &_rkEntityMgr;
    m_pkEventMgr = &_rkEventMgr;

    COpenGLRenderer* pkOpenGLRenderer = dynamic_cast <COpenGLRenderer*>(GetRenderer());
    if (pkOpenGLRenderer == nullptr)
        return;

    if (!pkOpenGLRenderer->CreateSurface(m_hWND))
        return;

    // Set the depth buffer to be entirely cleared to 1.0 values.
    glClearDepth(1.0f);

    wglSwapIntervalEXT((pkOpenGLRenderer->IsVsync()) ? 1 : 0);

    _rkEventMgr.subscribe <IRenderContextArg, IRenderSystem>(*this);
    SetRenderPipeline(m_spkDefaultPipeline.get());
}

void COpenGLRenderSystem::ReSizeBackBuffer(int _nWidth, int _nHeight)
{
    IRenderer* pkRenderer = GetRenderer();
    if (!pkRenderer)
        return;

    pkRenderer->ReSizeBackBuffer(_nWidth, _nHeight);

    IRenderPipeline* pkPipeline = GetRenderPipeline();
    if (pkPipeline)
        pkPipeline->OnResize(*pkRenderer, _nWidth, _nHeight);

    if (m_spkDefaultPipeline.get() != pkPipeline)
        m_spkDefaultPipeline->OnResize(*pkRenderer, _nWidth, _nHeight);
}

void COpenGLRenderSystem::BeginFrame()
{
    COpenGLRenderer* pkRenderer = dynamic_cast <COpenGLRenderer*> (GetRenderer());
    if (!pkRenderer)
        return;

    IRenderSystem::BeginFrame();

    if (m_pkEventMgr) {
        COpenGLRenderEventArg kEventArg(pkRenderer, COpenGLRenderEventArg::eET_BeginFrame, GetCurrentPass());
        const IRenderEventArg& kArg = kEventArg;
        m_pkEventMgr->emit <IRenderEventArg>(kArg);
    }
}

void COpenGLRenderSystem::EndFrame()
{
    COpenGLRenderer* pkRenderer = dynamic_cast <COpenGLRenderer*> (GetRenderer());
    if (!pkRenderer)
        return;

    IRenderSystem::EndFrame();

    if (m_pkEventMgr) {
        COpenGLRenderEventArg kEventArg(pkRenderer, COpenGLRenderEventArg::eET_EndFrame, GetCurrentPass());
        const IRenderEventArg& kArg = kEventArg;
        m_pkEventMgr->emit <IRenderEventArg>(kArg);
    }
}

void COpenGLRenderSystem::SwapBuffer()
{
    IRenderSystem::SwapBuffer();
    COpenGLRenderer* pkRenderer = dynamic_cast <COpenGLRenderer*> (GetRenderer());
    if (!pkRenderer)
        return;

    if (m_pkEventMgr) {
        COpenGLRenderEventArg kEventArg(pkRenderer, COpenGLRenderEventArg::eET_Swap, GetCurrentPass());
        const IRenderEventArg& kArg = kEventArg;
        m_pkEventMgr->emit <IRenderEventArg>(kArg);
    }
}

std::shared_ptr <ICameraComponent> COpenGLRenderSystem::OnCreateCamera(IRenderer::ECameraType _eType)
{
    std::shared_ptr<ICameraComponent> spkCam = IRenderSystem::OnCreateCamera(_eType);
#if 0
    if (_eType == IRenderer::eCT_Cube) {
        COpenGLCubeCameraComponent* pkCubeCam = dynamic_cast <COpenGLCubeCameraComponent*>(spkCam.get());
        if (pkCubeCam)
            m_pkEventMgr->subscribe <IRenderEventArg, IRenderListener>(*pkCubeCam);
    }
#endif
    return spkCam;
}

void COpenGLRenderSystem::SetRenderPipeline(IRenderPipeline* _pkPipeline)
{
    COpenGLRenderer* pkRenderer = dynamic_cast<COpenGLRenderer*> (GetRenderer());
    if (!pkRenderer)
        return;

    if (!_pkPipeline->IsInited())
        _pkPipeline->Prepared(pkRenderer->GetScreenWidth(), pkRenderer->GetScreenHeight(), *m_pkEntityMgr, *m_pkEventMgr);
    IRenderPipeline* pkOldPipeline = GetRenderPipeline();
    if (pkOldPipeline)
        pkOldPipeline->Deactive(*m_pkEventMgr);
    IRenderSystem::SetRenderPipeline(_pkPipeline);
    _pkPipeline->Active(*m_pkEventMgr);
}

void COpenGLRenderSystem::SetSubRenderPipeline(short _nPriority, IRenderPipeline* _pkPipeline)
{
    COpenGLRenderer* pkRenderer = dynamic_cast<COpenGLRenderer*> (GetRenderer());
    if (!pkRenderer)
        return;

    if (!_pkPipeline->IsInited())
        _pkPipeline->Prepared(pkRenderer->GetScreenWidth(), pkRenderer->GetScreenHeight(), *m_pkEntityMgr, *m_pkEventMgr);
    IRenderPipeline* pkOldPipeline = GetSubRenderPipeline(_nPriority);
    if (pkOldPipeline) {
        pkOldPipeline->Deactive(*m_pkEventMgr);
        IRenderSystem::RemoveSubRenderPipeline(_nPriority);
    }
    IRenderSystem::SetSubRenderPipeline(_nPriority, _pkPipeline);
    _pkPipeline->Active(*m_pkEventMgr);
}