#include "MainPCH.h"
#include "IApplication.h"
#include "ILanucherConfig.h"
#include "ModelViewRenderSystem.h"
#include "OpenGLMaterialSystem.h"
#include "Win32InputSystem.h"
#include "DefaultResourceSystem.h"
#include "ModelViewSpawnSystem.h"
#include "ModelViewSceneSystem.h"
#include "ModelViewMovementSystem.h"
#include "ModelViewTextSystem.h"
#include "NanoLoggerSystem.h"
#include "EventSystem.h"
#include "ImguiSystem.h"
#include "ShareModule.h"
#include "ModelViewImgui.h"

#include "Delegate.h"

#include <stdio.h>
#ifdef WIN32
#include <WinSock2.h>
#include <time.h>
int gettimeofday(struct timeval* _pkTimeval, void* _pvTimezone)
{
    time_t kTime;
    struct tm kTm;
    SYSTEMTIME kSystemtime;
    GetLocalTime(&kSystemtime);
    kTm.tm_year = kSystemtime.wYear - 1900;
    kTm.tm_mon = kSystemtime.wMonth - 1;
    kTm.tm_mday = kSystemtime.wDay;
    kTm.tm_hour = kSystemtime.wHour;
    kTm.tm_min = kSystemtime.wMinute;
    kTm.tm_sec = kSystemtime.wSecond;
    kTm.tm_isdst = -1;
    kTime = mktime(&kTm);
    _pkTimeval->tv_sec = kTime;
    _pkTimeval->tv_usec = kSystemtime.wMilliseconds * 1000;
    return 0;
}
#else
#include <sys/time.h>
#include <unistd.h>
#endif

bool g_bFullScreen = false;

