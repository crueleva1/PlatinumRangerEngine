#pragma once

#include "TComponentInterface.h"

class ECS_API IMovementComponent
{
public:
    virtual ~IMovementComponent()
    {}

    virtual void onHandleControl(entityx::Entity _kEntity) = 0;
};

typedef TComponent <IMovementComponent> IMovementHandler;