#pragma once

#include "ILanucherConfig.h"
#include "XmlParser.h"

class CLanucherConfig 
    : public ILanucherConfig
    , public IXMLHandler
{
    std::map <std::string, std::string> m_kData;
    std::string m_kSchemaName;
    float m_fVersion;
public:
    CLanucherConfig(const std::string& _rkConfigName, float fVersion);

    virtual ~CLanucherConfig();

    bool LoadConfig();

    void UnLoadConfig();

    bool ReLoadConfig();

    virtual const char* GetSchemaName() const;

    virtual void OnElementStart(const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr);

    virtual void OnElementEnd(const std::string& _rkElement);

    virtual void OnText(const std::string& _rkText);

    float GetVersion()
    {
        return m_fVersion;
    }

    const std::string& GetValueByString(const std::string& _rkKey) const
    {
        static std::string gs_kDefault;
        auto kIt = m_kData.find(_rkKey);
        if (kIt != m_kData.end())
            return kIt->second;
        return gs_kDefault;
    }
};