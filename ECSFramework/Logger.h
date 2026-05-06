#pragma once

#include <memory>
#include <string>

enum ELogLevel
{
    eLL_None = -1,
    eLL_Error,
    eLL_Warning,
    eLL_Info,
    eLL_Verbose,
};


class ILogger
{
    static std::shared_ptr<ILogger> ms_spkLogger;
    ELogLevel m_eLevelThreshold = eLL_Info;

    virtual void Initialize() = 0;

    virtual void Finalize() = 0;

    virtual void OnLog(ELogLevel _eLevel, const char* _pcFormat, va_list _ArgList) = 0;

    virtual void OnSync(float _nDelta) = 0;
public:
    template <typename Logger, typename ... Args>
    static void Implement(Args&& ... args)
    {
        if (ms_spkLogger)
            ms_spkLogger->Finalize();
        ms_spkLogger = std::make_shared<Logger>(std::forward<Args>(args)...);
        ms_spkLogger->Initialize();
    }

    virtual ~ILogger();

    static const char* LogLevel2String(ELogLevel _eLevel);

    static void Log(ELogLevel _eLevel, std::string _kLog);

    static void Log(ELogLevel _eLevel, const char* _pkFormat, ...);

    static void Sync(float _nDelta);
    
    static void SetLogLevelTheshold(ELogLevel _eLevel);

    static ELogLevel GetCurrentTheshold();
};
