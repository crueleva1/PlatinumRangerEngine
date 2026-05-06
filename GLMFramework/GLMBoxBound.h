#pragma once


#include "IBound.h"
#include "DataModifier.h"

class ECSGLM_API CGLMBoundBox : public IBound
{
    TDataModifier<glm::vec3> m_kWorldCenter;
    glm::vec3 m_kCenter;
    glm::vec3 m_kBoxVec;
    glm::vec3 m_kScaled;
    std::map <size_t, bool> m_kTestedGroup;
public:
    CGLMBoundBox();

    virtual ~CGLMBoundBox();

    virtual float GetRadius();

    virtual const glm::vec3& GetCenter() const;

    virtual const glm::vec3& GetBoxVector() const;

    virtual const glm::vec3& GetWorldCenter() const;

    virtual void SetCenter(glm::vec3&);

    virtual void SetWorldTransform(const glm::mat4&);

    virtual void UpdateWorldTransform(class ISceneComponent& _rkComp);

    virtual void SetBound(glm::vec3& _rkMin, glm::vec3& _rkMax);

    virtual bool IsTested(size_t _nHash);

    void SetTested(size_t _nHash, bool _bTested);
};