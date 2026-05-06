#pragma once

// Because it's relation with glm, so I want to make different with DirectX Utility Matrix So make source here
#include "GLMPlane.h"

class ECSGLM_API CGLMFrustum
{
public:
    CGLMFrustum();
    virtual ~CGLMFrustum();

    void CalculateFrustum(glm::mat4& _rkViewMatrix, glm::mat4& _rkProjectMatrix);

    bool PointInFrustum(const glm::vec3& _rkPoint);

    bool PointInFrustum(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf);
private:
    enum EPlane
    {
        eFP_Near,
        eFP_Far,
        eFP_Left,
        eFP_Right,
        eFP_Top,
        eFP_Bottom,
        eFP_Max,
    };
    CGLMPlane m_kFrustumPlane [eFP_Max];
};