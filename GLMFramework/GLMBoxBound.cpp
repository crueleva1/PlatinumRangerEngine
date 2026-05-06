#include "GLMMainPCH.h"
#include "GLMBoxBound.h"
#include "GLMSceneComponent.h"

CGLMBoundBox::CGLMBoundBox()
    :m_kScaled(1.0f, 1.0f, 1.0f)
{
}

CGLMBoundBox::~CGLMBoundBox()
{

}

float CGLMBoundBox::GetRadius()
{
    float fRadius = (m_kBoxVec.x > m_kBoxVec.y) ? m_kBoxVec.x : m_kBoxVec.y;
    fRadius = (fRadius > m_kBoxVec.z) ? fRadius : m_kBoxVec.z;
    return fRadius;
}

const glm::vec3& CGLMBoundBox::GetCenter() const
{
    return m_kCenter;
}

const glm::vec3& CGLMBoundBox::GetBoxVector() const
{
    return m_kBoxVec;
}

const glm::vec3& CGLMBoundBox::GetWorldCenter() const
{
    return m_kWorldCenter.get();
}

void CGLMBoundBox::SetCenter(glm::vec3& _rkCentor)
{
    m_kCenter = _rkCentor;
}

void CGLMBoundBox::SetBound(glm::vec3& _rkMin, glm::vec3& _rkMax)
{
    m_kCenter = (_rkMax + _rkMin) / 2.0f;
    m_kBoxVec = (_rkMax - _rkMin) / 2.0f;
}

void CGLMBoundBox::SetWorldTransform(const glm::mat4& _rkWorld)
{
    m_kWorldCenter = _rkWorld * glm::vec4(m_kCenter, 1.0f);
    m_kBoxVec = m_kBoxVec / m_kScaled;
    m_kBoxVec = _rkWorld * glm::vec4(m_kBoxVec, 0.0f);
    m_kScaled = glm::vec3(1.0f, 1.0f, 1.0f);
    m_kScaled = _rkWorld * glm::vec4(m_kScaled, 0.0f);
    if (m_kWorldCenter.isDirty()) {
        m_kWorldCenter.clearDirty();
        m_kTestedGroup.clear();
    }
}

void CGLMBoundBox::UpdateWorldTransform(class ISceneComponent& _rkComp)
{
    CGLMSceneComponent* pkComp = dynamic_cast<CGLMSceneComponent*>(&_rkComp);
    if (!pkComp)
        return;

    SetWorldTransform(pkComp->GetTransform());
}

bool CGLMBoundBox::IsTested(size_t _nHash)
{
    auto kIt = m_kTestedGroup.find(_nHash);
    if (kIt != m_kTestedGroup.end())
        return kIt->second;
    return false;
}

void CGLMBoundBox::SetTested(size_t _nHash, bool _bTested)
{
#if 1   // There was Performance issue
    auto kIt = m_kTestedGroup.find(_nHash);
    if (kIt != m_kTestedGroup.end())
        kIt->second |= _bTested;
    else
        m_kTestedGroup.insert(std::make_pair(_nHash, _bTested));
#else
    m_kTestedGroup [_nHash] = _bTested;
#endif
}