class ECS_MODELVIEW_API CApplication 
    : public IApplication
    , public entityx::EntityX
    , public IInputEventListener
    , public IOpenGLRenderListener
{
    // Imgui Editor
    std::shared_ptr <CModelViewImgui> m_spkImguiMgr;

    std::shared_ptr<CEventSystem> m_spkEventSystem;
    std::shared_ptr<CModelViewRenderSystem> m_spkModelViewRenderSystem;
    std::shared_ptr<COpenGLMaterialSystem> m_spkOpenGLMaterialSystem;
    std::shared_ptr<CModelViewSpawnSystem> m_spkModelViewSpawnSystem;
    std::shared_ptr<CModelViewTextSystem> m_spkModelViewTextSystem;
    std::shared_ptr<CWin32InputSystem> m_spkWin32InputSystem;
    std::shared_ptr<CModelViewMovementSystem> m_spkModelViewMovementSystem;
    std::shared_ptr<CModelViewSceneSystem> m_spkModelViewSceneSystem;
    std::shared_ptr<CImguiSystem> m_spkImguiSystem;
    std::shared_ptr<CDefaultResourceSystem> m_spkDefaultResourceSystem;
    std::shared_ptr<CNanoLoggerSystem> m_spkNanoLoggerSystem;

    static CApplication* m_pkThis;
    LPCWSTR m_kApplicationName;
    HINSTANCE m_hInstance;
    HWND m_hWnd;
    entityx::Entity m_kFPSTextEntity;
    double m_nCurrentDelta;
    int m_nFrameCount;
    double m_nSecDuration;
    int m_nDrawCount;
    bool m_bRuning;
public:
    explicit CApplication()
        : m_bRuning(false)
        , m_hWnd(0)
        , m_hInstance(0)
        , m_nCurrentDelta(0)
        , m_nFrameCount(0)
        , m_nSecDuration(0)
    {
        m_pkThis = this;
        m_spkImguiMgr = std::make_shared<CModelViewImgui>();
    }

    virtual ~CApplication()
    {
        events.unsubscribe<IInputEvent, IInputEventListener>(*m_spkImguiMgr);
        events.unsubscribe<IInputEvent, IInputEventListener>(*this);
        events.unsubscribe<IRenderEventArg, IRenderListener>(*this);

        m_pkThis = nullptr;

        // Fix the display settings if leaving full screen mode.
        if (g_bFullScreen) {
            ChangeDisplaySettings(nullptr, 0);
        }

        // Remove the window.
        DestroyWindow(m_hWnd);
        // Remove the application instance.
        UnregisterClass(m_kApplicationName, m_hInstance);
    }

    virtual void Initialize(const ILanucherConfig& _rkConfig)
    {
        std::string kWorkDir = _rkConfig.GetValueByString("WorkDir");
        if (!kWorkDir.empty())
            SetCurrentDirectoryA(kWorkDir.c_str());

        // Query Config
        int nScreenWidth = _rkConfig.GetValueByInt("ScreenWidth", 1920);
        int nScreenHeight = _rkConfig.GetValueByInt("ScreenHeight", 1080);

        float fScreenDepth = _rkConfig.GetValueByFloat("ScreenDepth", 0.1);
        float fScreenNear = _rkConfig.GetValueByFloat("ScreenNear", 2000.0f);
        bool bVsync = _rkConfig.GetValueByBool("Vsync", false);

        WNDCLASSEX kWinClass;
        ZeroMemory(&kWinClass, sizeof(WNDCLASSEX));
        DEVMODE kDevModeScreenSettings;
        int nPosX, nPosY;
        bool bResult;

        m_hInstance = GetModuleHandle(nullptr);
        m_kApplicationName = L"ModelView";
        kWinClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        kWinClass.lpfnWndProc = CApplication::WndProc;
        kWinClass.cbClsExtra = 0;
        kWinClass.cbWndExtra = 0;
        kWinClass.hInstance = m_hInstance;
        kWinClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
        kWinClass.hIconSm = kWinClass.hIcon;
        kWinClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        kWinClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
        kWinClass.lpszMenuName = nullptr;
        kWinClass.lpszClassName = m_kApplicationName;
        kWinClass.cbSize = sizeof(WNDCLASSEX);

        RegisterClassEx(&kWinClass);

        int nWinStyle = WS_POPUP;
        if (g_bFullScreen) {
            // Determine the resolution of the clients desktop screen.
            nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
            nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

            memset(&kDevModeScreenSettings, 0, sizeof(kDevModeScreenSettings));
            kDevModeScreenSettings.dmSize = sizeof(kDevModeScreenSettings);
            kDevModeScreenSettings.dmPelsWidth = (unsigned long)nScreenWidth;
            kDevModeScreenSettings.dmPelsHeight = (unsigned long)nScreenHeight;
            kDevModeScreenSettings.dmBitsPerPel = 32;
            kDevModeScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            ChangeDisplaySettings(&kDevModeScreenSettings, CDS_FULLSCREEN);

            nPosX = nPosY = 0;
        }
        else {
            nWinStyle = WS_OVERLAPPEDWINDOW;

            // Place the window in the middle of the screen.
            nPosX = (GetSystemMetrics(SM_CXSCREEN) - nScreenWidth) / 2;
            nPosY = (GetSystemMetrics(SM_CYSCREEN) - nScreenHeight) / 2;
        }
        // Create the window with the screen settings and get the handle to it.
        m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_kApplicationName, m_kApplicationName, nWinStyle,
                                nPosX, nPosY, nScreenWidth, nScreenHeight, nullptr, nullptr, m_hInstance, nullptr);
        assert(m_hWnd);

        // init seed
        timeval kTimeval;
        gettimeofday(&kTimeval, nullptr);
        srand(kTimeval.tv_usec);
        
        m_spkEventSystem = systems.add<CEventSystem>(4, eED_Async);
        m_spkModelViewRenderSystem = systems.add<CModelViewRenderSystem>(m_hWnd, nScreenWidth, nScreenHeight, fScreenDepth, fScreenNear, bVsync);
        m_spkOpenGLMaterialSystem = systems.add<COpenGLMaterialSystem>();
        m_spkModelViewSpawnSystem = systems.add<CModelViewSpawnSystem>("init.xml", "data", "0.1");
        m_spkModelViewTextSystem = systems.add<CModelViewTextSystem>(*systems.system<COpenGLMaterialSystem>());
        m_spkWin32InputSystem = systems.add<CWin32InputSystem>();
        m_spkModelViewMovementSystem = systems.add<CModelViewMovementSystem>();
        m_spkModelViewSceneSystem = systems.add<CModelViewSceneSystem>();
        m_spkImguiSystem = systems.add<CImguiSystem>(m_spkImguiMgr.get(), m_hWnd);
        m_spkDefaultResourceSystem = systems.add<CDefaultResourceSystem>();
        m_spkNanoLoggerSystem = systems.add<CNanoLoggerSystem>();
        systems.configure();

        events.subscribe<IInputEvent, IInputEventListener>(*this);
        events.subscribe<IRenderEventArg, IRenderListener>(*this);

        ShowWindow(m_hWnd, SW_SHOW);
        SetForegroundWindow(m_hWnd);
        SetFocus(m_hWnd);

        ShowCursor(true);

        std::shared_ptr <CModelViewRenderSystem> spkRenderSystem = systems.system <CModelViewRenderSystem>();
        spkRenderSystem->SetRender(CModelViewRenderSystem::eRT_Deferred);
        IRenderer* pkRenderer = IRenderSystem::GetRenderer();
#if 1
        std::shared_ptr<CImguiSystem> spkImguiSystem = systems.system<CImguiSystem>();
        if (spkImguiSystem) {
            events.subscribe<IInputEvent, IInputEventListener>(*m_spkImguiMgr);
            spkImguiSystem->ConfigRenderer(*IRenderSystem::GetSystem());
        }
#endif

        m_kFPSTextEntity = entities.create();
        {
            CCreateTextCompContext kTextContext(m_kFPSTextEntity, "monaco-20", "FPS:");
            const ITextContext& rkIContext = kTextContext;
            events.emit <ITextContext>(rkIContext);
        }
        {
            CSceneCreateContext kContext(m_kFPSTextEntity);
            std::string kPos = std::to_string(pkRenderer->GetScreenWidth() - 300);
            kPos += ",0.0,0.0";
            kContext.SetProperty("Position", kPos.c_str());
            kContext.SetProperty("ScreenSpace", "true");
            const ISceneContext& rkIContext = kContext;
            events.emit <ISceneContext>(rkIContext);
        }
        {
            COpenGLRenderCreateContextArg kArgs(m_kFPSTextEntity);
            kArgs.SetProperty("Visible", "true");
            const IRenderContextArg& rkIArg = kArgs;
            events.emit <IRenderContextArg>(rkIArg);
        }
    }

    double GetTime(timeval& _kTimeval)
    {
        double nSec = _kTimeval.tv_sec;
        double nMSec = _kTimeval.tv_usec;
        return nSec + ((double)_kTimeval.tv_usec / 1000000.0);
    }

    void Run()
    {
        m_bRuning = true;
        MSG kMsg;
        ZeroMemory(&kMsg, sizeof(MSG));
        timeval kTimeval;
        double nCurrentTime = 0;
        double nDelta = 0;
        while (m_bRuning) {
            if (PeekMessage(&kMsg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&kMsg);
                DispatchMessage(&kMsg);
            }

            if (kMsg.message == WM_QUIT) {
                m_bRuning = false;
                break;
            }

            gettimeofday(&kTimeval, nullptr);
            if (!nCurrentTime) {
                nCurrentTime = GetTime(kTimeval);
                continue;
            }
            nDelta = GetTime(kTimeval) - nCurrentTime;
            update(nDelta);
            nCurrentTime += nDelta;
            m_nCurrentDelta = nDelta;
        }
    }

    int Return()
    {
        systems.shutdown();
        return 0;
    }

    void update(entityx::TimeDelta dt)
    {
#if 0
        m_spkEventSystem->update(entities, events, dt);
        m_spkDefaultResourceSystem->update(entities, events, dt);
        m_spkOpenGLMaterialSystem->update(entities, events, dt);
        m_spkModelViewSpawnSystem->update(entities, events, dt);
        m_spkModelViewTextSystem->update(entities, events, dt);
        m_spkWin32InputSystem->update(entities, events, dt);
        m_spkModelViewMovementSystem->update(entities, events, dt);
        m_spkModelViewSceneSystem->update(entities, events, dt);
        m_spkImguiSystem->update(entities, events, dt);
        m_spkModelViewRenderSystem->update(entities, events, dt);
        m_spkNanoLoggerSystem->update(entities, events, dt);
#else
        systems.update_all(dt);
#endif

    }

    static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
    {
        switch (0)
        {
        default:
            if (!m_pkThis)
                break;
            if (!m_pkThis->m_bRuning)
                break;
            std::shared_ptr <CWin32InputSystem> spkInputSystem = m_pkThis->systems.system <CWin32InputSystem>();
            if (spkInputSystem == nullptr)
                break;
            return spkInputSystem->WndProc(_hWnd, _nMessage, _wParam, _lParam);
        }
        return DefWindowProc(_hWnd, _nMessage, _wParam, _lParam);
    }

    virtual void receive(const IInputEvent& _rkEvent)
    {
        if (_rkEvent.GetInputType() == eIET_App) {
            const CAppInputEvent& rkAppInput = static_cast <const CAppInputEvent&>(_rkEvent);
            switch (rkAppInput.GetEventType())
            {
                case CAppInputEvent::eAIE_Quit:
                    PostQuitMessage(0);
                    break;
                case CAppInputEvent::eAIE_ResizeWindow:
                {
                    int nWidth = LOWORD(rkAppInput.GetLParam());
                    int nHeight = HIWORD(rkAppInput.GetLParam());
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer && nWidth && nHeight) {
                        spkRenderer->ReSizeBackBuffer(nWidth, nHeight);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else if (_rkEvent.GetInputType() == eIET_Keyboard) {
            const CKeyboardEvent& rkKeyBoard = static_cast <const CKeyboardEvent&>(_rkEvent);
            switch (rkKeyBoard.GetKey())
            {
                case CKeyboardEvent::eKBC_O:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetRender(CModelViewRenderSystem::eRT_Forward);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_P:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetRender(CModelViewRenderSystem::eRT_Deferred);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_LeftBracket:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem>();
                    if (spkRenderer) {
                        spkRenderer->SetRender(CModelViewRenderSystem::eRT_PhysicalBaseDeferred);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_RightBracket:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem>();
                    if (spkRenderer) {
                        static bool gs_bToggleDrawNormal = false;
                        gs_bToggleDrawNormal = !gs_bToggleDrawNormal;
                        spkRenderer->SetDrawDebugNormal(gs_bToggleDrawNormal);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_L:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    static bool gs_bToggleLightVolume = false;
                    gs_bToggleLightVolume = !gs_bToggleLightVolume;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetRenderLightVolume(gs_bToggleLightVolume);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_I:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetBrightnessStep((rkKeyBoard.IsDown()) ? 0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_K:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetBrightnessStep((rkKeyBoard.IsDown()) ? -0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_U:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetContrastStep((rkKeyBoard.IsDown()) ? 0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_J:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetContrastStep((rkKeyBoard.IsDown()) ? -0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_Y:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetBrightnessThresholdStep((rkKeyBoard.IsDown()) ? 0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_H:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetBrightnessThresholdStep ((rkKeyBoard.IsDown()) ? -0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_T:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetBlurWeightStep((rkKeyBoard.IsDown()) ? 0.0001f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_G:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetBlurWeightStep((rkKeyBoard.IsDown()) ? -0.0001f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_M:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        static bool gs_bToggleBloom = true;
                        gs_bToggleBloom = !gs_bToggleBloom;
                        spkRenderer->SetEnableBloom(gs_bToggleBloom);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_N:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem>();
                    if (spkRenderer) {
                        static bool gs_bToggleDrawBinTree = false;
                        gs_bToggleDrawBinTree = !gs_bToggleDrawBinTree;
                        spkRenderer->SetEnableDrawBinTreeBox(gs_bToggleDrawBinTree);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_B:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem>();
                    if (spkRenderer) {
                        static bool gs_bToggleUseBinTree = false;
                        gs_bToggleUseBinTree = !gs_bToggleUseBinTree;
                        spkRenderer->SetUseBinTreeTest(gs_bToggleUseBinTree);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_V:
                {
                    if (!rkKeyBoard.IsDown())
                        return;
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem>();
                    if (spkRenderer) {
                        static bool gs_bToggleDrawObjectBound = false;
                        gs_bToggleDrawObjectBound = !gs_bToggleDrawObjectBound;
                        spkRenderer->SetEnableDrawObjectBox(gs_bToggleDrawObjectBound);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_R:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetPBRRimLightingStep((rkKeyBoard.IsDown()) ? 0.01f : 0.0f);
                    }
                    break;
                }
                case CKeyboardEvent::eKBC_F:
                {
                    std::shared_ptr <CModelViewRenderSystem> spkRenderer = systems.system <CModelViewRenderSystem> ();
                    if (spkRenderer) {
                        spkRenderer->SetPBRRimLightingStep((rkKeyBoard.IsDown()) ? -0.01f : 0.0f);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    virtual void BeginFrame(IRenderer& _rkRenderer, int _nPass)
    {
        if (_nPass != 0)
            return;

        m_nSecDuration += m_nCurrentDelta;
    }

    virtual void PrepareState(IRenderer& _rkRenderer, int _nPass)
    {

    }

    virtual void EndFrame(IRenderer& _rkRenderer, int _nPass)
    {
        if (_nPass != 0)
            return;
        m_nFrameCount++;
    }

    virtual void PreDraw(IRenderer& _rkRenderer, int _nPass)
    {

    }

    virtual void PostDraw(IRenderer& _rkRenderer, int _nPass)
    {
        if (!m_kFPSTextEntity.valid())
            return;
        m_nDrawCount++;
    }

    virtual void Swap(IRenderer& _rkRenderer)
    {
        if (!m_kFPSTextEntity.valid())
            return;
        if (m_nSecDuration > 1.0) {
            m_nSecDuration = 0.0;
            char cBuffer [512] = {};
            sprintf_s(cBuffer, sizeof(cBuffer), "FPS:%d \nDrawCall:%d", m_nFrameCount, m_nDrawCount);
            ITextHandler& rkText = (*m_kFPSTextEntity.component <ITextHandler>().get());
            rkText->SetText(cBuffer);
            m_nFrameCount = 0;
        }
        m_nDrawCount = 0;
    }
};

CApplication* CApplication::m_pkThis = nullptr;
#if 0
int main(int argc, char *argv [])
{
    CApplication kApp(1920, 1080, 0.1f, 2000, false);
    //CApplication kApp(1024, 768, 0.1f, 2000, false);
    kApp.Run();
    return 0;
}
#else
extern "C"
{
    ECS_MODELVIEW_API IApplication* LanucherCreateApp()
    {
        return new CApplication();
    }

    ECS_MODELVIEW_API void LanucherDestroyApp(IApplication* _pkApp)
    {
        if (_pkApp == nullptr)
            return;
        delete _pkApp;
    }
}
#endif