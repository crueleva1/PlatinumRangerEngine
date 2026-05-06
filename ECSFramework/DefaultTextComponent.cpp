#include "ECSFrameworkPCH.h"
#include "DefaultTextComponent.h"


CDefaultTextComponent::CDefaultTextComponent()
    : m_pkFont(nullptr)
{

}

CDefaultTextComponent::~CDefaultTextComponent()
{

}

IFont* CDefaultTextComponent::GetFont()
{
    return m_pkFont;
}

void CDefaultTextComponent::SetFont(IFont* _pkFont)
{
    m_pkFont = _pkFont;
}


void CDefaultTextComponent::SetText(const char* _pcText)
{
	const std::string kText = _pcText;
    m_kText = kText;
}

bool CDefaultTextComponent::ValidateText(entityx::Entity& _rkEntity)
{
    bool bRet = m_kText.isDirty();
    m_kText.clearDirty();
    return bRet;
}

const char* CDefaultTextComponent::GetText()
{
    return m_kText.get().c_str();
}

const std::string& CDefaultTextComponent::GetTextString()
{
    return m_kText.get();
}