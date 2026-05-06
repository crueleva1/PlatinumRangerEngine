#include "ECSFrameworkPCH.h"
#include "LoggerSystem.h"


CLoggerSystem::CLoggerSystem()
{

}

CLoggerSystem::~CLoggerSystem()
{

}

void CLoggerSystem::configure(entityx::EntityManager& _kEntitieMgr, entityx::EventManager& _kEventMgr)
{

}

void CLoggerSystem::update(entityx::EntityManager& _kEntitieMgr, entityx::EventManager& _kEventMgr, entityx::TimeDelta _nDelta)
{
    ILogger::Sync(_nDelta);
}