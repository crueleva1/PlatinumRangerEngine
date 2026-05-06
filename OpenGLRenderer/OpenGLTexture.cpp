#include "ECSOpenGLPCH.h"
#include "OpenGLTexture.h"

COpenGLTexture::COpenGLTexture(unsigned int _nType)
    : CEditableTexture(_nType)
    , m_nTexture(-1)
    , m_bDirty(true)
{
    glGenTextures(1, &m_nTexture);
}

COpenGLTexture::~COpenGLTexture()
{
    if (m_nTexture != -1)
        glDeleteTextures(1, &m_nTexture);
}

bool COpenGLTexture::Active(int _nStage)
{
    glActiveTexture(GL_TEXTURE0 + _nStage);
    return true;
}

bool COpenGLTexture::BindTexture()
{
    if (m_nTexture == -1) {
        glGenTextures(1, &m_nTexture);
        if (m_nTexture == -1)
            return false;
    }
    glBindTexture(m_nTextureType, m_nTexture);
    return FlushTexture();
}

bool COpenGLTexture::FlushTexture()
{
    bool bDirty = (m_bDirty || IsModeChanged());
    if (!bDirty)
        return true;
    GLint nCurrentTexture = 0;
    glGetIntegerv(GetTextureBindingType(), &nCurrentTexture);
    bool bOther = (nCurrentTexture != m_nTexture);
    if (bOther)
        glBindTexture(m_nTextureType, m_nTexture);
    if (m_bDirty) {
        m_bDirty = false;
        // Set Level
        glTexParameteri(m_nTextureType, GL_TEXTURE_BASE_LEVEL, 0);
        if (!m_kPixelDatas.empty())
            glTexParameteri(m_nTextureType, GL_TEXTURE_MAX_LEVEL, m_kPixelDatas.size() - 1);
        // Copy Data
        switch (m_nTextureType) {
        case GL_TEXTURE_2D:
        {
            unsigned int nFormat = GetGLTextureFormat();
            unsigned int nBaseFormat = GetGLTextureBaseFormat();
            unsigned int nType = GetGLTextureDataType();
            glPixelStorei(GL_UNPACK_ALIGNMENT, m_nAlignment);
            const size_t nCount = m_kPixelDatas.size();
            for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
                ITexture::IPixelData* pkData = m_kPixelDatas [nIndex].get();
                if (!pkData)
                    continue;
                if (nFormat == -1 || nType == -1)
                    continue;
                char* pcData = pkData->GetData();
                if (pcData) {
                    if (IsCompressed(nIndex)) {
                        glCompressedTexImage2D(m_nTextureType, nIndex, nFormat, pkData->m_nWidth, pkData->m_nHeight, 0, pkData->GetSize(), pcData);
                        // Save Client memory
                        pkData->ClearData();
                    }
                    else {
                        glTexImage2D(m_nTextureType, nIndex, nFormat, pkData->m_nWidth, pkData->m_nHeight, 0, nBaseFormat, nType, pcData);
                    }
                }
                else {
                    glTexImage2D(m_nTextureType, nIndex, nFormat, pkData->m_nWidth, pkData->m_nHeight, 0, nBaseFormat, nType, 0);
                }
            }
            break;
        }
        case GL_TEXTURE_3D:
        {
            unsigned int nFormat = GetGLTextureFormat();
            unsigned int nBaseFormat = GetGLTextureBaseFormat();
            unsigned int nType = GetGLTextureDataType();
            glPixelStorei(GL_UNPACK_ALIGNMENT, m_nAlignment);
            const size_t nCount = m_kPixelDatas.size();
            for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
                ITexture::IPixelData* pkData = m_kPixelDatas [nIndex].get();
                if (!pkData)
                    continue;
                if (nFormat == -1 || nType == -1)
                    continue;
                char* pcData = pkData->GetData();
                if (pcData) {
                    if (IsCompressed(nIndex)) {
                        glCompressedTexImage3D(m_nTextureType, nIndex, nFormat, pkData->m_nWidth, pkData->m_nHeight, pkData->m_nDepth, 0, pkData->GetSize(), pcData);
                        // Save Client memory
                        pkData->ClearData();
                    }
                    else {
                        glTexImage3D(m_nTextureType, nIndex, nFormat, pkData->m_nWidth, pkData->m_nHeight, pkData->m_nDepth, 0, nBaseFormat, nType, pcData);
                    }
                }
                else {
                    glTexImage3D(m_nTextureType, nIndex, nFormat, pkData->m_nWidth, pkData->m_nHeight, pkData->m_nDepth, 0, nBaseFormat, nType, 0);
                }
            }
            break;
        }
        case GL_TEXTURE_CUBE_MAP:
        {
            unsigned int nFormat = GetGLTextureFormat();
            unsigned int nBaseFormat = GetGLTextureBaseFormat();
            unsigned int nType = GetGLTextureDataType();
            const size_t nCount = (m_kPixelDatas.size() > 6) ? 6 : m_kPixelDatas.size();
            for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
                ITexture::IPixelData* pkData = m_kPixelDatas [nIndex].get();
                if (!pkData)
                    continue;
                if (nFormat == -1 || nType == -1)
                    continue;
                char* pcData = pkData->GetData();
                if (pcData) {
                    if (IsCompressed(nIndex)) {
                        glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + nIndex, 0, nFormat, pkData->m_nWidth, pkData->m_nHeight, 0, pkData->GetSize(), pcData);
                        // Save Client memory
                        pkData->ClearData();
                    }
                    else {
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + nIndex, 0, nFormat, pkData->m_nWidth, pkData->m_nHeight, 0, nBaseFormat, nType, pcData);
                    }
                }
                else {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + nIndex, 0, nFormat, pkData->m_nWidth, pkData->m_nHeight, 0, nBaseFormat, nType, 0);
                }
            }
            break;
        }
        default:
            break;
        }
    }
    if (IsModeChanged()) {
        switch (m_nTextureType) {
        case GL_TEXTURE_3D:
            glTexParameteri(m_nTextureType, GL_TEXTURE_WRAP_R, GetGLClampMode((EClampMode)m_anClampMode [eCT_W].get()));
        case GL_TEXTURE_2D:
            glTexParameteri(m_nTextureType, GL_TEXTURE_WRAP_T, GetGLClampMode((EClampMode)m_anClampMode [eCT_V].get()));
            glTexParameteri(m_nTextureType, GL_TEXTURE_WRAP_S, GetGLClampMode((EClampMode)m_anClampMode [eCT_U].get()));

        default:
            break;
        }

        GLint nMagMode = GetGLFilterMode((EFilterMode)m_anFilterMode [eFT_Mag].get());
        GLint nMinMode = GetGLFilterMode((EFilterMode)((m_kPixelDatas.size() > 1) ? m_anFilterMode [eFT_Min].get() : m_anFilterMode [eFT_Mag].get()));
        glTexParameteri(m_nTextureType, GL_TEXTURE_MAG_FILTER, nMagMode);
        glTexParameteri(m_nTextureType, GL_TEXTURE_MIN_FILTER, nMinMode);
        for (int nIndex = eCT_U; nIndex < eCT_Max; nIndex++) {
            m_anClampMode [nIndex].clearDirty();
        }
        for (int nIndex = eFT_Min; nIndex < eFT_Max; nIndex++) {
            m_anFilterMode [nIndex].clearDirty();
        }
    }
    if (bOther)
        glBindTexture(m_nTextureType, nCurrentTexture);
    return true;
}

