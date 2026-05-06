#include "MainPCH.h"
#include "ModelViewImgui.h"
#include "RenderSystem.h"
#include "ImguiRenderPipeline.h"
#include "Win32InputSystem.h"

CModelViewImgui::CModelViewImgui()
    :m_hWnd(nullptr)
,m_nX(0)
, m_nY(0)
,m_bDown(false)
,m_bLeft(false)
,m_bRight(false)
,m_bMoving(false)
{

}

CModelViewImgui::~CModelViewImgui()
{

}

void CModelViewImgui::ConfigIO(ImGuiIO& _rkIO)
{
    CImgui::ConfigIO(_rkIO);

    if (!m_hWnd)
        return;
    _rkIO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (opt_rkIOnal)
    _rkIO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor _rkIO.WantSetMousePos requests (opt_rkIOnal, rarely used)
    _rkIO.ImeWindowHandle = m_hWnd;

    // Keyboard mapping. ImGui will use those indices to peek into the _rkIO.KeysDown[] array that we will update during the applicat_rkIOn lifetime.
    _rkIO.KeyMap [ImGuiKey_Tab] = VK_TAB;
    _rkIO.KeyMap [ImGuiKey_LeftArrow] = VK_LEFT;
    _rkIO.KeyMap [ImGuiKey_RightArrow] = VK_RIGHT;
    _rkIO.KeyMap [ImGuiKey_UpArrow] = VK_UP;
    _rkIO.KeyMap [ImGuiKey_DownArrow] = VK_DOWN;
    _rkIO.KeyMap [ImGuiKey_PageUp] = VK_PRIOR;
    _rkIO.KeyMap [ImGuiKey_PageDown] = VK_NEXT;
    _rkIO.KeyMap [ImGuiKey_Home] = VK_HOME;
    _rkIO.KeyMap [ImGuiKey_End] = VK_END;
    _rkIO.KeyMap [ImGuiKey_Insert] = VK_INSERT;
    _rkIO.KeyMap [ImGuiKey_Delete] = VK_DELETE;
    _rkIO.KeyMap [ImGuiKey_Backspace] = VK_BACK;
    _rkIO.KeyMap [ImGuiKey_Space] = VK_SPACE;
    _rkIO.KeyMap [ImGuiKey_Enter] = VK_RETURN;
    _rkIO.KeyMap [ImGuiKey_Escape] = VK_ESCAPE;
    _rkIO.KeyMap [ImGuiKey_A] = 'A';
    _rkIO.KeyMap [ImGuiKey_C] = 'C';
    _rkIO.KeyMap [ImGuiKey_V] = 'V';
    _rkIO.KeyMap [ImGuiKey_X] = 'X';
    _rkIO.KeyMap [ImGuiKey_Y] = 'Y';
    _rkIO.KeyMap [ImGuiKey_Z] = 'Z';

    if (m_pkRenderSystem == nullptr)
        return;

    IRenderer* pkRenderer = m_pkRenderSystem->GetRenderer();
    if (pkRenderer == nullptr)
        return;

    _rkIO.DisplaySize = ImVec2(pkRenderer->GetScreenWidth(), pkRenderer->GetScreenHeight());
    _rkIO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    _rkIO.MouseDrawCursor = true;

    RECT rcWindow, rcClient;
    GetWindowRect((HWND)m_hWnd, &rcWindow);
    GetClientRect((HWND)m_hWnd, &rcClient);

    int windowHeight = rcWindow.bottom - rcWindow.top;
    int clientHeight = rcClient.bottom - rcClient.top;
    TitleBarOffsetY = windowHeight - clientHeight;

}

void CModelViewImgui::ConfigWndHandle(void* _hWnd)
{
    m_hWnd = _hWnd;
}

void CModelViewImgui::ConfigRenderer(ImGuiIO& _rkIO, IRenderSystem& _rkRenderSystem)
{
    CImgui::ConfigRenderer(_rkIO, _rkRenderSystem);
    m_pkRenderSystem = &_rkRenderSystem;

	CImguiRenderPipeline* pkPipeline = GetRenderPipeline();
	if (!pkPipeline)
		return;
	pkPipeline->CreateFontTexture(_rkIO);
}

