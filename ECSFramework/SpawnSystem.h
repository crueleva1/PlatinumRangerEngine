#pragma once

#include "PropertySet.h"

struct ECS_API IComponentDesc : public CPropertySet
{
    virtual ~IComponentDesc()
    {
    }

    virtual void SetAttrTag(const char* _pcAttr) = 0;

    virtual const char* GetAttrTag() = 0;

    virtual bool IsAttrTagMatch(const char* _pcAttr) = 0;

};

struct ECS_API ISpawnDesc
{
    virtual ~ISpawnDesc()
    {
    }

    virtual size_t GetComponentsCount() = 0;

    virtual IComponentDesc& GetComponent(size_t _nIndex) = 0;
};



class ECS_API ISpawnSystem : public entityx::System <ISpawnSystem>
{
    std::queue <std::shared_ptr <ISpawnDesc> > m_kSpawnQueue;
protected:
    void PushSpawnDesc(std::shared_ptr <ISpawnDesc> _rspkDesc)
    {
        m_kSpawnQueue.push(_rspkDesc);
    }
public:
    virtual ~ISpawnSystem()
    {
    }

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
    {
        while (!m_kSpawnQueue.empty()) {
            std::shared_ptr <ISpawnDesc> spkDesc = m_kSpawnQueue.front();
            m_kSpawnQueue.pop();
            ResolveDesc(_rkEntityMgr, _rkEventMgr, spkDesc.get());
        }
    }

    virtual void ResolveDesc(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, ISpawnDesc* _rkDesc) = 0;
};