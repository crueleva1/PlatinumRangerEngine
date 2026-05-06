#include "MainPCH.h"
#include "NanoLoggerSystem.h"
#include "NanoLog.hpp"

class CNanoLogger : public ILogger
{
    std::string m_kDirName;
    std::string m_kFileName;
    unsigned int m_nLoggerFileMBSize;
    unsigned int m_nLoggerBufferMBSize;
public:
    CNanoLogger()
        : m_kFileName("ModelViewLog.log")
        , m_nLoggerBufferMBSize(512)
    {

    }

    virtual ~CNanoLogger()
    {

    }

    virtual void Initialize()
    {
        nanolog::initialize(nanolog::NonGuaranteedLogger(m_nLoggerBufferMBSize), m_kDirName, m_kFileName, 2048);
    }

    virtual void Finalize()
    {

    }

    virtual void OnLog(ELogLevel _eLevel, const char* _pcFormat, va_list _ArgList)
    {
        nanolog::LogLevel eLevel = nanolog::LogLevel::INFO;
        switch (_eLevel)
        {
        case eLL_Error:
            eLevel = nanolog::LogLevel::CRIT;
            break;
        case eLL_Warning:
            eLevel = nanolog::LogLevel::WARN;
            break;
        case eLL_Info:
            eLevel = nanolog::LogLevel::INFO;
            break;
        case eLL_Verbose:
            eLevel = nanolog::LogLevel::INFO;
            break;
        case eLL_None:
        default:
            return;
        }
        if (!nanolog::is_logged(eLevel))
            return;

        nanolog::NanoLogLine kLine(eLevel, __FILE__, __func__, __LINE__);
        static char cBuffer[2048] = {};
        vsprintf_s(cBuffer, sizeof(cBuffer), _pcFormat, _ArgList);

        // console
        printf("NanoLogger Level:%s Message:%s", ILogger::LogLevel2String(_eLevel), cBuffer);

        kLine << cBuffer;
        nanolog::NanoLog() == kLine;
    }

    virtual void OnSync(float _nDelta)
    {
        
    }
};

CNanoLoggerSystem::CNanoLoggerSystem()
{
}

CNanoLoggerSystem::~CNanoLoggerSystem()
{

}

void CNanoLoggerSystem::configure(entityx::EntityManager& _kEntitieMgr, entityx::EventManager& _kEventMgr)
{
    ILogger::Implement<CNanoLogger>();
    ILogger::SetLogLevelTheshold(eLL_Info);
}