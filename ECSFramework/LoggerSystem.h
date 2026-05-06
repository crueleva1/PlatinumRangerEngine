#pragma once

#include "Logger.h"

class CLoggerSystem : public entityx::System<CLoggerSystem>
{
public:

    CLoggerSystem();

    virtual ~CLoggerSystem();

    virtual void configure(entityx::EntityManager& _kEntitieMgr, entityx::EventManager& _kEventMgr);

    virtual void update(entityx::EntityManager& _kEntitieMgr, entityx::EventManager& _kEventMgr, entityx::TimeDelta _nDelta);
};