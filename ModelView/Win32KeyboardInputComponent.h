#pragma once

#include "IInputControlComponent.h"
#include "DataModifier.h"

class ECS_MODELVIEW_API CWin32KeyboardInputComponent : public IInputControlComponent
{
    TDataModifier <int> m_nKey;
    bool m_bDown;
    entityx::EventManager& m_rkEventMgr;
public:
    CWin32KeyboardInputComponent(entityx::EventManager&);

    virtual ~CWin32KeyboardInputComponent();

    virtual bool isHandled();

    virtual void receive(const IInputEvent& _rkEvent);

    void setHandled();

    int getKey();

    bool isDown();
};