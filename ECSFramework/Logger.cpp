#include "ECSFrameworkPCH.h"
#include "Logger.h"
#include <stdarg.h>

std::shared_ptr<ILogger> ILogger::ms_spkLogger = nullptr;

ILogger::~ILogger()
{

}

const char* ILogger::LogLevel2String(ELogLevel _eLevel)
{
    switch (_eLevel) {
    case eLL_Error:
        return "ERROR";
    case eLL_Warning:
        return "WARNING";
    case eLL_Info:
        return "IFNO";
    case eLL_Verbose:
        return "VERBOSE";
    default:
        break;
    }
    return "";
}

void ILogger::Log(ELogLevel _eLevel, std::string _kLog)
{
    if (ms_spkLogger == nullptr)
        return;

    if (ms_spkLogger->m_eLevelThreshold < _eLevel)
        return;

    Log(_eLevel, _kLog.c_str());
}

void ILogger::Log(ELogLevel _eLevel, const char* _pkFormat, ...)
{
    if (ms_spkLogger == nullptr)
        return;

    if (ms_spkLogger->m_eLevelThreshold < _eLevel)
        return;

    va_list kArgList;
    va_start(kArgList, _pkFormat);
    ms_spkLogger->OnLog(_eLevel, _pkFormat, kArgList);
    va_end(kArgList);
}

void ILogger::Sync(float _nDelta)
{
    if (ms_spkLogger == nullptr)
        return;
    ms_spkLogger->OnSync(_nDelta);
}

void ILogger::SetLogLevelTheshold(ELogLevel _eLevel)
{
    if (ms_spkLogger == nullptr)
        return;
    ms_spkLogger->m_eLevelThreshold = _eLevel;
}

ELogLevel ILogger::GetCurrentTheshold()
{
    if (ms_spkLogger == nullptr)
        return eLL_None;

    return ms_spkLogger->m_eLevelThreshold;
}