#include "GLMMainPCH.h"
#include "GLITexture.h"

CGLITexture::CGLITexture(unsigned int _nTextureType)
    : m_eFormat(-1)
    , m_nAnisotropy(1)
    , m_nAlignment(4)
    , m_nTextureType(_nTextureType)
{
    m_anFilterMode [eFT_Min] = eFM_Linear;
    m_anFilterMode [eFT_Mag] = eFM_Linear;

    m_anClampMode [eCT_U] = eCM_Clamp;
    m_anClampMode [eCT_V] = eCM_Clamp;
    m_anClampMode [eCT_W] = eCM_Clamp;
}

CGLITexture::~CGLITexture()
{

}

bool CGLITexture::SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel)
{
    if (m_kPixelDatas.size() <= _nLevel) {
        m_kPixelDatas.resize(_nLevel + 1);
    }
    m_eFormat = _eTextureFormat;
    size_t nSize = (_pcData) ? GetPixelSize(_nWidth, _nHeight, _nDepth) : 0;

    ITexture::IPixelData* pkPixelData = m_kPixelDatas [_nLevel].get();
    if (pkPixelData == nullptr) {
        m_kPixelDatas [_nLevel] = AllocatePixelData(_nLevel);
        pkPixelData = m_kPixelDatas [_nLevel].get();
    }
    pkPixelData->m_nWidth = _nWidth;
    pkPixelData->m_nHeight = _nHeight;
    pkPixelData->m_nDepth = _nDepth;

    if (nSize) { // make has empty texture
        pkPixelData->SetData(_pcData, nSize);
    }
    return true;
}

bool CGLITexture::SetTextureDimension(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _nLevel)
{
    return SetTextureData(nullptr, _nWidth, _nHeight, _nDepth, m_eFormat, _nLevel);
}

int CGLITexture::GetTextureFormat()
{
    return m_eFormat;
}

void CGLITexture::SetTextureFormat(int _eTextureFormat)
{
    m_eFormat = _eTextureFormat;
}

void CGLITexture::SetAlignment(int _nAlign)
{
    if (_nAlign & (_nAlign - 1))
        return;
    m_nAlignment = _nAlign;
}

int CGLITexture::GetLevel()
{
    return m_kPixelDatas.size();
}

int CGLITexture::GetClampMode(EClampType _eType)
{
    if (_eType < eCT_U || _eType >= eCT_Max)
        return -1;
    return m_anClampMode [_eType];
}

int CGLITexture::GetFilterMode(EFilterType _eType)
{
    if (_eType < eFT_Min || _eType >= eFT_Max)
        return -1;
    return m_anFilterMode [_eType];
}

short CGLITexture::GetAnisotropy()
{
    return m_nAnisotropy;
}

bool CGLITexture::SetClampMode(EClampType _eType, EClampMode _nClamp)
{
    if (_eType < eCT_U || _eType >= eCT_Max)
        return false;
    m_anClampMode [_eType] = _nClamp;
    return true;
}

bool CGLITexture::SetFilterMode(EFilterType _eType, EFilterMode _nFilter)
{
    if (_eType < eFT_Min || _eType >= eFT_Max)
        return false;
    m_anFilterMode [_eType] = _nFilter;
    return true;
}

bool CGLITexture::SetAnisotropy(short _nAnisotropy)
{
    if ((_nAnisotropy & (_nAnisotropy - 1)))
        return false;
    m_nAnisotropy = _nAnisotropy;
    return true;
}

void CGLITexture::SetLevel(int _nLevel)
{
    if (m_kPixelDatas.size() == _nLevel)
        return;
    if (m_kPixelDatas.size() < _nLevel) {
        const size_t nOldSize = m_kPixelDatas.size();
        const size_t nAllocateCount = _nLevel - nOldSize;
        std::vector <std::shared_ptr <ITexture::IPixelData> > kTemp = m_kPixelDatas;
        m_kPixelDatas.resize(_nLevel);
        m_kPixelDatas = kTemp;
        for (size_t nIndex = 0; nIndex < nAllocateCount; nIndex++) {
            m_kPixelDatas [nIndex + nOldSize] = AllocatePixelData(nIndex + nOldSize);
        }
        return;
    }
    m_kPixelDatas.resize(_nLevel);
    short nLevel = 0;
    for (auto& spkPixelData : m_kPixelDatas) {
        spkPixelData = AllocatePixelData(nLevel++);
    }
}

