#include "ECSOpenGLPCH.h"
#include "OpenGLMaterial.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"

COpenGLFrameBuffer::COpenGLFrameBuffer(int _nWidth, int _nHeight)
    : IOpenGLFrameBufferInterface (_nWidth, _nHeight)
    , m_nFrameBufferObject(-1)
    , m_nDepthRenderTarget(-1)
{
    glGenFramebuffers(1, &m_nFrameBufferObject);
}

COpenGLFrameBuffer::~COpenGLFrameBuffer()
{
    m_kRenderTexture.clear();
    glDeleteFramebuffers(1, &m_nFrameBufferObject);
    if (m_nDepthRenderTarget != -1)
        glDeleteRenderbuffers(1, &m_nDepthRenderTarget);
}

GLuint COpenGLFrameBuffer::GetFrameBufferObjectID()
{
    return m_nFrameBufferObject;
}

void COpenGLFrameBuffer::SetRenderBufferColorAttactmentCount(int _nCount)
{
    if (_nCount <= m_kRenderTexture.size()) {
        GLint nCurrentFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
        for (size_t nIndex = _nCount; nIndex < m_kRenderTexture.size(); ++nIndex) {
            std::shared_ptr <ITexture> spkTexture = m_kRenderTexture [nIndex];
            if (!spkTexture)
                continue;
            int nTextureType = spkTexture->GetTextureType();
            switch (nTextureType) {
            case GL_TEXTURE_2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nIndex, GL_TEXTURE_2D, -1, 0);
                break;
            case GL_TEXTURE_3D:
                glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nIndex, GL_TEXTURE_3D, -1, 0, 0);
                break;
            default:
                break;
            }
        }
        m_kRenderTexture.resize(_nCount);
        glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);
    }
    else {
        size_t nOldSize = m_kRenderTexture.size();
        m_kRenderTexture.resize(_nCount);
    }
}

int COpenGLFrameBuffer::GetRenderBufferColorAttachmentCount()
{
    return m_kRenderTexture.size();
}

bool COpenGLFrameBuffer::SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment)
{
    if (!_rspkTexture)
        return false;
    COpenGLTexture* pkGLTexture = dynamic_cast <COpenGLTexture*>(_rspkTexture.get());
    if (!pkGLTexture)
        return false;

    switch (pkGLTexture->GetTextureFormat()) {
    case gli::FORMAT_D16_UNORM:
    case gli::FORMAT_D24_UNORM:
    case gli::FORMAT_D24S8_UNORM:
    case gli::FORMAT_D32_UFLOAT:
    case gli::FORMAT_D32_UFLOAT_S8_UNORM:
        return false;
    default:
        break;
    }

    if (m_kRenderTexture.size() <= _nAttachment)
        return false;

    bool bRet = true;
    GLint nCurrentFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    pkGLTexture->SetLevel(1);
    pkGLTexture->SetTextureData(nullptr, pkGLTexture->GetWidth(), pkGLTexture->GetHeight(), 0, pkGLTexture->GetTextureFormat());
#if 0   // Custom for other
    pkGLTexture->SetFilterMode(eFT_Min, GL_LINEAR);
    pkGLTexture->SetFilterMode(eFT_Mag, GL_LINEAR);
    pkGLTexture->SetClampMode(eCT_U, GL_CLAMP_TO_EDGE);
    pkGLTexture->SetClampMode(eCT_V, GL_CLAMP_TO_EDGE);
#endif
    pkGLTexture->FlushTexture(); // flush Texture

    int nTextureType = pkGLTexture->GetTextureType();
    switch (nTextureType) {
    case GL_TEXTURE_2D:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _nAttachment, GL_TEXTURE_2D, pkGLTexture->GetGLTexture(), 0);
        break;
    case GL_TEXTURE_3D:
        glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _nAttachment, GL_TEXTURE_3D, pkGLTexture->GetGLTexture(), 0, 0);
        break;
    default:
        bRet = false;
        break;
    }

    GLenum eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (eStatus != GL_FRAMEBUFFER_COMPLETE)
        bRet = false;

    glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);

    if (bRet) {
        m_kRenderTexture [_nAttachment] = _rspkTexture;
    }

    return bRet;
}

