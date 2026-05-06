#pragma once


#include "LoggerSystem.h"


class CNanoLoggerSystem : public CLoggerSystem
{
public:

    CNanoLoggerSystem();

    virtual ~CNanoLoggerSystem();

    void configure(entityx::EntityManager& _kEntitieMgr, entityx::EventManager& _kEventMgr);
};