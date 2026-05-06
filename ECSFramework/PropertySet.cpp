#include "ECSFrameworkPCH.h"
#include "PropertySet.h"

template<> CPropertyFactory* TSingleton<CPropertyFactory>::ms_pkSingleton = nullptr;

std::string CPropertySet::m_kEmpty;

CPropertySet::CPropertySet()
{}

CPropertySet::CPropertySet(const CPropertySet* _pkSet)
{
    m_kProperty = _pkSet->m_kProperty;
}

CPropertySet::CPropertySet(const CPropertySet& _rkSet)
{
    m_kProperty = _rkSet.m_kProperty;
}

CPropertySet::~CPropertySet()
{}

const char* CPropertySet::GetProperty(const char* _pcKey) const
{
    auto kIt = m_kProperty.find(_pcKey);
    if (kIt == m_kProperty.end())
        return nullptr;

    return kIt->second.c_str();
}

const std::string& CPropertySet::GetPropertyStr(const char* _pcName) const
{
    auto kIt = m_kProperty.find(_pcName);
    if (kIt == m_kProperty.end())
        return m_kEmpty;

    return kIt->second;
}

bool CPropertySet::SetProperty(const char* _pcKey, const char* _pcValue)
{
    m_kProperty.insert(std::make_pair(_pcKey, _pcValue));
    return true;
}

bool CPropertySet::SetProperty(const std::string& _rkKey, const std::string& _rkValue)
{
    m_kProperty.insert(std::make_pair(_rkKey, _rkValue));
    return true;
}

bool CPropertySet::IsPropertyExist(const char* _pcKey) const
{
    return (m_kProperty.find(_pcKey) != m_kProperty.end());
}