bool COpenGLFrameBuffer::SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue)
{
    if (!_rspkDepthTextue) {
        if (m_nDepthRenderTarget != -1) {
            GLint nCurrentFrameBuffer = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
            std::shared_ptr <ITexture> spkTexture = m_spkDepthTexture;
            if (spkTexture) {
                glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthRenderTarget);
                int nTextureType = spkTexture->GetTextureType();
                switch (nTextureType) {
                case GL_TEXTURE_2D:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, -1, 0);
                    break;
                case GL_TEXTURE_3D:
                    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, -1, 0, 0);
                    break;
                default:
                    break;
                }
            }
            glDeleteRenderbuffers(1, &m_nDepthRenderTarget);
            glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);
        }
        return false;
    }

    COpenGLTexture* pkGLTexture = dynamic_cast <COpenGLTexture*>(_rspkDepthTextue.get());
    if (!pkGLTexture)
        return false;

    switch (_rspkDepthTextue->GetTextureFormat()) {
    case gli::FORMAT_D16_UNORM:
    case gli::FORMAT_D24_UNORM:
    case gli::FORMAT_D24S8_UNORM:
    case gli::FORMAT_D32_UFLOAT:
    case gli::FORMAT_D32_UFLOAT_S8_UNORM:
        break;
    default:
        return false;
    }

    if (m_nDepthRenderTarget == -1) {
        glGenRenderbuffers(1, &m_nDepthRenderTarget);
    }

    bool bRet = true;
    GLint nCurrentFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthRenderTarget);

    pkGLTexture->SetLevel(1);
    pkGLTexture->SetTextureData(nullptr, pkGLTexture->GetWidth(), pkGLTexture->GetHeight(), 0, pkGLTexture->GetTextureFormat());
#if 0   // Custom for other
    pkGLTexture->SetFilterMode(eFT_Min, GL_LINEAR);
    pkGLTexture->SetFilterMode(eFT_Mag, GL_LINEAR);
    pkGLTexture->SetClampMode(eCT_U, GL_CLAMP_TO_EDGE);
    pkGLTexture->SetClampMode(eCT_V, GL_CLAMP_TO_EDGE);
#endif
    pkGLTexture->FlushTexture(); // flush Texture

    glRenderbufferStorage(GL_RENDERBUFFER, pkGLTexture->GetGLTextureFormat(), pkGLTexture->GetWidth(), pkGLTexture->GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nDepthRenderTarget);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    int nTextureType = pkGLTexture->GetTextureType();
    switch (nTextureType) {
    case GL_TEXTURE_2D:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pkGLTexture->GetGLTexture(), 0);
        break;
    case GL_TEXTURE_3D:
        glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, pkGLTexture->GetGLTexture(), 0, 0);
        break;
    default:
        bRet = false;
        break;
    }

    GLenum eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (eStatus != GL_FRAMEBUFFER_COMPLETE)
        bRet = false;

    glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);
    m_spkDepthTexture = _rspkDepthTextue;

    return bRet;
}

ITexture* COpenGLFrameBuffer::GetTexture(int _nAttachment)
{
    if (m_kRenderTexture.size() <= _nAttachment)
        return nullptr;

    return m_kRenderTexture [_nAttachment].get();
}

ITexture* COpenGLFrameBuffer::GetDepthTexture()
{
    return m_spkDepthTexture.get();
}

bool COpenGLFrameBuffer::Active()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    //glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, m_nViewportWidth, m_nViewportHeight);

    std::vector <GLenum> kBuffers(m_kRenderTexture.size());
    for (size_t nIndex = 0; nIndex < kBuffers.size(); ++nIndex) {
        kBuffers [nIndex] = GL_COLOR_ATTACHMENT0 + nIndex;
    }

    if (kBuffers.empty())
        glDrawBuffer(GL_NONE);
    else
        glDrawBuffers(kBuffers.size(), kBuffers.data());
    return true;
}

void COpenGLFrameBuffer::Resize(int _nWidth, int _nHeight)
{
    int nAttachmnet = 0;
    for (auto spkTexture : m_kRenderTexture) {
        if (!spkTexture)
            continue;
        bool bResize = (spkTexture->GetWidth() != _nWidth);
        bResize |= (spkTexture->GetHeight() != _nHeight);
        if (!bResize)
            continue;
        spkTexture->SetTextureDimension(_nWidth, _nHeight, 1);
        SetTexture(spkTexture, nAttachmnet);
        nAttachmnet++;
    }
    if (m_spkDepthTexture) {
        bool bResize = (m_spkDepthTexture->GetWidth() != _nWidth);
        bResize |= (m_spkDepthTexture->GetHeight() != _nHeight);
        if (bResize) {
            m_spkDepthTexture->SetTextureDimension(_nWidth, _nHeight, 1);
            SetDepthTexture(m_spkDepthTexture);
        }
    }
    m_nViewportWidth = _nWidth;
    m_nViewportHeight = _nHeight;
}

