#pragma once


template<class ComponentInterface>
class TComponent
{
    std::shared_ptr <ComponentInterface> m_spkComponent;
public:
    TComponent(std::shared_ptr <ComponentInterface> _spkComponent)
        : m_spkComponent(_spkComponent)
    {

    }

    ~TComponent()
    {
    }

    inline ComponentInterface* operator -> ()
    {
        return m_spkComponent.get();
    }
    inline const ComponentInterface* operator -> () const
    {
        return m_spkComponent.get();
    }

    inline ComponentInterface* get()
    {
        return m_spkComponent.get();
    }
    inline const ComponentInterface* get() const
    {
        return m_spkComponent.get();
    }

    inline std::weak_ptr<ComponentInterface> MakeWeakRef()
    {
        return m_spkComponent;
    }
};