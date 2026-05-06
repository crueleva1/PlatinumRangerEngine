#include "MainPCH.h"
#include "Win32MouseInputComponent.h"
#include "Win32InputSystem.h"


CWin32MouseInputComponent::CWin32MouseInputComponent(entityx::EventManager& _rkEventMgr)
    : m_rkEventMgr(_rkEventMgr)
    , m_bDown(false)
    , m_bLeft(false)
    , m_bMoving(false)
{
    m_nX = 0.0f;
    m_nY = 0.0f;
    m_nX.clearDirty();
    m_nY.clearDirty();
    m_rkEventMgr.subscribe <IInputEvent, IInputEventListener>(*this);
}

CWin32MouseInputComponent::~CWin32MouseInputComponent()
{
    m_rkEventMgr.unsubscribe <IInputEvent, IInputEventListener>(*this);
}

bool CWin32MouseInputComponent::isHandled()
{
    bool bRet = m_nX.isDirty();
    bRet |= m_nY.isDirty();
    return !bRet;
}

void CWin32MouseInputComponent::receive(const IInputEvent& _rkEvent)
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
        default:
            return;
    }
}

void CWin32MouseInputComponent::setHandled()
{
    m_nX.clearDirty();
    m_nY.clearDirty();
    m_nX.get() = 0.0f;
    m_nY.get() = 0.0f;
}

