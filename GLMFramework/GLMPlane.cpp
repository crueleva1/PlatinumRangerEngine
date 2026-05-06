#include "GLMMainPCH.h"
#include "GLMPlane.h"

CGLMPlane::CGLMPlane()
    :m_fD(0.0f)
{
}

CGLMPlane::CGLMPlane(glm::vec3& _rkNormal, float _fConstant)
    : m_kNormal(_rkNormal)
    , m_fD(-_fConstant)
{

}

CGLMPlane::~CGLMPlane()
{

}

CGLMPlane::EPlaneSide CGLMPlane::GetSide(const glm::vec3& _rkPos) const
{
    float fDistance = GetDistance(_rkPos);
    if (fDistance < 0.0f)
        return EPlaneSide::ePS_Neg;

    if (fDistance > 0.0f)
        return EPlaneSide::ePS_Pos;

    return EPlaneSide::ePS_None;
}

CGLMPlane::EPlaneSide CGLMPlane::GetSide(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf) const
{
    float fDistance = GetDistance(_rkCenter);

    float fMaxAdsDist = glm::dot(glm::abs(m_kNormal), glm::abs(_rkHalf));

    if (fDistance < -fMaxAdsDist)
        return EPlaneSide::ePS_Neg;

    if (fDistance > fMaxAdsDist)
        return EPlaneSide::ePS_Pos;

    return EPlaneSide::ePS_Both;
}

float CGLMPlane::GetDistance(const glm::vec3& _rkPos) const
{
    float fDot = glm::dot(m_kNormal, _rkPos);
    return fDot + m_fD;
}

float CGLMPlane::Normalize()
{
    float fLength = glm::length(m_kNormal);

    if (fLength > 0.0f) {
        float fInvLength = 1.0f / fLength;
        m_kNormal *= fInvLength;
        m_fD *= fInvLength;
    }

    return fLength;
}