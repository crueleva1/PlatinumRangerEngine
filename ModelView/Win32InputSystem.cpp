#include "MainPCH.h"
#include "Win32InputSystem.h"
#include "Win32KeyboardInputComponent.h"
#include "Win32KeyboardMouseInputComponent.h"
#include "Win32MouseInputComponent.h"
#include <windowsx.h>

UINT GetVirtualKeyToScanCode(WPARAM _nwParam, LPARAM _nlParam)
{
    if (HIWORD(_nlParam) & 0x0F00) {
        UINT nScan = MapVirtualKey(_nwParam, 0);
        return nScan | 0x80;
    }
    return HIWORD(_nlParam) & 0x00FF;
}

CWin32InputSystem::CWin32InputSystem()
    : m_pkEventMgr(nullptr)
{

}

CWin32InputSystem::~CWin32InputSystem()
{

}

void CWin32InputSystem::configure(entityx::EventManager& _rkEvnetMgr)
{
    m_pkEventMgr = &_rkEvnetMgr;
}

LRESULT CWin32InputSystem::WndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
    static bool bDown = false;
    static bool bLeft = false;
    static bool bMoving = false;
    switch (_nMessage)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        {
            float nX = GET_X_LPARAM(_lParam);
            float nY = GET_Y_LPARAM(_lParam);
            switch (_nMessage)
            {
                case WM_LBUTTONDOWN:
                {
                    bDown = true;
                    bLeft = true;
                    bMoving = false;
                    break;
                }
                case WM_RBUTTONDOWN:
                {
                    bDown = true;
                    bLeft = false;
                    bMoving = false;
                    break;
                }
                case WM_LBUTTONUP:
                {
                    bDown = false;
                    bLeft = true;
                    bMoving = false;
                    break;
                }
                case WM_RBUTTONUP:
                {
                    bDown = false;
                    bLeft = false;
                    bMoving = false;
                    break;
                }
                case WM_MOUSEMOVE:
                {
                    bMoving = true;
                    bool bLeftButtonDown = (_wParam & MK_LBUTTON) != 0;
                    bool bRightButtonDown = (_wParam & MK_RBUTTON) != 0;

                    if (bLeftButtonDown) {
                        bDown = true;
                        bLeft = true;
                    }
                    else if (bRightButtonDown) {
                        bDown = true;
                        bLeft = false;
                    }
                    else {
                        bDown = false;
                    }
                    break;
                }
            }
            PushInputEvent(std::make_shared <CMouseEvent>(nX, nY, bLeft, bDown, bMoving));
            return 0;
        }
        case WM_KEYDOWN:
        {
            PushInputEvent(std::make_shared <CKeyboardEvent>(GetVirtualKeyToScanCode(_wParam, _lParam), true));
            return 0;
        }
        case WM_KEYUP:
        {
            PushInputEvent(std::make_shared <CKeyboardEvent> (GetVirtualKeyToScanCode(_wParam, _lParam), false));
            return 0;
        }
        case WM_CLOSE:
        {
            PushInputEvent(std::make_shared <CAppInputEvent>(CAppInputEvent::eAIE_Quit));
            return 0;
        }
        case WM_SIZE:
        {
            PushInputEvent(std::make_shared <CAppInputEvent>(CAppInputEvent::eAIE_ResizeWindow, _wParam, _lParam));
        }
        default:
        {
            return DefWindowProc(_hWnd, _nMessage, _wParam, _lParam);
        }
    }
}

void CWin32InputSystem::ResolveInput(entityx::EventManager& _rkEventMgr, const IInputEvent& _rkEvent)
{
    _rkEventMgr.emit <IInputEvent>(_rkEvent);
}

void CWin32InputSystem::receive(const IInputSystemContext& _rkContext)
{
    EInputSystemContextType eType = (EInputSystemContextType)_rkContext.GetContextType();
    const CWin32InputSystemContext& rkContext = static_cast <const CWin32InputSystemContext&>(_rkContext);
    entityx::Entity kEntity = rkContext.GetEntity();
    if (!kEntity)
        return;
    if (kEntity.has_component <IInputControlHandler> ()) {
        kEntity.remove <IInputControlHandler> ();
    }
    switch (eType)
    {
        case eISCT_CreateKeyboard:
        {
            kEntity.assign <IInputControlHandler>(std::make_shared<CWin32KeyboardInputComponent>(*m_pkEventMgr));
            break;
        }
        case eISCT_CreateMouse:
        {
            kEntity.assign <IInputControlHandler>(std::make_shared<CWin32MouseInputComponent>(*m_pkEventMgr));
            break;
        }
        case eISCT_CreateKeyboardMouse:
        {
            kEntity.assign <IInputControlHandler>(std::make_shared<CWin32KeyboardMouseInputComponent>(*m_pkEventMgr));
            break;
        }
        default:
            return;
    }
}