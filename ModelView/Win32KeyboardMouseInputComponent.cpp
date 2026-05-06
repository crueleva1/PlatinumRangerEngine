#include "MainPCH.h"
#include "Win32KeyboardMouseInputComponent.h"
#include "Win32InputSystem.h"


CWin32KeyboardMouseInputComponent::CWin32KeyboardMouseInputComponent(entityx::EventManager& _rkEventMgr)
    : m_rkEventMgr(_rkEventMgr)
{
    m_nKey = 0;
    m_nX = 0;
    m_nY = 0;
    m_nKey.clearDirty();
    m_nX.clearDirty();
    m_nY.clearDirty();
    m_rkEventMgr.subscribe <IInputEvent, IInputEventListener> (*this);
}

CWin32KeyboardMouseInputComponent::~CWin32KeyboardMouseInputComponent()
{
    m_rkEventMgr.unsubscribe <IInputEvent, IInputEventListener> (*this);
}

bool CWin32KeyboardMouseInputComponent::isHandled()
{
    bool bRet = m_nKey.isDirty();
    bRet |= m_nX.isDirty();
    bRet |= m_nY.isDirty();
    return !bRet;
}

void CWin32KeyboardMouseInputComponent::receive(const IInputEvent& _rkEvent)
{
    EInputEventType eType = (EInputEventType)_rkEvent.GetInputType();
    switch (eType) 
    {
        case eIET_Mouse:
        {
            const CMouseEvent& rkEvent = static_cast <const CMouseEvent&>(_rkEvent);
            m_nX = rkEvent.GetX();
            m_nY = rkEvent.GetY();
            m_bDown = rkEvent.IsDown();
            m_bLeft = rkEvent.IsLeft();
            m_bMoving = rkEvent.IsMoving();
            break;
        }
        case eIET_Keyboard:
        {
            const CKeyboardEvent& rkEvent = static_cast <const CKeyboardEvent&>(_rkEvent);
            m_nKey = rkEvent.GetKey();
            m_bKeyDown = rkEvent.IsDown();
            break;
        }
        default:
            return;
    }
}

void CWin32KeyboardMouseInputComponent::setHandled()
{
    m_nX.clearDirty();
    m_nY.clearDirty();
    m_nKey.clearDirty();
    m_nX.get() = 0.0f;
    m_nY.get() = 0.0f;
    m_nKey.get() = 0;
}

bool CWin32KeyboardMouseInputComponent::isMouseHandled()
{
    if (m_nX.isDirty())
        return false;

    return (!m_nY.isDirty());
}

bool CWin32KeyboardMouseInputComponent::isKeyHandled()
{
    return (!m_nKey.isDirty());
}