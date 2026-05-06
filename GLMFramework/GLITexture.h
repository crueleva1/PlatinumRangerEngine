#pragma once


#include <vector>
#include "DataModifier.h"
#include "ITexture.h"


class ECSGLM_API CGLITexture : public ITexture
{
protected:
    std::vector <std::shared_ptr <ITexture::IPixelData> > m_kPixelDatas;
    TDataModifier <int> m_anClampMode [eCT_Max];
    TDataModifier <int> m_anFilterMode [eFT_Max];
    unsigned int m_nTextureType;
    int m_eFormat;
    short m_nAnisotropy;
    short m_nAlignment;

    virtual std::shared_ptr <ITexture::IPixelData> AllocatePixelData(short _nLevel)
    {
        return std::make_shared <ITexture::IPixelData>(_nLevel);
    }

public:
    CGLITexture(unsigned int _nTextureType);

    virtual ~CGLITexture();

    virtual bool SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel = 0);

    virtual bool SetTextureDimension(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _nLevel = 0);

    virtual unsigned int GetWidth(int _nLevel = 0)
    {
        if (m_kPixelDatas.size() <= _nLevel)
            return 0;
        if (m_kPixelDatas [_nLevel] == nullptr)
            return 0;
        return m_kPixelDatas [_nLevel]->m_nWidth;
    }

    virtual unsigned int GetHeight(int _nLevel = 0)
    {
        if (m_kPixelDatas.size() <= _nLevel)
            return 0;
        if (m_kPixelDatas [_nLevel] == nullptr)
            return 0;
        return m_kPixelDatas [_nLevel]->m_nHeight;
    }

    virtual unsigned int GetDepth(int _nLevel = 0)
    {
        if (m_kPixelDatas.size() <= _nLevel)
            return 0;
        if (m_kPixelDatas [_nLevel] == nullptr)
            return 0;
        return m_kPixelDatas [_nLevel]->m_nDepth;
    }

    virtual unsigned int GetTextureType()
    {
        return m_nTextureType;
    }

    inline bool IsModeChanged()
    {
#define IF_RETURN(x,y,z) if (x==y) return z;
        for (int nIndex = eCT_U; nIndex < eCT_Max; nIndex++) {
            IF_RETURN(m_anClampMode [nIndex].isDirty(), true, true)
        }
        for (int nIndex = eFT_Min; nIndex < eFT_Max; nIndex++) {
            IF_RETURN(m_anFilterMode [nIndex].isDirty(), true, true)
        }
#undef IF_RETURN
        return false;
    }

    virtual int GetTextureFormat();

    virtual void SetTextureFormat(int _eTextureFormat);

    virtual void SetAlignment(int _nAlign);

    virtual int GetLevel();

    virtual int GetClampMode(EClampType);

    virtual int GetFilterMode(EFilterType);

    virtual short GetAnisotropy();

    virtual bool SetClampMode(EClampType _eType, EClampMode _nClamp);

    virtual bool SetFilterMode(EFilterType _eType, EFilterMode _nFilter);

    virtual bool SetAnisotropy(short _nAnisotropy);

    virtual void SetLevel(int _nLevel);

    virtual size_t GetPixelSize(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth);
};

class ECSGLM_API CEditableTexture : public CGLITexture
{
    struct SEditablePixalData : public ITexture::IPixelData
    {
        std::vector <char> m_kPixelData;

        SEditablePixalData(short _nLevel);

        virtual ~SEditablePixalData();

        virtual void Allocate(size_t _nSize);

        virtual void SetData(const char* _pcData, size_t _nSize);

        virtual char* GetData();

        virtual void ClearData();
    };

    virtual std::shared_ptr <ITexture::IPixelData> AllocatePixelData(short _nLevel)
    {
        return std::make_shared <CEditableTexture::SEditablePixalData>(_nLevel);
    }
public:
    CEditableTexture(unsigned int _nType);

    virtual ~CEditableTexture();
};