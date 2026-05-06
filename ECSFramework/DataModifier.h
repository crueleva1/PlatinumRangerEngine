#pragma once


template <typename T>
class TDataModifier
{
    T m_Data;
    bool m_bDirty;
public:

    TDataModifier()
        : m_bDirty(true)
    {
        //memset(&m_Data, 0, sizeof(T));
    }

    TDataModifier(const T& _rkData)
        : m_bDirty(true)
    {
        m_Data = std::move(_rkData);
    }

    void clearDirty()
    {
        m_bDirty = false;
    }

    const T& get() const
    {
        return m_Data;
    }

    T& get()
    {
        return m_Data;
    }

    bool isDirty() const
    {
        return m_bDirty;
    }

    operator T() const
    {
        bool& bDirty = const_cast <bool&>(m_bDirty);
        bDirty = false;
        return m_Data;
    }

    void operator =(T& _rData)
    {
        if (m_Data == _rData)
            return;

        m_Data = std::move(_rData);
        m_bDirty = true;
    }

    void operator =(const T& _rData)
    {
        if (m_Data == _rData)
            return;

        m_Data = std::move(_rData);
        m_bDirty = true;
    }

    bool operator==(const T& _rData)
    {
        return (m_Data == _rData);
    }

    bool operator!=(const T& _rData)
    {
        return (m_Data != _rData);
    }
};