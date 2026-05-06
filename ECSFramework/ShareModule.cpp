#include "ECSFrameworkPCH.h"
#include "ShareModule.h"

#ifdef _WIN32
#include "Windows.h"
#else
#include <dlfcn.h>
#endif

CShareModule::CShareModule(const std::string& _rkModuleName, bool _bAuto)
    :m_kModuleName(_rkModuleName)
    ,m_hModule(nullptr)
{
    if (_bAuto) {
        verify(InitializeModule());
    }
}

CShareModule::~CShareModule()
{
    ReleaseModule();
}

void* CShareModule::GetAddress(const std::string& _rkSymbo)
{
    if (m_hModule == nullptr)
        return nullptr;
#ifdef _WIN32
    return GetProcAddress((HMODULE)m_hModule, _rkSymbo.c_str());
#else
    return dlsym(m_hModule, _rkSymbo.c_str());
#endif
}

bool CShareModule::InitializeModule()
{
    if (m_hModule)
        return false;
#ifdef _WIN32
    m_hModule = LoadLibraryA(m_kModuleName.c_str());
#else
    m_hModule = dlopen(m_kModuleName.c_str(), RTLD_LAZY);
#endif
    return (m_hModule != nullptr);
}

void CShareModule::ReleaseModule()
{
    if (!m_hModule)
        return;
#ifdef _WIN32
    FreeLibrary((HMODULE)m_hModule);
#else
    dlclose(m_hModule);
#endif
    m_hModule = nullptr;
}