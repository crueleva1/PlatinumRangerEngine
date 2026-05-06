#pragma once

#include <map>
#include <string>
#include "Factory.h"


class ECS_API IPropertyListener
{
public:
    IPropertyListener() {}
    virtual ~IPropertyListener() {}
};

class ECS_API CProperty
{
protected:
    std::string m_kName;
    std::string m_kHelp;
    std::string m_kDefault;
    std::string m_kDataType;
    std::string m_kOrigin;
    bool m_bWriteXML;
                     
    
public:
    CProperty(const std::string& _rkName, const std::string& _rkHelp, const std::string& _rkDefaultValue = "", const std::string& _rkDataType = "Unknown", const std::string& _rkOrigin = "Unknown", bool _bWritesXML = true) :
        m_kName(_rkName),
        m_kHelp(_rkHelp),
        m_kDefault(_rkDefaultValue),
        m_bWriteXML(_bWritesXML),
        m_kDataType(_rkDataType),
        m_kOrigin(_rkOrigin)
    {
    }

    virtual ~CProperty() 
    {
    }


    const std::string& getHelp() const
    {
        return m_kHelp;
    }

    const std::string& getName() const
    {
        return m_kName;
    }

    const std::string& getDataType() const
    {
        return m_kDataType;
    }

    const std::string& getOrigin() const 
    {
        return m_kOrigin;
    }

    virtual std::string    get(const IPropertyListener* receiver) const = 0;

    virtual void set(IPropertyListener* receiver, const std::string& value) = 0;

    virtual bool isDefault(const IPropertyListener* receiver) const
    {
        return false;
    }

    virtual std::string    getDefault(const IPropertyListener* receiver) const
    {
        return m_kDefault;
    }

    virtual bool isReadable() const 
    {
        return true;
    }

    virtual bool isWritable() const 
    {
        return false;
    }

    virtual bool doesWriteXML() const
    {
        return m_bWriteXML;
    }

    virtual void initialisePropertyReceiver(IPropertyListener* /*receiver*/) const 
    {
    }

    virtual CProperty* clone() const = 0;
};

class ECS_API CPropertyFactory 
    : public TSingleton<CPropertyFactory>
    , public IFactoryMgr<CProperty, std::string>
{
public:
    CPropertyFactory()
    {
    }

    virtual ~CPropertyFactory()
    {
    }
};

class ECS_API CPropertySet
{
    static std::string m_kEmpty;
#if 0       // Preparing Property Binding
    std::map <std::string, std::shared_ptr<CProperty> > m_kProperty;
#else
    std::map <std::string, std::string > m_kProperty;
#endif
public:
    CPropertySet();

    CPropertySet(const CPropertySet* _pkSet);

    CPropertySet(const CPropertySet& _rkSet);

    virtual ~CPropertySet();

    const char* GetProperty(const char* _pcKey) const;

    const std::string& GetPropertyStr(const char* _pcName) const;

    bool SetProperty(const char* _pcKey, const char* _pcValue);

    bool SetProperty(const std::string& _rkKey, const std::string& _rkValue);

    bool IsPropertyExist(const char* _pcKey) const;

    bool RegisterProperty(std::shared_ptr <CProperty> _spkProperty);

    bool UnRegisterProperty(const std::string& _rkKey);
};