#pragma once

#include <map>
#include <memory>

template <class InterfaceType, typename EnumType>
struct IFactoryObj
{
    const EnumType m_kType;
    IFactoryObj(const EnumType& _kType)
        : m_kType(_kType)
    {
    }

    template <class T, class ...Args>
    InterfaceType* Create(Args&& ..._Args)
    {
        return new T(static_cast <Args&&> (_Args)...);
    }

    template <class ...Args>
    std::shared_ptr <InterfaceType> Create(Args&& ..._Args)
    {
        return std::make_shared <T>(_Args);
    }

    inline EnumType GetType()
    {
        return m_kType;
    }
};

template <class InterfaceType, typename EnumType>
class IFactoryMgr
{
protected:
    std::map <EnumType, std::shared_ptr <IFactoryObj <InterfaceType, EnumType> > > m_kFactorys;
public:
    template <typename Factory>
    typename Factory* GetFactoryByType (EnumType _Type)
    {
        auto kIt = m_kFactorys.find (_Type);
        if (kIt == m_kFactorys.end ())
            return nullptr;

        return static_cast <Factory*> (kIt->second.get());
    }

    IFactoryMgr ()
    {}

    virtual ~IFactoryMgr ()
    {
        m_kFactorys.clear();
    }

    bool RegisterFactory (std::shared_ptr <IFactoryObj <InterfaceType, EnumType> > _pkFactory)
    {
        if (!_pkFactory)
            return false;

        auto kIt = m_kFactorys.find(_pkFactory->GetType ());
        if (kIt != m_kFactorys.end()) {
            return false;
        }

        m_kFactorys.insert (std::make_pair(_pkFactory->GetType(), _pkFactory));
        return true;
    }

    bool UnRegisterFactory (EnumType& _kType)
    {
        auto kIt = m_kFactorys.find (_kType);
        if (kIt == m_kFactorys.end ()) {
            return false;
        }

        m_kFactorys.erase (kIt);
        return true;
    }

    inline bool IsFactoryExist (EnumType _eType)
    {
        return (m_kFactorys.find (_eType) != m_kFactorys.end ());
    }
};