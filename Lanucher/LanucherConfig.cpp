#include "LanucherPCH.h"
#include "LanucherConfig.h"


CLanucherConfig::CLanucherConfig(const std::string& _rkConfigName, float _fVersion)
    :m_kSchemaName(_rkConfigName)
    ,m_fVersion(_fVersion)
{
}

CLanucherConfig::~CLanucherConfig()
{
}

bool CLanucherConfig::LoadConfig()
{
    FILE* pkFile = nullptr;
    pkFile = fopen(m_kSchemaName.c_str(), "rb");
    if (!pkFile)
        return false;
    if (fseek(pkFile, 0, SEEK_END)) {
        fclose(pkFile);
        return false;
    }
    size_t nSize = ftell(pkFile);
    rewind(pkFile);
    std::string kXmlData;
    kXmlData.resize(nSize);
    fread((void*)kXmlData.data(), nSize, 1, pkFile);
    fclose(pkFile);

    CRapidXMLDocument kDoc(*this, kXmlData.c_str(), kXmlData.size());

    return true;
}

void CLanucherConfig::UnLoadConfig()
{
    m_kData.clear();
}

bool CLanucherConfig::ReLoadConfig()
{
    UnLoadConfig();
    return LoadConfig();
}

const char* CLanucherConfig::GetSchemaName() const
{
    return m_kSchemaName.c_str();
}

void CLanucherConfig::OnElementStart(const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr)
{
    if (_rkElement == "Lanucher") {
        const rapidxml::xml_attribute<>* pkCurrAttr = &_rkAttr;

        while (pkCurrAttr) {
            std::string kKey = pkCurrAttr->name();
            if (kKey == "version") {
                std::string kValue = pkCurrAttr->value();
                float fConfigVersion = std::stof(kValue);
                assert(fConfigVersion <= m_fVersion);
                break;
            }
            pkCurrAttr = pkCurrAttr->next_attribute();
        }
    }
    else if (_rkElement == "Config") {
        const rapidxml::xml_attribute<>* pkCurrAttr = &_rkAttr;

        while (pkCurrAttr) {
#ifdef _DEBUG
            if (m_kData.find(pkCurrAttr->name()) != m_kData.end()) {
                // Log
            }
            else
#endif
            {
                // assigned data
                m_kData.insert(std::make_pair(pkCurrAttr->name(), pkCurrAttr->value()));
            }
            pkCurrAttr = pkCurrAttr->next_attribute();
        }
    }
}

void CLanucherConfig::OnElementEnd(const std::string& _rkElement)
{

}

void CLanucherConfig::OnText(const std::string& _rkText)
{

}