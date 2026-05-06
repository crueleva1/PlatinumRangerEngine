#pragma once

#include "DefaultTextComponent.h"

class ECS_MODELVIEW_API CModelViewTextComp : public CDefaultTextComponent
{
public:
    CModelViewTextComp();

    virtual ~CModelViewTextComp();

    virtual bool ValidateText(entityx::Entity& _rkEntity);
};