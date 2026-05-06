#pragma once

#include "GLITexture.h"

class ECS_OGL_API COpenGLTexture : public CEditableTexture
{
    GLuint m_nTexture;
    bool m_bDirty;
public:
    COpenGLTexture(unsigned int _nType);

    virtual ~COpenGLTexture();

    virtual bool Active(int _nStage);

    virtual bool BindTexture();

    virtual bool FlushTexture();

    virtual bool SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel = 0);

    virtual void SetTextureFormat(int _eTextureFormat);

    virtual char* GetTextureData(int _nLevel);

    virtual void SetLevel(int _nLevel);

    virtual bool SetAnisotropy(short _nAnisotropy);

    virtual bool IsCompressed(int _nLevel);

    GLint GetTextureBindingType();

    GLint GetGLTextureFormat();

    GLint GetGLTextureBaseFormat();

    GLint GetGLTextureDataType();

    GLint GetGLClampMode(EClampMode _eMode);

    GLint GetGLFilterMode(EFilterMode _eMode);

    inline const GLuint GetGLTexture()
    {
        return m_nTexture;
    }
};