bool COpenGLTexture::SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel)
{
    m_bDirty = CGLITexture::SetTextureData(_pcData, _nWidth, _nHeight, _nDepth, _eTextureFormat, _nLevel);
    return m_bDirty;
}

void COpenGLTexture::SetTextureFormat(int _eTextureFormat)
{
    CGLITexture::SetTextureFormat(_eTextureFormat);
    m_bDirty = true;
}

char* COpenGLTexture::GetTextureData(int _nLevel)
{
    if (m_kPixelDatas.size() <= _nLevel)
        return nullptr;
    if (m_kPixelDatas [_nLevel] == nullptr)
        return nullptr;
    return m_kPixelDatas [_nLevel]->GetData();
}

void COpenGLTexture::SetLevel(int _nLevel)
{
    CGLITexture::SetLevel(_nLevel);
    m_bDirty = true;
}

bool COpenGLTexture::SetAnisotropy(short _nAnisotropy)
{
    if (CGLITexture::SetAnisotropy(_nAnisotropy) == false)
        return false;

    glTexParameterf(m_nTextureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)_nAnisotropy);
    return true;
}

bool COpenGLTexture::IsCompressed(int _nLevel)
{
    if (m_kPixelDatas.size() <= _nLevel)
        return false;
    int nType = GetGLTextureBaseFormat();
    bool bRet = (nType == GL_COMPRESSED_RGBA);
    bRet |= (nType == GL_COMPRESSED_RGB);
    return bRet;
}