bool COpenGLFrameBuffer::Deactive()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glPopAttrib();
    return true;
}

int COpenGLFrameBuffer::GetWidth(int _nAttachment)
{
    if (m_kRenderTexture.size() <= _nAttachment) {
        return 0;
    }

    std::shared_ptr <ITexture>& rspkTexture = m_kRenderTexture [_nAttachment];
    if (!rspkTexture)
        return 0;

    return rspkTexture->GetWidth();
}

int COpenGLFrameBuffer::GetHeight(int _nAttachment)
{
    if (m_kRenderTexture.size() <= _nAttachment) {
        return 0;
    }

    std::shared_ptr <ITexture>& rspkTexture = m_kRenderTexture [_nAttachment];
    if (!rspkTexture)
        return 0;

    return rspkTexture->GetHeight();
}


COpenGLDefaultFrameBuffer::COpenGLDefaultFrameBuffer(int _nWidth, int _nHeight)
    : IOpenGLFrameBufferInterface (_nWidth, _nHeight)
{

}

COpenGLDefaultFrameBuffer::~COpenGLDefaultFrameBuffer()
{

}

GLuint COpenGLDefaultFrameBuffer::GetFrameBufferObjectID()
{
    return 0;
}

void COpenGLDefaultFrameBuffer::SetRenderBufferColorAttactmentCount(int _nCount)
{
}

int COpenGLDefaultFrameBuffer::GetRenderBufferColorAttachmentCount()
{
    return 1;
}

bool COpenGLDefaultFrameBuffer::SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment)
{
    return false;
}

bool COpenGLDefaultFrameBuffer::SetDepthTexture(std::shared_ptr <ITexture>)
{
    return false;
}

ITexture* COpenGLDefaultFrameBuffer::GetTexture(int _nAttachment)
{
    return nullptr;
}

ITexture* COpenGLDefaultFrameBuffer::GetDepthTexture()
{
    return nullptr;
}

bool COpenGLDefaultFrameBuffer::Active()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_nViewportWidth, m_nViewportHeight);
    return true;
}

bool COpenGLDefaultFrameBuffer::Deactive()
{
    return true;
}

void COpenGLDefaultFrameBuffer::Resize(int _nWidth, int _nHeight)
{
    m_nViewportWidth = _nWidth;
    m_nViewportHeight = _nHeight;
}

int COpenGLDefaultFrameBuffer::GetWidth(int _nAttachment)
{
    return m_nViewportWidth;
}

int COpenGLDefaultFrameBuffer::GetHeight(int _nAttachment)
{
    return m_nViewportHeight;
}

COpenGLCubeFrameBuffer::COpenGLCubeFrameBuffer(int _nRadius)
    :COpenGLFrameBuffer(_nRadius, _nRadius)
{

}

COpenGLCubeFrameBuffer::~COpenGLCubeFrameBuffer()
{

}

void COpenGLCubeFrameBuffer::SetRenderBufferColorAttactmentCount(int _nCount)
{
    if (_nCount <= m_kRenderTexture.size()) {
        GLint nCurrentFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
        for (size_t nIndex = _nCount; nIndex < m_kRenderTexture.size(); ++nIndex) {
            std::shared_ptr <ITexture> spkTexture = m_kRenderTexture [nIndex];
            if (!spkTexture)
                continue;
            int nTextureType = spkTexture->GetTextureType();
            switch (nTextureType) {
            case GL_TEXTURE_2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nIndex, GL_TEXTURE_CUBE_MAP, -1, 0);
                break;
            case GL_TEXTURE_3D:
                glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nIndex, GL_TEXTURE_CUBE_MAP, -1, 0, 0);
                break;
            default:
                break;
            }
        }
        m_kRenderTexture.resize(_nCount);
        glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);
    }
    else {
        size_t nOldSize = m_kRenderTexture.size();
        m_kRenderTexture.resize(_nCount);
    }
}

