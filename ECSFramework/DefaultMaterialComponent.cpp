#include "ECSFrameworkPCH.h"
#include "DefaultMaterialComponent.h"

CDefaultMaterialComponent::CDefaultMaterialComponent()
    :m_pkMaterial(nullptr)
{

}

CDefaultMaterialComponent::~CDefaultMaterialComponent()
{

}

void CDefaultMaterialComponent::SetMaterialInstance(IMaterial* _pkMaterial)
{
    m_pkMaterial = _pkMaterial;
}

IMaterial* CDefaultMaterialComponent::GetMaterialInstance()
{
    return m_pkMaterial;
}