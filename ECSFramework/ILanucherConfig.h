#pragma once

#include <string>

class ILanucherConfig
{
public:
    virtual ~ILanucherConfig()
    {
    }

    virtual bool LoadConfig() = 0;

    virtual void UnLoadConfig() = 0;

    virtual bool ReLoadConfig() = 0;

    virtual float GetVersion() = 0;

    virtual const std::string& GetValueByString(const std::string& _rkKey) const = 0;

    inline int GetValueByInt(const std::string& _rkKey, int _nDefault = 0) const
    {
        const std::string& kValue = GetValueByString(_rkKey);
        if (kValue.empty() == false)
            return std::stoi(kValue);
        return _nDefault;
    }

    inline float GetValueByFloat(const std::string& _rkKey, float _nDefault = std::numeric_limits<float>::quiet_NaN()) const
    {
        const std::string& kValue = GetValueByString(_rkKey);
        if (kValue.empty() == false)
            return std::stof(kValue);
        return _nDefault;
    }

    inline bool GetValueByBool(const std::string& _rkKey, bool _bDefault = false) const
    {
        const std::string& kValue = GetValueByString(_rkKey);
        if ((kValue == "true") || (kValue == "True"))
            return true;
        else if ((kValue == "false") || (kValue == "False"))
            return false;
        return _bDefault;
    }

    inline bool IsValueExist(const std::string& _rkKey) const
    { 
        return (GetValueByString(_rkKey).empty() == false);
    }
};