int COpenGLCubeFrameBuffer::GetRenderBufferColorAttachmentCount()
{
    return m_kRenderTexture.size();
}

bool COpenGLCubeFrameBuffer::SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment)
{
    COpenGLTexture* pkGLTexture = dynamic_cast <COpenGLTexture*>(_rspkTexture.get());
    if (!pkGLTexture)
        return false;
    if (pkGLTexture->GetTextureType() != GL_TEXTURE_CUBE_MAP)
        return false;

    switch (pkGLTexture->GetTextureFormat()) {
    case gli::FORMAT_D16_UNORM:
    case gli::FORMAT_D24_UNORM:
    case gli::FORMAT_D24S8_UNORM:
    case gli::FORMAT_D32_UFLOAT:
    case gli::FORMAT_D32_UFLOAT_S8_UNORM:
        return false;
    default:
        break;
    }

    if (m_kRenderTexture.size() <= _nAttachment)
        return false;

    bool bRet = true;
    GLint nCurrentFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    pkGLTexture->SetLevel(6);
    for (int nFace = 0; nFace < 6; nFace++) {
        pkGLTexture->SetTextureData(nullptr, pkGLTexture->GetWidth(), pkGLTexture->GetHeight(), 0, pkGLTexture->GetTextureFormat(), nFace);
    }
#if 0   // Custom for other
    pkGLTexture->SetFilterMode(eFT_Min, GL_LINEAR);
    pkGLTexture->SetFilterMode(eFT_Mag, GL_LINEAR);
    pkGLTexture->SetClampMode(eCT_U, GL_CLAMP_TO_EDGE);
    pkGLTexture->SetClampMode(eCT_V, GL_CLAMP_TO_EDGE);
#endif
    pkGLTexture->FlushTexture(); // flush Texture


    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _nAttachment, GL_TEXTURE_2D, pkGLTexture->GetGLTexture(), 0);

    GLenum eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (eStatus != GL_FRAMEBUFFER_COMPLETE)
        bRet = false;

    glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);

    if (bRet) {
        m_kRenderTexture [_nAttachment] = _rspkTexture;
    }

    return bRet;
}

bool COpenGLCubeFrameBuffer::SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue)
{
    if (!_rspkDepthTextue) {
        if (m_nDepthRenderTarget != -1) {
            GLint nCurrentFrameBuffer = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
            std::shared_ptr <ITexture> spkTexture = m_spkDepthTexture;
            if (spkTexture) {
                glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthRenderTarget);
                int nTextureType = spkTexture->GetTextureType();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, -1, 0);
            }
            glDeleteRenderbuffers(1, &m_nDepthRenderTarget);
            glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);
        }
        return false;
    }

    COpenGLTexture* pkGLTexture = dynamic_cast <COpenGLTexture*>(_rspkDepthTextue.get());
    if (!pkGLTexture)
        return false;

    switch (_rspkDepthTextue->GetTextureFormat()) {
    case gli::FORMAT_D16_UNORM:
    case gli::FORMAT_D24_UNORM:
    case gli::FORMAT_D24S8_UNORM:
    case gli::FORMAT_D32_UFLOAT:
    case gli::FORMAT_D32_UFLOAT_S8_UNORM:
        break;
    default:
        return false;
    }

    if (m_nDepthRenderTarget == -1) {
        glGenRenderbuffers(1, &m_nDepthRenderTarget);
    }

    bool bRet = true;
    GLint nCurrentFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nCurrentFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthRenderTarget);

    pkGLTexture->SetLevel(6);
    for (int nFace = 0; nFace < 6; nFace++) {
        pkGLTexture->SetTextureData(nullptr, pkGLTexture->GetWidth(), pkGLTexture->GetHeight(), 0, pkGLTexture->GetTextureFormat(), nFace);
    }
#if 0   // Custom for other
    pkGLTexture->SetFilterMode(eFT_Min, GL_LINEAR);
    pkGLTexture->SetFilterMode(eFT_Mag, GL_LINEAR);
    pkGLTexture->SetClampMode(eCT_U, GL_CLAMP_TO_EDGE);
    pkGLTexture->SetClampMode(eCT_V, GL_CLAMP_TO_EDGE);
