#pragma once

#include "IResourceComponent.h"

class ECS_API IResourceSystem : public entityx::System <IResourceSystem>
{
public:

    virtual ~IResourceSystem()
    {
    }

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
    {
        validateResource(_rkEntityMgr);
    }

    virtual void validateResource(entityx::EntityManager& _rkEnitityMgr) = 0;

    inline void OnSetStateus(IResourceComponent* _pkComp, int _eStatus)
    {
        _pkComp->SetStatus(_eStatus);
    }
};