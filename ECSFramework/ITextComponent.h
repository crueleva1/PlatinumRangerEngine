#pragma once

#include "TComponentInterface.h"

class ECS_API ITextComponent
{
public:
    virtual ~ITextComponent()
    {}

    virtual class IFont* GetFont() = 0;

    virtual void SetFont(class IFont* _pkFont) = 0;

    virtual void SetText(const char* _pcText) = 0;

    virtual bool ValidateText(entityx::Entity& _rkEntity) = 0;
};

typedef TComponent <ITextComponent> ITextHandler;