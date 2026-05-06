#pragma once


#include "IInputControlComponent.h"
#include "DataModifier.h"

class ECS_MODELVIEW_API CWin32KeyboardMouseInputComponent : public IInputControlComponent
{
    TDataModifier <int> m_nKey;
    TDataModifier <float> m_nX;
    TDataModifier <float> m_nY;
    bool m_bKeyDown;
    bool m_bDown;
    bool m_bLeft;
    bool m_bMoving;
    entityx::EventManager& m_rkEventMgr;
public:
    CWin32KeyboardMouseInputComponent(entityx::EventManager& _rkEventMgr);

    virtual ~CWin32KeyboardMouseInputComponent();

    virtual bool isHandled();

    virtual void receive(const IInputEvent& _rkEvent);

    void setHandled();

    bool isKeyHandled();

    bool isMouseHandled();

    inline float getX()
    {
        return m_nX;
    }

    inline float getY()
    {
        return m_nY;
    }

    inline bool isDown()
    {
        return m_bDown;
    }

    inline bool isLeft()
    {
        return m_bLeft;
    }

    inline bool isMoving()
    {
        return m_bMoving;
    }

    inline bool isKeyDown()
    {
        return m_bKeyDown;
    }

    inline int getKey()
    {
        return m_nKey;
    }
};