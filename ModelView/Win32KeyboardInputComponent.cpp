#include "MainPCH.h"
#include "Win32KeyboardInputComponent.h"
#include "Win32InputSystem.h"

CWin32KeyboardInputComponent::CWin32KeyboardInputComponent(entityx::EventManager& _rkEventMgr)
    : m_rkEventMgr(_rkEventMgr)
{
    m_nKey = 0;
    m_nKey.clearDirty();
    m_rkEventMgr.subscribe <IInputEvent, IInputEventListener> (*this);
}

CWin32KeyboardInputComponent::~CWin32KeyboardInputComponent ()
{
    m_rkEventMgr.unsubscribe <IInputEvent, IInputEventListener> (*this);
}

bool CWin32KeyboardInputComponent::isHandled ()
{
    return !m_nKey.isDirty();
}

void CWin32KeyboardInputComponent::receive(const IInputEvent& _rkEvent)
{
    EInputEventType eType = (EInputEventType)_rkEvent.GetInputType();
    switch (eType)
    {
        case eIET_Keyboard:
        {
            const CKeyboardEvent& rkKeyBoardEvent = static_cast <const CKeyboardEvent&>(_rkEvent);
            m_nKey = rkKeyBoardEvent.GetKey();
            m_bDown = rkKeyBoardEvent.IsDown();
            break;
        }
        default:
            return;
    }
}

void CWin32KeyboardInputComponent::setHandled()
{
    m_nKey.clearDirty();
}

int CWin32KeyboardInputComponent::getKey()
{
    m_nKey.clearDirty();
    m_nKey.get() = 0;
    return m_nKey;
}

bool CWin32KeyboardInputComponent::isDown()
{
    return m_bDown;
}