#pragma once

#include "RenderComponent.h"
#include "IMaterialComponent.h"
#include "RenderState.h"
#include <vector>

#if 1
#define OpenGLCheckError()          \
{                                   \
    GLint nError = glGetError();    \
    assert(nError == GL_NO_ERROR);  \
}
#else
#define OpenGLCheckError()
#endif

GLint CovToOpenGLType(EVariableSemanticElementType _eType);
GLenum CovToOpenGLPrivitiveType(EPrimitiveType _eType);
GLenum CovToOpenGLPolygonMode(EPolygonMode _eMode);
GLenum CovToOpenGLCullMode(ECullMode _eCullMode);
GLenum CovToOpenGLFrontFace(EFrontFace _eFrontFace);
GLenum CovToOpenGLDepthTestOp(EDepthTestOp _eDepthTest);
GLenum CovToOpenGLBlendFactor(EBlendFactor _eFactor);
GLenum CovToOpenGLBlendFuncOp(EBlendFuncOp _eOp);
GLenum CovToOpenGLAlphaTestOp(EAlphaTestOp _eOp);

enum EOpenGLBufferType
{
    eOGLT_None,
    eOGLT_Vertex,
    eOGLT_Index,
    eOGLT_Uniform,
    eOGLT_Max,
};

class ECS_OGL_API COpenGLVertexBuffer : public IVertexBuffer
{
protected:
    size_t m_nSize;
    size_t m_nOffset;
    GLuint m_nVertexArray;
    GLuint m_nBuffer;
    int m_nUsage;
    std::vector <std::shared_ptr <IVariableSemantic> > m_kSematics;
public:

    COpenGLVertexBuffer();

    virtual ~COpenGLVertexBuffer();

    virtual int GetBufferType();

    virtual void* Map();

    virtual void UnMap();

    virtual bool Allocate(size_t _nSize);

    virtual bool Bind();

    virtual size_t GetSize()
    {
        return m_nSize;
    }

    virtual void SetAttrSemanticCount(short _nSize);

    virtual bool OnSetAttrSemantic(std::shared_ptr <IVariableSemantic> _spkSemantic, size_t _nIndex);

    virtual size_t GetAttrSemanticCount();

    virtual const IVariableSemantic* GetAttrSemantic(size_t _nIndex);

    virtual bool SetVertexArray();

    virtual bool SetVertexBuffer(struct IMaterial*);

    virtual size_t GetOffset()
    {
        return m_nOffset;
    }

    virtual void SetOffset(size_t _nOffset)
    {
        m_nOffset = _nOffset;
    }

    size_t GetTirangleCount();

    size_t GetVertexCount();

    virtual size_t GetAttrSemanticSize();

    inline int GetUseage()
    {
        return m_nUsage;
    }

    bool SetUsage(int _nUsage);
};

class ECS_OGL_API COpenGLBufferDataVertexBuffer : public COpenGLVertexBuffer
{
    char* m_pcCachedBuffer;
public:
    COpenGLBufferDataVertexBuffer();

    virtual ~COpenGLBufferDataVertexBuffer();

    virtual bool Allocate(size_t _nSize);

    virtual void* Map();

    virtual void UnMap();
};

class ECS_OGL_API COpenGLIndiceBuffer : public IIndiceBuffer
{
protected:
    GLuint m_nBuffer;
    int m_nUsage;
    int m_nIndicesType;
    size_t m_nSize;
public:
    COpenGLIndiceBuffer(int _nIndicesType);

    virtual ~COpenGLIndiceBuffer();

    virtual int GetBufferType();

    virtual void* Map();

    virtual void UnMap();

    virtual bool Allocate(size_t _nSize);

    virtual bool Bind();

    virtual size_t GetSize()
    {
        return m_nSize;
    }

    virtual size_t GetElementSize();

    virtual int GetIndicesType();

    inline int GetUsage()
    {
        return m_nUsage;
    }

    bool SetUsage(int _nUsage);
};

class ECS_OGL_API COpenGLBufferDataIndiceBuffer : public COpenGLIndiceBuffer
{
    char* m_pcCachedBuffer;
public:
    COpenGLBufferDataIndiceBuffer(int _nIndicesType);

    virtual ~COpenGLBufferDataIndiceBuffer();

    virtual void* Map();

    virtual void UnMap();
};

class ECS_OGL_API COpenGLRenderComponent : public CRenderComponent
{
    friend class  COpenGLRenderer;
    static CRenderState m_kCoreState;
public:
    COpenGLRenderComponent();

    virtual ~COpenGLRenderComponent();

    virtual bool SetRenderState(IRenderState* _pkRenderState);

    virtual const IRenderState* GetRenderState() const;

    virtual bool ValidatePipeline(class IRenderer&);
};
