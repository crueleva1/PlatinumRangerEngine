#pragma once

#include <string>


class ECS_API CShareModule
{
    void* m_hModule;
    std::string m_kModuleName;
public:
    CShareModule(const std::string& _rkModuleName, bool _bAuto = true);

    virtual ~CShareModule();

    const std::string& GetModuleName()
    {
        return m_kModuleName;
    }

    void* GetAddress(const std::string& _rkSymbo);

    template <typename T>
    inline typename T GetAddressByType(const std::string& _rkSymbo)
    {
        return static_cast<T>(GetAddress(_rkSymbo));
    }

    virtual bool InitializeModule();

    virtual void ReleaseModule();
};