size_t CGLITexture::GetPixelSize(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth)
{
    size_t nResolution = _nWidth * _nHeight * _nDepth;
    size_t nSize = -1;
    gli::format eFormat = (gli::format)m_eFormat;
    switch (eFormat) {
    case gli::FORMAT_R8_UNORM:
        nSize = 1;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RG8_UNORM:
        nSize = 2;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGB8_UNORM:
        nSize = 3;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGBA8_UNORM:
        nSize = 4;
        nSize *= nResolution;
        break;
    case gli::FORMAT_R8_SNORM:
        nSize = 1;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RG8_SNORM:
        nSize = 2;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGB8_SNORM:
        nSize = 3;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGBA8_SNORM:
        nSize = 4;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGBA4_UNORM:
        nSize = 2;
        nSize *= nResolution;
        break;
    case gli::FORMAT_D16_UNORM:
        nSize = 2;
        nSize *= nResolution;
        break;
    case gli::FORMAT_D24_UNORM:
        nSize = 3;
        nSize *= nResolution;
        break;
    case gli::FORMAT_D32_UFLOAT:
        nSize = 4;
        nSize *= nResolution;
    case gli::FORMAT_D24S8_UNORM:
        nSize = 4;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGB_DXT1_UNORM:
    {
        if (_nDepth > 1)
            nSize = ((_nWidth + 2) >> 2) * ((_nHeight + 2) >> 2) * ((_nDepth - 1 + 2) >> 2) * 16;
        else
            nSize = ((_nWidth + 2) >> 2) * ((_nHeight + 2) >> 2) * 16;
        break;
    }
    case gli::FORMAT_RGBA_DXT1_UNORM:
    case gli::FORMAT_RGBA_DXT3_UNORM:
    case gli::FORMAT_RGBA_DXT5_UNORM:
    {
        if (_nDepth > 1)
            nSize = ((_nWidth + 3) >> 2) * ((_nHeight + 3) >> 2) * ((_nDepth - 1 + 3) >> 2) * 16;
        else
            nSize = ((_nWidth + 3) >> 2) * ((_nHeight + 3) >> 2) * 16;
        break;
    }
    case gli::FORMAT_RGBA32_SFLOAT:
        nSize = 16;
        nSize *= nResolution;
        break;
    case gli::FORMAT_RGBA16_SFLOAT:
        nSize = 8;
        nSize *= nResolution;
        break;
    case gli::FORMAT_R32_SFLOAT:
        nSize = 4;
        nSize *= nResolution;
    default:
        break;
    }
    return nSize;
}

CEditableTexture::SEditablePixalData::SEditablePixalData(short _nLevel)
    :ITexture::IPixelData(_nLevel)
{

}

CEditableTexture::SEditablePixalData::~SEditablePixalData()
{

}

void CEditableTexture::SEditablePixalData::Allocate(size_t _nSize)
{
    if (!_nSize)
        return;

    ITexture::IPixelData::Allocate(_nSize);
    m_kPixelData.resize(_nSize);
}

void CEditableTexture::SEditablePixalData::SetData(const char* _pcData, size_t _nSize)
{
    if (!_nSize) {
        return;
    }
    if (!_pcData) {
        Allocate(_nSize);
        return;
    }

    ITexture::IPixelData::SetData(_pcData, _nSize);
    m_kPixelData.resize(_nSize);
    memcpy(m_kPixelData.data(), _pcData, _nSize);
}

char* CEditableTexture::SEditablePixalData::GetData()
{
    if (m_kPixelData.empty())
        return nullptr;
    return m_kPixelData.data();
}

void CEditableTexture::SEditablePixalData::ClearData()
{
    std::vector <char> kEmpty;
    m_kPixelData.swap(kEmpty);
}

CEditableTexture::CEditableTexture(unsigned int _nType)
    :CGLITexture(_nType)
{

}

CEditableTexture::~CEditableTexture()
{

}