#pragma once


#include "ICameraComponent.h"
#include "GLMFrustum.h"

class ECSGLM_API CGLMBaseCameraComponent : public ICameraComponent
{
protected:
    CGLMFrustum m_kFrustum;
    glm::mat4 m_kViewMat;
    glm::mat4 m_kProjectMat;
    glm::vec2 m_kNearFar;
    bool m_bActive;
    bool m_bOrth;
public:
    CGLMBaseCameraComponent();

    virtual ~CGLMBaseCameraComponent();

    virtual void ActiveCamera();

    virtual void DeactiveCamera();

    virtual bool IsCameraActive();

    virtual bool IsOrth();

    virtual void UpdateOrthProjectMatrix(float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar);

    virtual void UpdatePerspectiveProjectMatrix(float _fNear, float _fFar, int _nWidth, int _nHeight);

    inline const glm::mat4& GetViewMatrix()
    {
        return m_kViewMat;
    }

    inline const glm::mat4& GetProjectMatrix()
    {
        return m_kProjectMat;
    }

    inline const glm::vec2& GetNearFar()
    {
        return m_kNearFar;
    }

    virtual bool IsInView(struct IBound* _pkBound, size_t _nHash = -1);

    virtual bool IsInView(const glm::vec3& _rkPoint);

    virtual bool IsInView(const glm::vec3& _rkPoint, const glm::vec3& _rkHalf);

    virtual void UpdateFrustum();
};