GLint COpenGLTexture::GetTextureBindingType()
{
    switch (m_nTextureType) {
    case GL_TEXTURE_2D:
        return GL_TEXTURE_BINDING_2D;
    case GL_TEXTURE_3D:
        return GL_TEXTURE_BINDING_3D;
    case GL_TEXTURE_CUBE_MAP:
        return GL_TEXTURE_BINDING_CUBE_MAP;
    default:
        break;
    }
    return GL_TEXTURE_BINDING_2D;
}

GLint COpenGLTexture::GetGLTextureFormat()
{
    GLint nFormat = -1;
    gli::format eFormat = (gli::format)m_eFormat;
    switch (eFormat) {
    case gli::FORMAT_R8_UNORM:
        nFormat = GL_RED;
        break;
    case gli::FORMAT_RG8_UNORM:
        nFormat = GL_RG8;
        break;
    case gli::FORMAT_RGB8_UNORM:
        nFormat = GL_RGB8;
        break;
    case gli::FORMAT_RGBA8_UNORM:
        nFormat = GL_RGBA8;
        break;
    case gli::FORMAT_R8_SNORM:
        nFormat = GL_RED;
        break;
    case gli::FORMAT_RG8_SNORM:
        nFormat = GL_RG8;
        break;
    case gli::FORMAT_RGB8_SNORM:
        nFormat = GL_RGB8;
        break;
    case gli::FORMAT_RGBA8_SNORM:
        nFormat = GL_RGBA8;
        break;
    case gli::FORMAT_RGBA4_UNORM:
        nFormat = GL_RGBA4;
        break;
    case gli::FORMAT_D16_UNORM:
        nFormat = GL_DEPTH_COMPONENT16;
        break;
    case gli::FORMAT_D24_UNORM:
        nFormat = GL_DEPTH_COMPONENT24;
        break;
    case gli::FORMAT_D32_UFLOAT:
        nFormat = GL_DEPTH_COMPONENT32;
        break;
    case gli::FORMAT_D24S8_UNORM:
        nFormat = GL_DEPTH24_STENCIL8;
        break;
    case gli::FORMAT_RGB_DXT1_UNORM:
        nFormat = GL_COMPRESSED_RGB;
        break;
    case gli::FORMAT_RGBA_DXT1_UNORM:
        nFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case gli::FORMAT_RGBA_DXT3_UNORM:
        nFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case gli::FORMAT_RGBA_DXT5_UNORM:
        nFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    case gli::FORMAT_RGBA32_SFLOAT:
        nFormat = GL_RGBA32F;
        break;
    case gli::FORMAT_RGBA16_SFLOAT:
        nFormat = GL_RGBA16F;
        break;
    case gli::FORMAT_R32_SFLOAT:
        nFormat = GL_R32F;
        break;
    default:
        break;
    }
    return nFormat;
}

GLint COpenGLTexture::GetGLTextureBaseFormat()
{
    GLint nFormat = -1;
    gli::format eFormat = (gli::format)m_eFormat;
    switch (eFormat) {
    case gli::FORMAT_R8_UNORM:
        nFormat = GL_RED;
        break;
    case gli::FORMAT_RG8_UNORM:
        nFormat = GL_RG;
        break;
    case gli::FORMAT_RGB8_UNORM:
        nFormat = GL_RGB;
        break;
    case gli::FORMAT_RGBA8_UNORM:
        nFormat = GL_RGBA;
        break;
    case gli::FORMAT_R8_SNORM:
        nFormat = GL_RED;
        break;
    case gli::FORMAT_RG8_SNORM:
        nFormat = GL_RG;
        break;
    case gli::FORMAT_RGB8_SNORM:
        nFormat = GL_RGB;
        break;
    case gli::FORMAT_RGBA8_SNORM:
        nFormat = GL_RGBA;
        break;
    case gli::FORMAT_RGBA4_UNORM:
        nFormat = GL_RGBA;
        break;
    case gli::FORMAT_D16_UNORM:
        nFormat = GL_DEPTH_COMPONENT;
        break;
    case gli::FORMAT_D24_UNORM:
        nFormat = GL_DEPTH_COMPONENT;
        break;
    case gli::FORMAT_D32_UFLOAT:
        nFormat = GL_DEPTH_COMPONENT;
        break;
    case gli::FORMAT_D24S8_UNORM:
        nFormat = GL_DEPTH_STENCIL;
        break;
    case gli::FORMAT_RGB_DXT1_UNORM:
        nFormat = GL_RGB;
        break;
    case gli::FORMAT_RGBA_DXT1_UNORM:
        nFormat = GL_COMPRESSED_RGBA;
        break;
    case gli::FORMAT_RGBA_DXT3_UNORM:
        nFormat = GL_COMPRESSED_RGBA;
        break;
    case gli::FORMAT_RGBA_DXT5_UNORM:
        nFormat = GL_COMPRESSED_RGBA;
        break;
    case gli::FORMAT_RGBA32_SFLOAT:
        nFormat = GL_RGBA;
        break;
    case gli::FORMAT_R32_SFLOAT:
        nFormat = GL_RED;
        break;
    default:
        break;
    }
    return nFormat;
}

