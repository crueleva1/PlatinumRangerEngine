#pragma once

#include "MovementSystem.h"

class ECS_API CDefaultMovementSystem : public IMovementSystem
{
public:
    CDefaultMovementSystem();

    virtual ~CDefaultMovementSystem();

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventsMgr);

    virtual void update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta);
};