void CModelViewImgui::UpdateNewFrame(ImGuiIO& _rkIO, float _nDelta)
{
    CImgui::UpdateNewFrame(_rkIO, _nDelta);
}

void CModelViewImgui::UpdateMouseCursor(ImGuiIO& _rkIO)
{
    if (_rkIO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;
    // This will move to Win32 Input system
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || _rkIO.MouseDrawCursor) {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor) {
        case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
        case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
        case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
        case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
        case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
        case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
        case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
        case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
    return;
}

void CModelViewImgui::UpdateMousePos(ImGuiIO& _rkIO)
{
    if (_rkIO.WantSetMousePos) {
        // Not Impl
        // Maybe use emit
    }
    _rkIO.MousePos.x = m_nX;
    _rkIO.MousePos.y = m_nY + TitleBarOffsetY;

    _rkIO.MouseDown[0] = m_bLeft;
    _rkIO.MouseDown[1] = m_bRight;
#if 0
    if (m_bLeft || m_bRight)
	    ILogger::Log(eLL_Info, "UpdateMousePos X:%f, Y:%f, _rkIO.MouseDown[0]=%s _rkIO.MouseDown[1]=%s\n", m_nX, m_nY, _rkIO.MouseDown [0] ? "true" : "false", _rkIO.MouseDown [1] ? "true" : "false");
#endif    
    m_bLeft = false;
    m_bRight = false;

}

void CModelViewImgui::ShutDown()
{
    //CImgui::ShutDown();
}

void CModelViewImgui::RenderNewFrame()
{
    CImgui::RenderNewFrame();
}

void CModelViewImgui::RenderDrawData(ImGuiIO& _rkIO, ImDrawData* _pkDrawData)
{
    int fb_width = (int)(_pkDrawData->DisplaySize.x * _rkIO.DisplayFramebufferScale.x);
    int fb_height = (int)(_pkDrawData->DisplaySize.y * _rkIO.DisplayFramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    _pkDrawData->ScaleClipRects(_rkIO.DisplayFramebufferScale);
    
    CImguiRenderPipeline* pkPipeline = GetRenderPipeline();
    if (!pkPipeline)
        return;
    pkPipeline->CreateFontTexture(_rkIO);
    pkPipeline->CreateMeshBuffer(_pkDrawData);
}

void CModelViewImgui::InvalidateDeviceObjects()
{

}

bool CModelViewImgui::CreateDeviceObjects(ImGuiIO& _rkIO)
{
    CImguiRenderPipeline* pkPipeline = GetRenderPipeline();
    if (!pkPipeline)
        return false;
    pkPipeline->CreateFontTexture(_rkIO);
    return true;
}

void CModelViewImgui::DrawImgui()
{
    ImGui::ShowDemoWindow();
#if 1
	ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("Debug");
    ImGui::Text("MousePos = (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
    ImGui::End();
#endif
}

void CModelViewImgui::receive(const IInputEvent& _rkEvent)
{
    EInputEventType eType = (EInputEventType)_rkEvent.GetInputType();
    switch (eType) {
    case eIET_Mouse:
    {
        const CMouseEvent& rkEvent = static_cast <const CMouseEvent&>(_rkEvent);
        m_nX = rkEvent.GetX();
        m_nY = rkEvent.GetY();
		if (rkEvent.IsLeft())
			m_bLeft = rkEvent.IsDown();
		else
			m_bRight = rkEvent.IsDown();
        m_bMoving = rkEvent.IsMoving();
		//ILogger::Log(eLL_Info, "CModelViewImgui X:%f, Y:%f, m_bDown=%s, m_bLeft=%s, m_bMoving=%s\n", m_nX, m_nY, m_bDown ? "true" : "false", m_bLeft ? "true" : "false", m_bMoving ? "true" : "false");
        break;
    }
    default:
        return;
    }
}