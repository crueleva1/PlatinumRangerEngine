#pragma once

#include "RenderSystem.h"

class ECS_OGL_API IOpenGLFrameBufferInterface : public IFrameBuffer
{
public:
    IOpenGLFrameBufferInterface(int _nWidth, int _nHeight)
        :IFrameBuffer(_nWidth, _nHeight)
    {}

    virtual ~IOpenGLFrameBufferInterface()
    {}

    virtual GLuint GetFrameBufferObjectID() = 0;
};

class ECS_OGL_API COpenGLFrameBuffer : public IOpenGLFrameBufferInterface
{
protected:
    std::vector <std::shared_ptr <ITexture> > m_kRenderTexture;
    std::shared_ptr <ITexture> m_spkDepthTexture;
    GLuint m_nDepthRenderTarget;
    GLuint m_nFrameBufferObject;
public:
    COpenGLFrameBuffer(int _nWidth, int _nHeight);

    virtual ~COpenGLFrameBuffer();

    virtual GLuint GetFrameBufferObjectID();

    virtual void SetRenderBufferColorAttactmentCount(int _nCount);

    virtual int GetRenderBufferColorAttachmentCount();

    virtual bool SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment);

    virtual bool SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue);

    virtual ITexture* GetTexture(int _nAttachment);

    virtual ITexture* GetDepthTexture();

    virtual bool Active();

    virtual bool Deactive();

    virtual void Resize(int _nWidth, int _nHeight);

    virtual int GetHeight(int _nAttachment);

    virtual int GetWidth(int _nAttachment);
};

class ECS_OGL_API COpenGLDefaultFrameBuffer : public IOpenGLFrameBufferInterface
{
public:
    COpenGLDefaultFrameBuffer(int _nWidth, int _nHeight);

    virtual ~COpenGLDefaultFrameBuffer();

    virtual GLuint GetFrameBufferObjectID();

    virtual void SetRenderBufferColorAttactmentCount(int _nCount);

    virtual int GetRenderBufferColorAttachmentCount();

    virtual bool SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment);

    virtual bool SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue);

    virtual ITexture* GetTexture(int _nAttachment);

    virtual ITexture* GetDepthTexture();

    virtual bool Active();

    virtual bool Deactive();

    virtual void Resize(int _nWidth, int _nHeight);

    virtual int GetHeight(int _nAttachment);

    virtual int GetWidth(int _nAttachment);
};

class ECS_OGL_API COpenGLCubeFrameBuffer : public COpenGLFrameBuffer
{
public:
    COpenGLCubeFrameBuffer(int _nRadius);

    virtual ~COpenGLCubeFrameBuffer();

    virtual void SetRenderBufferColorAttactmentCount(int _nCount);

    virtual int GetRenderBufferColorAttachmentCount();

    virtual bool SetTexture(std::shared_ptr<ITexture> _rspkTexture, int _nAttachment);

    virtual bool SetDepthTexture(std::shared_ptr<ITexture> _rspkDepthTextue);

    void ActiveTextureFace(char _nFace, int _nAttachment);

    void ActiveTextureFace(char _nFace);

    void ActiveDepthTextureFace(char _nFace);

    void Resize(int _nWidth, int _nHeight);
};