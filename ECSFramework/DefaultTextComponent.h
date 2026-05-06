#pragma once

#include "ITextComponent.h"
#include "DataModifier.h"

class ECS_API CDefaultTextComponent : public ITextComponent
{
    IFont* m_pkFont;
    TDataModifier<std::string> m_kText;
public:
    CDefaultTextComponent();

    virtual ~CDefaultTextComponent();

    virtual IFont* GetFont();

    virtual void SetFont(IFont* _pkFont);

    virtual void SetText(const char* _pcText);

    virtual bool ValidateText(entityx::Entity& _rkEntity);

    const char* GetText();

    const std::string& GetTextString();
};