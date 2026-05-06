#pragma once


class ECS_API ICameraComponent
{
public:
    virtual ~ICameraComponent()
    {}

    virtual void ActiveCamera() = 0;

    virtual void DeactiveCamera() = 0;

    virtual bool IsCameraActive() = 0;

    virtual bool IsOrth() = 0;

    virtual void UpdateViewMatrix(class ISceneComponent& _rkComp) = 0;

    virtual void UpdateOrthProjectMatrix(float _fTop, float _fBottom, float _fRight, float _fLeft, float _fNear, float _fFar) = 0;

    virtual void UpdatePerspectiveProjectMatrix(float _fNear, float _fFar, int _nWidth, int _nHeight) = 0;

    virtual bool IsInView(struct IBound* _pkBound, size_t _nHash = -1) = 0;

    virtual void UpdateFrustum() = 0;

    virtual size_t GetCameraNameHash() = 0;

    virtual void UpdateCameraName(class ISceneComponent& _rkComp) = 0;
};

typedef TComponent<ICameraComponent> ICameraHandler;