#pragma once

#include "SceneSystem.h"

class ECS_API CDefaultSceneSystem : public ISceneSystem
{
protected:
    std::map <std::string, ISceneComponent*> m_kRootedComp;
public:
    CDefaultSceneSystem();

    virtual ~CDefaultSceneSystem();

    virtual void update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta);

    virtual void Attached(ISceneComponent* _pkChild);

    virtual void Detached(ISceneComponent* _pkChild);

    virtual void Renamed(ISceneComponent* _pkChild, const char* _pcNewName);
};