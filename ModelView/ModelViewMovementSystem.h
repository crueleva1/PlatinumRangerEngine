#pragma once

#include "DefaultMovementSystem.h"

enum EMovementSystemContextType
{
    eMSCT_None,
    eMSCT_CreateKBComp,
    eMSCT_CreateSeqComp,
};

class ECS_MODELVIEW_API CCreateKBMovementCompContext : public IMovementSystemContext
{
    float m_nVelocity;
    entityx::Entity& m_rkEntity;
public:
    CCreateKBMovementCompContext(entityx::Entity& _rkEntity, float _nVelocity)
        : m_rkEntity(_rkEntity)
        , m_nVelocity(_nVelocity)
    {}

    virtual ~CCreateKBMovementCompContext()
    {}

    int GetContextType () const
    {
        return eMSCT_CreateKBComp;
    }

    entityx::Entity GetEntity() const
    {
        return m_rkEntity;
    }

    float GetVelocity() const
    {
        return m_nVelocity;
    }
};

class ECS_MODELVIEW_API CCreateSequenceMovementCompContext : public IMovementSystemContext
{
    std::string m_kSequence;
    entityx::Entity& m_rkEntity;
    bool m_bAutoStart;
public:
    CCreateSequenceMovementCompContext(entityx::Entity& _rkEntity, const char* _pcSequnece, bool _bAutoStart)
        : m_rkEntity(_rkEntity)
        , m_kSequence(_pcSequnece)
        , m_bAutoStart(_bAutoStart)
    {
    }

    virtual ~CCreateSequenceMovementCompContext()
    {
    }

    int GetContextType() const
    {
        return eMSCT_CreateSeqComp;
    }

    entityx::Entity GetEntity() const
    {
        return m_rkEntity;
    }

    const std::string& GetSequenceString() const
    {
        return m_kSequence;
    }

    bool IsAutoStart() const
    {
        return m_bAutoStart;
    }
};

class ECS_MODELVIEW_API CModelViewMovementSystem : public CDefaultMovementSystem
{
public:
    CModelViewMovementSystem();

    virtual ~CModelViewMovementSystem();

    virtual void update (entityx::EntityManager& _rkEntities, entityx::EventManager& _rkEvents, entityx::TimeDelta _fDelta);

    virtual void receive(const IMovementSystemContext&);
};