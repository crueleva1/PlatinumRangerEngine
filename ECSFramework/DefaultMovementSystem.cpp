#include "ECSFrameworkPCH.h"
#include "DefaultMovementSystem.h"
#include "IMovementComponent.h"

CDefaultMovementSystem::CDefaultMovementSystem()
{

}

CDefaultMovementSystem::~CDefaultMovementSystem()
{

}

void CDefaultMovementSystem::configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventsMgr)
{
    _rkEventsMgr.subscribe <IMovementSystemContext, IMovementSystem> (*this);
}

void CDefaultMovementSystem::update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta)
{
    for (auto kEntity : _rkEntities.entities_with_components <IMovementHandler> ()) {
        entityx::ComponentHandle <IMovementHandler> kHandler = kEntity.component <IMovementHandler>();
        kHandler->get()->onHandleControl(kEntity);
    }
}