#endif
    pkGLTexture->FlushTexture(); // flush Texture

    glRenderbufferStorage(GL_RENDERBUFFER, pkGLTexture->GetGLTextureFormat(), pkGLTexture->GetWidth(), pkGLTexture->GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nDepthRenderTarget);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pkGLTexture->GetGLTexture(), 0);

    GLenum eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (eStatus != GL_FRAMEBUFFER_COMPLETE)
        bRet = false;

    glBindFramebuffer(GL_FRAMEBUFFER, nCurrentFrameBuffer);
    m_spkDepthTexture = _rspkDepthTextue;

    return bRet;
}

void COpenGLCubeFrameBuffer::ActiveTextureFace(char _nFace, int _nAttachment)
{
    if (_nFace < 0 || _nFace > 5)
        return;
    if (m_kRenderTexture.size() <= _nAttachment)
        return;

    // We make sure it's OpenGLTexture Object when SetTexture!
    COpenGLTexture* pkOpenGLTexture = static_cast <COpenGLTexture*>(m_kRenderTexture [_nAttachment].get());
    if (!pkOpenGLTexture)
        return;

    IRenderSystem* pkSystem = IRenderSystem::GetSystem();
    IRenderPipeline* pkPipeline = pkSystem->GetRenderPipeline();
    IFrameBuffer* pkFrameBuffer = pkPipeline->GetFrameBuffer(pkSystem->GetCurrentPass());
    IOpenGLFrameBufferInterface* pkGLFrameBuffer = dynamic_cast <IOpenGLFrameBufferInterface*>(pkFrameBuffer);
    if (!pkGLFrameBuffer)
        return;

    bool bChanged = (pkFrameBuffer != this);
    if (bChanged)
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _nAttachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + _nFace, pkOpenGLTexture->GetGLTexture(), 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    if (bChanged)
        glBindFramebuffer(GL_FRAMEBUFFER, pkGLFrameBuffer->GetFrameBufferObjectID());
}

void COpenGLCubeFrameBuffer::ActiveTextureFace(char _nFace)
{
    if (m_kRenderTexture.empty())
        return;

    const int nAttachCount = m_kRenderTexture.size();
    for (int nAttach = 0; nAttach < nAttachCount; nAttach++) {
        ActiveTextureFace(_nFace, nAttach);
    }
}

void COpenGLCubeFrameBuffer::ActiveDepthTextureFace(char _nFace)
{
    if (_nFace < 0 || _nFace > 5)
        return;
    // We make sure it's OpenGLTexture Object when SetTexture!
    COpenGLTexture* pkOpenGLTexture = static_cast <COpenGLTexture*>(m_spkDepthTexture.get());
    if (!pkOpenGLTexture)
        return;

    IRenderSystem* pkSystem = IRenderSystem::GetSystem();
    IRenderPipeline* pkPipeline = pkSystem->GetRenderPipeline();
    IFrameBuffer* pkFrameBuffer = pkPipeline->GetFrameBuffer(pkSystem->GetCurrentPass());
    IOpenGLFrameBufferInterface* pkGLFrameBuffer = dynamic_cast <IOpenGLFrameBufferInterface*>(pkFrameBuffer);
    if (!pkGLFrameBuffer)
        return;

    bool bChanged = (pkFrameBuffer != this);
    if (bChanged)
        glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBufferObject);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + _nFace, pkOpenGLTexture->GetGLTexture(), 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    if (bChanged)
        glBindFramebuffer(GL_FRAMEBUFFER, pkGLFrameBuffer->GetFrameBufferObjectID());
}

void COpenGLCubeFrameBuffer::Resize(int _nWidth, int _nHeight)
{
    // Make sure cube map is square
    if (_nWidth != _nHeight) {
        _nWidth = (_nHeight < _nWidth) ? _nHeight : _nWidth;
        _nHeight = _nWidth;
    }

    int nAttachmnet = 0;
    for (auto spkTexture : m_kRenderTexture) {
        if (!spkTexture)
            continue;
        for (int nFace = 0; nFace < 6; nFace++) {
            spkTexture->SetTextureDimension(_nWidth, _nHeight, 1, nFace);
        }
        SetTexture(spkTexture, nAttachmnet);
        nAttachmnet++;
    }
    if (m_spkDepthTexture) {
        for (int nFace = 0; nFace < 6; nFace++) {
            m_spkDepthTexture->SetTextureDimension(_nWidth, _nHeight, 1, nFace);
        }
        SetDepthTexture(m_spkDepthTexture);
    }
    m_nViewportWidth = _nWidth;
    m_nViewportHeight = _nHeight;
}