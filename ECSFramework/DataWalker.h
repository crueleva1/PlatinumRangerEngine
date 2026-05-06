#pragma once

template <typename T>
class TDataWalker
{
    T* m_pHead;
    T* m_pCurr;
    const size_t m_nLen;
public:
    TDataWalker(T* _pData, size_t _nLen)
        :m_pHead(_pData)
        ,m_pCurr(_pData)
        ,m_nLen(_nLen)
    {
    }

    ~TDataWalker()
    {
    }

    inline size_t GetNext(size_t _nN)
    {
        size_t nCurr = m_pCurr - m_pHead;
        size_t nNext = _nN;
        if (nCurr + _nN > m_nLen) {
            nNext = m_nLen - nCurr;
        }
        return nNext;
    }

    template <size_t N>
    void Skip(T (&_pData)[N])
    {
        size_t nNext = GetNext(N);
        m_pCurr += nNext;
    }

    void Skip(const T* _pData, size_t _nLen)
    {
        size_t nNext = GetNext(_nLen);
        m_pCurr += nNext;
    }

    template <size_t N>
    bool Compare(T (&_pData)[N])
    {
        size_t nNext = GetNext(N);
        if (nNext != N)
            return false;
        size_t nDataSize = sizeof(T) * nNext;
        return (!memcmp(m_pCurr, _pData, nDataSize));
    }

    bool Compare(const T* _pData, size_t _nLen)
    {
        size_t nNext = GetNext(_nLen);
        if (nNext != _nLen)
            return false;
        size_t nDataSize = sizeof(T) * nNext;
        return (!memcmp(m_pCurr, _pData, nDataSize));
    }

    template <size_t N>
    void Write(const T (&_pData)[N])
    {
        size_t nNext = GetNext(N);
        memcpy(m_pCurr, _pData, nNext);
        m_pCurr += nNext;
    }

    void Write(const T* _pData, size_t _nLen)
    {
        size_t nNext = GetNext(_nLen);
        memcpy(m_pCurr, _pData, nNext);
        m_pCurr += nNext;
    }

    void Write(T _Data)
    {
        size_t nLen = m_pCurr - m_pHead;
        if (nLen >= m_nLen)
            return;
        *m_pCurr = _Data;
        m_pCurr++;
    }

    TDataWalker<T> operator++(int)
    {
        size_t nLen = m_pCurr - m_pHead;
        if (nLen >= m_nLen)
            return *this;
        m_pCurr++;
        return *this;
    }

    operator T() const
    {
        return *m_pCurr;
    }

    bool isEnd()
    {
        return (m_pCurr - m_pHead) == m_nLen;
    }

    const T* GetCurr()
    {
        return m_pCurr;
    }
};