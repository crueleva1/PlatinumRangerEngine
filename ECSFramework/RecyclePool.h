#pragma once


#include <vector>
#include <set>

template <typename T, typename TIndex, TIndex nStartID = 0>
class CRecyclePool
{
    std::vector <T> m_kPool;
    std::set <TIndex> m_kRecycled;
public:

    CRecyclePool()
    {
    }

    ~CRecyclePool()
    {
    }

    template <typename TAllocator, typename ... Args >
    TIndex Allocate(Args&&... _Args)
    {
        TIndex nIndex;
        if (m_kRecycled.empty()) {
            T pkData = TAllocator::Create(_Args...);
            nIndex = m_kPool.size() + nStartID;
            m_kPool.push_back(pkData);
        }
        else {
            auto kIt = m_kRecycled.begin();
            nIndex = (*kIt);
            nIndex += nStartID;
            T pkData = m_kPool [nIndex];
            TAllocator::SetData(pkData, _Args...);
            m_kRecycled.erase(kIt);
        }
        return nIndex;
    }

    T GetData(TIndex _nID)
    {
        _nID -= nStartID;
        return m_kPool [_nID];
    }

    bool IsAllocated(TIndex _nID)
    {
        _nID -= nStartID;
        const TIndex nSize = (TIndex)m_kPool.size();
        if (nSize <= _nID)
            return false;
        return (m_kRecycled.find(_nID) == m_kRecycled.end());
    }

    bool Recycle(TIndex _nID)
    {
        if (!IsAllocated(_nID))
            return false;

        _nID -= nStartID;
        m_kRecycled.insert(_nID);
        return true;
    }

    void RecycleAll()
    {
        m_kRecycled.clear();
        TIndex nIndex = (TIndex)m_kPool.size();
        while (nIndex) {
            m_kRecycled.insert(--nIndex);
        }
    }
};