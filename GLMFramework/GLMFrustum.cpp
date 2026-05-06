#include "GLMMainPCH.h"
#include "GLMFrustum.h"

CGLMFrustum::CGLMFrustum()
{

}

CGLMFrustum::~CGLMFrustum()
{

}

void CGLMFrustum::CalculateFrustum(glm::mat4& _rkViewMatrix, glm::mat4& _rkProjectMatrix)
{
    glm::mat4& kViewProjection = _rkProjectMatrix * _rkViewMatrix;

    {
        glm::vec3& kNormal = m_kFrustumPlane [eFP_Right].GetNormal();
        float& fConstant = m_kFrustumPlane [eFP_Right].GetConstant();

        kNormal.x = kViewProjection [0] [3] - kViewProjection [0] [0];
        kNormal.y = kViewProjection [1] [3] - kViewProjection [1] [0];
        kNormal.z = kViewProjection [2] [3] - kViewProjection [2] [0];
        fConstant = kViewProjection [3] [3] - kViewProjection [3] [0];
        m_kFrustumPlane [eFP_Right].Normalize();
    }

    {
        glm::vec3& kNormal = m_kFrustumPlane [eFP_Left].GetNormal();
        float& fConstant = m_kFrustumPlane [eFP_Left].GetConstant();

        kNormal.x = kViewProjection [0] [3] + kViewProjection [0] [0];
        kNormal.y = kViewProjection [1] [3] + kViewProjection [1] [0];
        kNormal.z = kViewProjection [2] [3] + kViewProjection [2] [0];
        fConstant = kViewProjection [3] [3] + kViewProjection [3] [0];
        m_kFrustumPlane [eFP_Left].Normalize();
    }

    {
        glm::vec3& kNormal = m_kFrustumPlane [eFP_Bottom].GetNormal();
        float& fConstant = m_kFrustumPlane [eFP_Bottom].GetConstant();

        kNormal.x = kViewProjection [0] [3] + kViewProjection [0] [1];
        kNormal.y = kViewProjection [1] [3] + kViewProjection [1] [1];
        kNormal.z = kViewProjection [2] [3] + kViewProjection [2] [1];
        fConstant = kViewProjection [3] [3] + kViewProjection [3] [1];
        m_kFrustumPlane [eFP_Bottom].Normalize();
    }

    {
        glm::vec3& kNormal = m_kFrustumPlane [eFP_Top].GetNormal();
        float& fConstant = m_kFrustumPlane [eFP_Top].GetConstant();

        kNormal.x = kViewProjection [0] [3] - kViewProjection [0] [1];
        kNormal.y = kViewProjection [1] [3] - kViewProjection [1] [1];
        kNormal.z = kViewProjection [2] [3] - kViewProjection [2] [1];
        fConstant = kViewProjection [3] [3] - kViewProjection [3] [1];
        m_kFrustumPlane [eFP_Top].Normalize();
    }

    {
        glm::vec3& kNormal = m_kFrustumPlane [eFP_Near].GetNormal();
        float& fConstant = m_kFrustumPlane [eFP_Near].GetConstant();

        kNormal.x = kViewProjection [0] [2] + kViewProjection [0] [3];
        kNormal.y = kViewProjection [1] [2] + kViewProjection [1] [3];
        kNormal.z = kViewProjection [2] [2] + kViewProjection [2] [3];
        fConstant = kViewProjection [3] [2] + kViewProjection [3] [3];
        m_kFrustumPlane [eFP_Near].Normalize();
    }

    {
        glm::vec3& kNormal = m_kFrustumPlane [eFP_Far].GetNormal();
        float& fConstant = m_kFrustumPlane [eFP_Far].GetConstant();

        kNormal.x = kViewProjection [0] [3] - kViewProjection [0] [2];
        kNormal.y = kViewProjection [1] [3] - kViewProjection [1] [2];
        kNormal.z = kViewProjection [2] [3] - kViewProjection [2] [2];
        fConstant = kViewProjection [3] [3] - kViewProjection [3] [2];
        m_kFrustumPlane [eFP_Far].Normalize();
    }
}

bool CGLMFrustum::PointInFrustum(const glm::vec3& _rkPoint)
{
    for (int nIndex = eFP_Near; nIndex < eFP_Max; nIndex++) {
        CGLMPlane::EPlaneSide eSide = m_kFrustumPlane [nIndex].GetSide(_rkPoint);
        if (eSide == CGLMPlane::ePS_Neg)
            return false;
    }

    return true;
}

bool CGLMFrustum::PointInFrustum(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf)
{
    for (int nIndex = eFP_Near; nIndex < eFP_Max; nIndex++) {
        CGLMPlane::EPlaneSide eSide = m_kFrustumPlane [nIndex].GetSide(_rkCenter, _rkHalf);
        if (eSide == CGLMPlane::ePS_Neg)
            return false;
    }

    return true;
}