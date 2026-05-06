#pragma once

#include "entityx/System.h"


template <class SystemInterface>
class TSystem : entityx::System <TSystem <SystemInterface>>
{
    std::shared_ptr <SystemInterface> m_spkSystem;
public:

    TSystem(SystemInterface* _pkSystem)
        : m_spkSystem(_pkSystem)
    { 
    }

    virtual ~TSystem()
    {
    }

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
    {
        m_spkSystem->configure(_rkEntityMgr, _rkEventMgr);
    }

    virtual void configure(entityx::EventManager& _rkEventMgr)
    {
        m_spkSystem->configure(_rkEventMgr);
    }

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
    {
        m_spkSystem->update(_rkEntityMgr, _rkEventMgr, dt);
    }
};