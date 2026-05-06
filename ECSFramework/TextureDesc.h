#pragma once


#include "ITexture.h"

class ECS_API CTextureDesc : public ITextureDesc
{
    std::shared_ptr <ITexture> m_spkRenderData;
    std::string m_kName;
    std::string m_kFileName;
    std::string m_kFileExt;
    int m_nTextureDim;
public:
    CTextureDesc(const char* _pcName, const char* _pcFileName, const char* _pcFileExt, int _nTextureType);

    virtual ~CTextureDesc();

    virtual const char* GetName();

    virtual const char* GetFileName();

    virtual const char* GetFileExt();

    virtual int GetTextureDim();

    virtual ITexture* GetRenderData();

    virtual void SetRenderData(std::shared_ptr <ITexture> _spkRenderData);
};