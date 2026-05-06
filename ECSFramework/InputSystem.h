#pragma once

#include "IInputControlComponent.h"

class IInputSystemContext
{
public:
    virtual ~IInputSystemContext()
    {}

    virtual int GetContextType() const = 0;
};

class IInputSystem 
    : public entityx::System <IInputSystem>
    , public entityx::Receiver <IInputSystem>
{
protected:
    std::queue <std::shared_ptr <IInputEvent> > m_kInputQueue;

    virtual void PushInputEvent(std::shared_ptr <IInputEvent> _pkEvnet)
    {
        if (!_pkEvnet)
            return;

        m_kInputQueue.push(_pkEvnet);
    }
public:
    virtual ~IInputSystem()
    {
    }

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
    {
        _rkEventMgr.subscribe <IInputSystemContext, IInputSystem> (*this);
        entityx::System <IInputSystem>::configure(_rkEntityMgr, _rkEventMgr);
    }

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta _nDelta)
    {
        while (!m_kInputQueue.empty()) {
            std::shared_ptr <IInputEvent> spkEvent = m_kInputQueue.front();
            m_kInputQueue.pop();
            if (!spkEvent)
                continue;
            ResolveInput(_rkEventMgr, *spkEvent);
        }
    }

    virtual void ResolveInput(entityx::EventManager& _rkEventMgr, const IInputEvent& _rkEvent) = 0;

    virtual void receive(const IInputSystemContext&) = 0;
};