#include "ECSFrameworkPCH.h"
#include "TextureDesc.h"

CTextureDesc::CTextureDesc(const char* _pcName, const char* _pcFileName, const char* _pcFileExt, int _TextureDim)
    : m_kName(_pcName)
    , m_kFileName(_pcFileName)
    , m_kFileExt(_pcFileExt)
    , m_nTextureDim(_TextureDim)
{

}

CTextureDesc::~CTextureDesc()
{

}

const char* CTextureDesc::GetName()
{
    return m_kName.c_str();
}

const char* CTextureDesc::GetFileName()
{
    return m_kFileName.c_str();
}

const char* CTextureDesc::GetFileExt()
{
    return m_kFileName.c_str();
}

int CTextureDesc::GetTextureDim()
{
    return m_nTextureDim;
}

ITexture* CTextureDesc::GetRenderData()
{
    return m_spkRenderData.get();
}

void CTextureDesc::SetRenderData(std::shared_ptr<ITexture> _spkRenderData)
{
    m_spkRenderData = _spkRenderData;
}
