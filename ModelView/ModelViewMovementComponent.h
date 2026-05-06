#pragma once


#include "IMovementComponent.h"

class ECS_MODELVIEW_API CModelViewMovementComponent : public IMovementComponent
{
protected:
    glm::mat4 m_kRotate;
    glm::vec3 m_kVelocity;
    float m_nMaxVelocity;
public:
    CModelViewMovementComponent();

    virtual ~CModelViewMovementComponent();

    const glm::vec3& GetVelocity();

    const glm::mat4& GetRotate();

    void SetMaxVelocity(float _nMaxVelocity)
    {
        m_nMaxVelocity = _nMaxVelocity;
    }

    void SetVelocity(const glm::vec3& _rkVelocity);

    void SetCurrentRotation(const glm::mat3& _rkRotate);
};

class ECS_MODELVIEW_API CModelViewKBMovementComponent : public CModelViewMovementComponent
{
    float m_nX;
    float m_nY;
public:
    CModelViewKBMovementComponent();

    virtual ~CModelViewKBMovementComponent();

    virtual void onHandleControl(entityx::Entity _kEntity);
};

class ECS_MODELVIEW_API CModelViewSequenceMovementComponent : public CModelViewMovementComponent
{
public:
    struct SPosSequence
    {
        glm::vec3 m_kPos;
        glm::vec3 m_kSpeed;
    };
protected:
    std::vector <SPosSequence> m_kSequence;
    size_t m_nCurrentSeq;
public:
    CModelViewSequenceMovementComponent();

    virtual ~CModelViewSequenceMovementComponent();

    void ClearSequence();

    void AddSequence(SPosSequence& _rkSeq);

    void StartSequence();

    void StopSequence();

    virtual void onHandleControl(entityx::Entity _kEntity);
};