GLint COpenGLTexture::GetGLTextureDataType()
{
    GLint nType = -1;
    gli::format eFormat = (gli::format)m_eFormat;
    switch (eFormat) {
    case gli::FORMAT_R8_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RG8_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGB8_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGBA8_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_R8_SNORM:
        nType = GL_BYTE;
        break;
    case gli::FORMAT_RG8_SNORM:
        nType = GL_BYTE;
        break;
    case gli::FORMAT_RGB8_SNORM:
        nType = GL_BYTE;
        break;
    case gli::FORMAT_RGBA8_SNORM:
        nType = GL_BYTE;
        break;
    case gli::FORMAT_RGBA4_UNORM:
        nType = GL_UNSIGNED_SHORT_4_4_4_4;
        break;
    case gli::FORMAT_D16_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_D24_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_D32_UFLOAT:
        nType = GL_FLOAT;
        break;
    case gli::FORMAT_D24S8_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGB_DXT1_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGBA_DXT1_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGBA_DXT3_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGBA_DXT5_UNORM:
        nType = GL_UNSIGNED_BYTE;
        break;
    case gli::FORMAT_RGBA32_SFLOAT:
        nType = GL_FLOAT;
        break;
    case gli::FORMAT_RGBA16_SFLOAT:
        nType = GL_FLOAT;
        break;
    case gli::FORMAT_R32_SFLOAT:
        nType = GL_FLOAT;
        break;
    default:
        break;
    }
    return nType;
}

GLint COpenGLTexture::GetGLClampMode(EClampMode _eMode)
{
    GLint nMode = GL_CLAMP;
    switch (_eMode)
    {
    case eCM_Clamp:
        nMode = GL_CLAMP;
        break;
    case eCM_ClampToEdge:
        nMode = GL_CLAMP_TO_EDGE;
        break;
    case eCM_ClampToBorder:
        nMode = GL_CLAMP_TO_BORDER;
        break;
    case eCM_MirroredRepeat:
        nMode = GL_MIRRORED_REPEAT;
        break;
    case eCM_Repeat:
        nMode = GL_REPEAT;
        break;
    case eCM_MirrorClampToEdge:
        nMode = GL_MIRROR_CLAMP_TO_EDGE;
        break;
    default:
        break;
    }
    return nMode;
}

GLint COpenGLTexture::GetGLFilterMode(EFilterMode _eMode)
{
    GLint nMode = GL_LINEAR;
    switch (_eMode)
    {
    case eFM_Linear:
        nMode = GL_LINEAR;
        break;
    case eFM_Nearest:
        nMode = GL_NEAREST;
        break;
    case eFM_Linear_Mipmap_Linear:
        nMode = GL_LINEAR_MIPMAP_LINEAR;
        break;
    case eFM_Linear_Mipmap_Nearest:
        nMode = GL_LINEAR_MIPMAP_NEAREST;
        break;
    case eFM_Nearest_Mipmap_Linear:
        nMode = GL_NEAREST_MIPMAP_LINEAR;
        break;
    case eFM_Nearest_Mipmap_Nearest:
        nMode = GL_NEAREST_MIPMAP_NEAREST;
        break;
    default:
        break;
    }
    return nMode;
}
