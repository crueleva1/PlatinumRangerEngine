#include "ECSFrameworkPCH.h"
#include "DefaultSceneComponent.h"
#include "SceneSystem.h"


CDefaultSceneComponent::CDefaultSceneComponent(entityx::Entity& _rkEntity, ISceneSystem& _rkSceneSys)
    : ISceneComponent(_rkEntity)
    , m_rkSceneSystem(_rkSceneSys)
    , m_pkParent(nullptr)
    , m_bScreenSpace(false)
    , m_bIsStatic(false)
{
}

CDefaultSceneComponent::~CDefaultSceneComponent()
{
}

void CDefaultSceneComponent::SetName(const char* _pcName)
{
    if (m_kName == _pcName)
        return;
    m_rkSceneSystem.Renamed(this, _pcName);
    ISceneComponent::SetName(_pcName);
}

size_t CDefaultSceneComponent::AttachGroup(ISceneComponent* _pkComp)
{
    if (!_pkComp)
        return -1;

    m_kChilds.insert(_pkComp);
    auto kIt = m_kChilds.find(_pkComp);
    return std::distance(m_kChilds.begin(), kIt);
}

size_t CDefaultSceneComponent::DeatchGroup(ISceneComponent* _pkComp)
{
    if (!_pkComp)
        return -1;

    auto kIt = m_kChilds.find(_pkComp);
    size_t nIndex = std::distance(m_kChilds.begin(), kIt);
    m_kChilds.erase(kIt);
    return nIndex;
}

ISceneComponent* CDefaultSceneComponent::GetObjectByName(const char* _pcName)
{
    for (auto pkComp : m_kChilds) {
        if (!strcmp(pkComp->GetName(), _pcName)) {
            return pkComp;
        }
        return pkComp->GetObjectByName(_pcName);
    }
    return nullptr;
}

ISceneComponent* CDefaultSceneComponent::GetParent()
{
    return m_pkParent;
}

void CDefaultSceneComponent::SetParent(ISceneComponent* _pkComp)
{
    m_pkParent = _pkComp;
}

void CDefaultSceneComponent::Update(float _fDelta)
{
    for (auto kIt = m_kChilds.begin(); kIt != m_kChilds.end(); ++kIt) {
        ISceneComponent* pkComp = (*kIt);
        if (!pkComp)
            continue;
        pkComp->Update(_fDelta);
    }
}

bool CDefaultSceneComponent::IsScreenSpace()
{
    return m_bScreenSpace;
}

void CDefaultSceneComponent::SetScreenSpace(bool _bScreenSpace)
{
    m_bScreenSpace = _bScreenSpace;
}

const size_t CDefaultSceneComponent::GetChildCount()
{
    return m_kChilds.size();
}

ISceneComponent* CDefaultSceneComponent::GetChildByIndex(size_t _nIndex)
{
    if (m_kChilds.size() <= _nIndex)
        return nullptr;

    std::set <ISceneComponent*>::iterator kIt = m_kChilds.begin();
    std::advance(kIt, _nIndex);
    return *kIt;
}