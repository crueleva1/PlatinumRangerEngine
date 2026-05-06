#pragma once


struct IInputEvent
{
public:
    virtual ~IInputEvent ()
    {

    }

    virtual int GetInputType () const = 0;

};

class IInputEventListener : virtual public entityx::Receiver <IInputEventListener>
{
public:

    virtual ~IInputEventListener ()
    {
    }

    virtual void receive (const IInputEvent& _rkEvent) = 0;
};

class IInputControlComponent : public IInputEventListener
{
public:

    virtual ~IInputControlComponent()
    {
    }

    virtual bool isHandled() = 0;
};

typedef TComponent <IInputControlComponent> IInputControlHandler;