#pragma once

#include <string>
#include <vector>
#include <math.h>
#include <assert.h>
#include <numeric>
#include "RecyclePool.h"

static void SplitToken(const std::string& _rkIn, char _cDelimiter, std::vector <std::string>& _rkOut)
{
    if (_rkIn.empty())
        return;

    const char* pcHead = _rkIn.c_str();
    const char* pcCurr = nullptr;
    size_t nOffset = 0;
    size_t nLength = 0;
    while ((pcCurr = pcHead + nOffset) != '\0') {

        bool bEof = (nLength > _rkIn.size());

        if ((*pcCurr == _cDelimiter) || bEof) {
            std::string kSubStr(pcHead, pcCurr - pcHead);
            _rkOut.push_back(kSubStr);
            pcHead = pcCurr + 1;
            nLength += 1;
            nOffset = 0;
            if (!bEof)
                continue;
            break;;
        }

        nLength++;
        nOffset++;
    }
}

template <typename T>
struct SArrayDeleter
{
    void operator ()(const T* pArray)
    {
        delete [] pArray;
    }
};

#define MATH_PI 3.14159265359

static float DegreeToRadian(float _nDegree)
{
    return _nDegree * (MATH_PI / 180.0f);
}

static float RadianToDegree(float _nRadian)
{
    return _nRadian * (180.0f / MATH_PI);
}

inline void SinCos(float _nRadian, float& _nSinvalue, float& _nCosvalue)
{
#if defined(_WIN32)
#if 0   // x64 not support
    unsigned short nCwd_flag = 0;

    __asm
    {
        fstsw [nCwd_flag]
    }

    int nStackTop = (nCwd_flag & 0x3800) >> 11;

    if (nStackTop < 7) {
        __asm
        {
            mov eax, dword ptr [_nCosvalue]
            mov ebx, dword ptr [_nSinvalue]
            fld [_nRadian]
            fsincos
            fstp dword ptr [eax]
            fstp dword ptr [ebx]
        }
    }
    else {
        float s0, s1;
        __asm
        {
            fstp [s0]
            fstp [s1]
            mov eax, dword ptr [_nCosvalue]
            mov ebx, dword ptr [_nSinvalue]
            fld [_nRadian]
            fsincos
            fstp dword ptr [eax]
            fstp dword ptr [ebx]
            fld [s1]
            fld [s0]
        }
    }
#else
    _nSinvalue = std::sinf(_nRadian);
    _nCosvalue = std::cosf(_nRadian);
#endif

#else
    // Linux
    sincos(_nRadian, &_nSinvalue, &_nCosvalue);
#endif
}

#ifndef verify
#define verify(x)   if (!x) { assert(0); }
#endif

template <size_t nFirst, size_t nLast>
struct static_for
{
    template <typename Functor>
    inline void operator()(const Functor& _rkFunctor) const
    {
        if (nFirst < nLast) {
            if (_rkFunctor(nFirst))
                return;
            static_for<nFirst + 1, nLast>()(_rkFunctor);
        }
    }
};

template <size_t N>
struct static_for<N, N>
{
    template <typename Functor>
    inline void operator()(const Functor& _rkFunctor) const
    {
    }
};

template <size_t nBitCount, size_t nOffset = 0>
size_t GetBitPos(size_t _nValue)
{
    size_t nPos = -1;
    static_for <nOffset, nBitCount + nOffset>()([&] (size_t _nIndex)
    {
        size_t nBit = 1 << _nIndex;
        bool bRet = (_nValue & nBit) ? true : false;
        if (bRet)
            nPos = _nIndex - nOffset;
        return bRet;
    });
    return nPos;
}

template <typename T, size_t nMax = std::numeric_limits<T>::max(), size_t nMin = std::numeric_limits<T>::min()>
T SetRangedValue(const T& _nValue)
{
    if (_nValue > nMax)
        return nMax;
    if (_nValue < nMin)
        return nMin;
    return _nValue;
}

template <typename T> 
class TSingleton
{
protected:
    static T* ms_pkSingleton;

public:
    TSingleton()
    {
        assert(!ms_pkSingleton);
        ms_pkSingleton = static_cast<T*>(this);
    }

    virtual ~TSingleton()
    {
        assert(ms_pkSingleton);
        ms_pkSingleton = 0;
    }

    template <typename ... Args>
    static void New(Args&& ... args)
    {
        if (ms_pkSingleton != nullptr)
            return;
        ms_pkSingleton = new T(std::forward<Args>(args) ...);
    }

    static void Delete()
    {
        if (ms_pkSingleton == nullptr)
            return;

        delete ms_pkSingleton;
    }

    static T& GetInstance()
    {
        assert(ms_pkSingleton);
        return (*ms_pkSingleton);
    }

    static T* GetInstancePtr()
    {
        return (ms_pkSingleton);
    }

private:
    TSingleton& operator=(const TSingleton&) = delete;
    TSingleton(const TSingleton&) = delete;
};
