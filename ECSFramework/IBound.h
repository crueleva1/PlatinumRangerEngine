#pragma once


class ISceneComponent;
struct IBound
{
    virtual ~IBound()
    {
    }

    virtual float GetRadius() = 0;

    virtual bool IsTested(size_t _nHash) = 0;

    virtual void UpdateWorldTransform(ISceneComponent& _rkComp) = 0;
};