#include "ECSFrameworkPCH.h"
#include "DefaultSceneSystem.h"

CDefaultSceneSystem::CDefaultSceneSystem()
{

}

CDefaultSceneSystem::~CDefaultSceneSystem()
{

}

void CDefaultSceneSystem::update(entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta)
{
    for (auto kIt = m_kRootedComp.begin(); kIt != m_kRootedComp.end(); ++kIt) {
        ISceneComponent* pkRoot = kIt->second;
        if (!pkRoot)
            continue;
        pkRoot->Update(_fDelta);
    }
}

void CDefaultSceneSystem::Attached(ISceneComponent* _pkChild)
{
    if (!_pkChild)
        return;

    assert(m_kRootedComp.find(_pkChild->GetName()) == m_kRootedComp.end());
    m_kRootedComp.insert(std::make_pair(_pkChild->GetName(), _pkChild));
}

void CDefaultSceneSystem::Detached(ISceneComponent* _pkChild)
{
    if (!_pkChild)
        return;

    m_kRootedComp.erase(_pkChild->GetName());
}

void CDefaultSceneSystem::Renamed(ISceneComponent* _pkChild, const char* _pcNewName)
{
    if (!_pkChild)
        return;

    auto kIt = m_kRootedComp.find(_pkChild->GetName());
    if (kIt == m_kRootedComp.end())
        return;

    m_kRootedComp.erase(kIt);
    m_kRootedComp.insert(std::make_pair(_pcNewName, _pkChild));
}