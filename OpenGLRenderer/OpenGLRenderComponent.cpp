#include "ECSOpenGLPCH.h"
#include "OpenGLRenderComponent.h"
#include "OpenGLMaterial.h"
#include "RenderSystem.h"
#include "ITexture.h"

GLint CovToOpenGLType(EVariableSemanticElementType _eType)
{
    switch (_eType)
    {
        case eVSET_bool:
            return GL_BYTE;
        case eVSET_short:
            return GL_SHORT;
        case eVSET_ushort:
            return GL_UNSIGNED_SHORT;
        case eVSET_float:
            return GL_FLOAT;
        default:
            break;
    }
    return GL_FIXED;
}

GLenum CovToOpenGLPrivitiveType(EPrimitiveType _eType)
{
    switch (_eType) {
        case ePrimitive_None:
        case ePrimitive_Max:
        default:
            return GL_NONE;
        case ePrimitive_Points:
            return GL_POINTS;
        case ePrimitive_Lines:
            return GL_LINES;
        case ePrimitive_LineStrip:
            return GL_LINE_STRIP;
        case ePrimitive_Triangles:
            return GL_TRIANGLES;
        case ePrimitive_TriangleStrip:
            return GL_TRIANGLE_STRIP;
        case ePrimitive_TriangleFan:
            return GL_TRIANGLE_FAN;
    }
    return GL_NONE;
}

GLenum CovToOpenGLPolygonMode(EPolygonMode _eMode)
{
    switch (_eMode)
    {
        case ePM_None:
        case ePM_Max:
        default:
            return GL_NONE;
        case ePM_Point:
            return GL_POINT;
        case ePM_Line:
            return GL_LINE;
        case ePM_Fill:
            return GL_FILL;
    }
    return GL_NONE;
}

GLenum CovToOpenGLCullMode(ECullMode _eCullMode)
{
    switch (_eCullMode) {
        case eCM_None:
        case eCM_Max:
        default:
            return GL_NONE;
        case eCM_Front:
            return GL_FRONT;
        case eCM_Back:
            return GL_BACK;
        case eCM_FrontAndBack:
            return GL_FRONT_AND_BACK;
    }
    return GL_NONE;
}

GLenum CovToOpenGLFrontFace(EFrontFace _eFrontFace)
{
    switch (_eFrontFace) {
        case eFF_None:
        case eFF_Max:
        default:
            return GL_NONE;
        case eFF_ClockWise:
            return GL_CW;
        case eFF_CounterClockWise:
            return GL_CCW;
    }
    return GL_NONE;
}

GLenum CovToOpenGLDepthTestOp(EDepthTestOp _eDepthTest)
{
    switch (_eDepthTest) {
        case eDTO_None:
        case eDTO_Max:
        default:
            return GL_NONE;
        case eDTO_Never:
            return GL_NEVER;
        case eDTO_Less:
            return GL_LESS;
        case eDTO_Equal:
            return GL_EQUAL;
        case eDTO_LessEqual:
            return GL_LEQUAL;
        case eDTO_Greater:
            return GL_GREATER;
        case eDTO_NonEqual:
            return GL_NOTEQUAL;
        case eDTO_GreaterEqual:
            return GL_GEQUAL;
        case eDTO_Always:
            return GL_ALWAYS;
    }
    return GL_NONE;
}

GLenum CovToOpenGLBlendFactor(EBlendFactor _eFactor)
{
    switch (_eFactor)
    {
        case eBF_None:
        case eBF_Max:
        default:
            return GL_NONE;
        case eBF_One:
            return GL_ONE;
        case eBF_Zero:
            return GL_ZERO;
        case eBF_SrcColor:
            return GL_SRC_COLOR;
        case eBF_DstColor:
            return GL_DST_COLOR;
        case eBF_OneMinusSrcColor:
            return GL_ONE_MINUS_SRC_COLOR;
        case eBF_OneMinusDstColor:
            return GL_ONE_MINUS_DST_COLOR;
        case eBF_SrcAlpha:
            return GL_SRC_ALPHA;
        case eBF_DstAlpha:
            return GL_DST_ALPHA;
        case eBF_OneMinusSrcAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case eBF_OneMinusDstAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
        case eBF_SrcAlphaSat:
            return GL_SRC_ALPHA_SATURATE;
    }
    return GL_NONE;
}

GLenum CovToOpenGLBlendFuncOp(EBlendFuncOp _eOp)
{
    switch (_eOp)
    {
        case eBFO_None:
        case eBFO_Max:
        default:
            return GL_NONE;
        case eBFO_Add:
            return GL_FUNC_ADD;
        case eBFO_Sub:
            return GL_FUNC_SUBTRACT;
        case eBFO_ReverseSub:
            return GL_FUNC_REVERSE_SUBTRACT;
    }
    return GL_NONE;
}

GLenum CovToOpenGLAlphaTestOp(EAlphaTestOp _eOp)
{
    switch (_eOp)
    {
        case eATO_None:
        case eATO_Max:
        default:
            return GL_NONE;
        case eATO_Never:
            return GL_NEVER;
        case eATO_Less:
            return GL_LESS;
        case eATO_Equal:
            return GL_EQUAL;
        case eATO_LessEqual:
            return GL_LEQUAL;
        case eATO_Greater:
            return GL_GREATER;
        case eATO_NonEqual:
            return GL_NOTEQUAL;
        case eATO_GreaterEqual:
            return GL_GEQUAL;
        case eATO_Always:
            return GL_ALWAYS;
    }
    return GL_NONE;
}

COpenGLVertexBuffer::COpenGLVertexBuffer()
    : m_nBuffer(-1)
    , m_nVertexArray(-1)
    , m_nSize(0)
    , m_nOffset(0)
    , m_nUsage(GL_STATIC_DRAW)
{
    glGenVertexArrays(1, &m_nVertexArray);
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_nBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
}

int COpenGLVertexBuffer::GetBufferType()
{
    return eOGLT_Vertex;
}

void* COpenGLVertexBuffer::Map()
{
    if (!Bind())
        return nullptr;

    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void COpenGLVertexBuffer::UnMap()
{
    if (!Bind())
        return;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

bool COpenGLVertexBuffer::Allocate(size_t _nSize)
{
#if 0
    if (m_nBuffer != -1) {
        glDeleteBuffers(1, &m_nBuffer);
        m_nBuffer = -1;
    }
#endif
    if (m_nBuffer == -1)
        glGenBuffers(1, &m_nBuffer);

    if (!Bind())
        return false;
    std::vector <char> kTemp(_nSize);
    glBufferData(GL_ARRAY_BUFFER, _nSize, kTemp.data(), m_nUsage);
    m_nSize = _nSize;
    return true;
}

bool COpenGLVertexBuffer::Bind()
{
    if (m_nBuffer == -1)
        return false;

    glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer);
    return true;
}

void COpenGLVertexBuffer::SetAttrSemanticCount(short _nSize)
{
    m_kSematics.resize(_nSize);
}

bool COpenGLVertexBuffer::OnSetAttrSemantic(std::shared_ptr <IVariableSemantic> _spkSemantic, size_t _nIndex)
{
    if (m_kSematics.size() <= _nIndex)
        return false;

    m_kSematics [_nIndex] = _spkSemantic;
    return true;
}

size_t COpenGLVertexBuffer::GetAttrSemanticCount()
{
    return m_kSematics.size();
}

const IVariableSemantic* COpenGLVertexBuffer::GetAttrSemantic(size_t _nIndex)
{
    return m_kSematics [_nIndex].get();
}

bool COpenGLVertexBuffer::SetVertexArray()
{
    if (m_nVertexArray == -1)
        return false;

    glBindVertexArray(m_nVertexArray);
    return true;
}

bool COpenGLVertexBuffer::SetVertexBuffer(IMaterial* _pkMaterial)
{
    if (!SetVertexArray())
        return false;

    if (m_kSematics.empty())
        return false;

    const size_t nCount = m_kSematics.size();
    size_t nSize = 0;
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        const IVariableSemantic* pkSemantic = m_kSematics [nIndex].get();
        if (!pkSemantic)
            continue;
        nSize += pkSemantic->GetDescSize();
    }
    size_t nEnabledIndex = 0;
    size_t nOffset = 0;
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        const IVariableSemantic* pkSemantic = m_kSematics [nIndex].get();
        if (!pkSemantic)
            continue;
        if (!_pkMaterial->IsVariableActive(pkSemantic->GetName())) {
            glDisableVertexAttribArray (nIndex);
            nOffset += pkSemantic->GetDescSize();
            continue;
        }
        glEnableVertexAttribArray (nIndex);
        glVertexAttribPointer(nIndex, pkSemantic->GetElementCount(), CovToOpenGLType(pkSemantic->GetElementType()), false, nSize, (void*)nOffset);
        nOffset += pkSemantic->GetDescSize ();
    }
    return true;
}

size_t COpenGLVertexBuffer::GetTirangleCount()
{
    size_t nCount = GetVertexCount();
    return nCount / 3;
}

size_t COpenGLVertexBuffer::GetVertexCount()
{
    if (m_kSematics.empty())
        return 0;

    size_t nVertexStripSize = GetAttrSemanticSize();
    return (m_nSize / nVertexStripSize);
}

size_t COpenGLVertexBuffer::GetAttrSemanticSize()
{
    size_t nVertexStripSize = 0;
    const size_t nCount = m_kSematics.size();
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        nVertexStripSize += m_kSematics [nIndex]->GetDescSize();
    }
    return nVertexStripSize;
}

bool COpenGLVertexBuffer::SetUsage(int _nUsage)
{
    m_nUsage = _nUsage;
    if (!m_nSize)
        return true;
    // Change Usage
    return Allocate(m_nSize);
}

COpenGLBufferDataVertexBuffer::COpenGLBufferDataVertexBuffer()
    : COpenGLVertexBuffer()
    , m_pcCachedBuffer(nullptr)
{
}

COpenGLBufferDataVertexBuffer::~COpenGLBufferDataVertexBuffer()
{
    if (m_pcCachedBuffer)
        delete m_pcCachedBuffer;
}

bool COpenGLBufferDataVertexBuffer::Allocate(size_t _nSize)
{
    if (!COpenGLVertexBuffer::Allocate(_nSize))
        return false;

    if (m_pcCachedBuffer) {
        delete m_pcCachedBuffer;
    }
    m_pcCachedBuffer = new char [m_nSize + 1]();
    return true;
}

void* COpenGLBufferDataVertexBuffer::Map()
{
    return m_pcCachedBuffer;
}

void COpenGLBufferDataVertexBuffer::UnMap()
{
    if (!m_pcCachedBuffer)
        return;

    if (!Bind())
        return;

    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nSize, m_pcCachedBuffer);
}

COpenGLIndiceBuffer::COpenGLIndiceBuffer(int _nIndiceType)
    :m_nBuffer(-1)
    ,m_nSize(0)
    ,m_nUsage(GL_STATIC_DRAW)
    ,m_nIndicesType(_nIndiceType)
{

}

COpenGLIndiceBuffer::~COpenGLIndiceBuffer()
{
    glDeleteBuffers(1, &m_nBuffer);
}

int COpenGLIndiceBuffer::GetBufferType()
{
    return eOGLT_Index;
}

void* COpenGLIndiceBuffer::Map()
{
    if (!Bind())
        return nullptr;

    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void COpenGLIndiceBuffer::UnMap()
{
    if (!Bind())
        return;

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

bool COpenGLIndiceBuffer::Allocate(size_t _nSize)
{
#if 0
    if (m_nBuffer != -1) {
        glDeleteBuffers(1, &m_nBuffer);
        m_nBuffer = 0;
    }
#endif
    if (m_nBuffer == -1)
        glGenBuffers(1, &m_nBuffer);

    if (!Bind())
        return false;

    std::vector <char> kTemp(_nSize);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _nSize, kTemp.data(), m_nUsage);
    m_nSize = _nSize;
    return true;
}

bool COpenGLIndiceBuffer::Bind()
{
    if (m_nBuffer == -1)
        return false;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nBuffer);
    return true;
}

size_t COpenGLIndiceBuffer::GetElementSize()
{    
    switch (m_nIndicesType)
    {
        case GL_INT:
            return sizeof(int);
        case GL_UNSIGNED_INT:
            return sizeof(unsigned int);
        case GL_SHORT:
            return sizeof(short);
        case GL_UNSIGNED_SHORT:
            return sizeof(unsigned short);
        default:
            break;
    }
    return sizeof(unsigned short);
}

int COpenGLIndiceBuffer::GetIndicesType()
{
    return m_nIndicesType;
}

bool COpenGLIndiceBuffer::SetUsage(int _nUsage)
{
    m_nUsage = _nUsage;
    if (!m_nSize)
        return true;

    return Allocate(m_nSize);
}

COpenGLBufferDataIndiceBuffer::COpenGLBufferDataIndiceBuffer(int _nIndicesType)
    :COpenGLIndiceBuffer(_nIndicesType)
    ,m_pcCachedBuffer(nullptr)
{
}

COpenGLBufferDataIndiceBuffer::~COpenGLBufferDataIndiceBuffer()
{
    if (m_pcCachedBuffer)
        delete m_pcCachedBuffer;
}

void* COpenGLBufferDataIndiceBuffer::Map()
{
    if (!m_pcCachedBuffer)
        m_pcCachedBuffer = new char[m_nSize + 1]();
    return m_pcCachedBuffer;
}

void COpenGLBufferDataIndiceBuffer::UnMap()
{
    if (!m_pcCachedBuffer)
        return;

    if (!Bind())
        return;

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_nSize, m_pcCachedBuffer);

    delete m_pcCachedBuffer;
    m_pcCachedBuffer = nullptr;
}

CRenderState COpenGLRenderComponent::m_kCoreState;

COpenGLRenderComponent::COpenGLRenderComponent()
    : CRenderComponent()
{

}

COpenGLRenderComponent::~COpenGLRenderComponent()
{

}

bool COpenGLRenderComponent::SetRenderState(IRenderState* _pkRenderState)
{
    if (!_pkRenderState)
        return false;
    m_kCoreState.SetBlendEnable(_pkRenderState->IsBlendEnable());
    m_kCoreState.SetBlendFactor(_pkRenderState->GetBlendFactor(false, false), false, false);
    m_kCoreState.SetBlendFactor(_pkRenderState->GetBlendFactor(true, false), true, false);
    m_kCoreState.SetBlendFactor(_pkRenderState->GetBlendFactor(false, true), false, true);
    m_kCoreState.SetBlendFactor(_pkRenderState->GetBlendFactor(true, true), true, true);
    m_kCoreState.SetBlendOp(_pkRenderState->GetBlendOp(false), false);
    m_kCoreState.SetBlendOp(_pkRenderState->GetBlendOp(true), true);
    m_kCoreState.SetCullFaceEnable(_pkRenderState->IsCullFaceEnable());
    m_kCoreState.SetCullMode(_pkRenderState->GetCullMode());
    m_kCoreState.SetDepthTestEnable(_pkRenderState->IsDepthTestEnable());
    m_kCoreState.SetDepthCompareType(_pkRenderState->GetDepthCompareType());
    m_kCoreState.SetDepthOpMask(_pkRenderState->GetDepthOpMask());
    m_kCoreState.SetFrontFace(_pkRenderState->GetFrontFace());
    m_kCoreState.SetPolygonMode(_pkRenderState->GetPolygonMode());
    m_kCoreState.SetPrimitiveTopology(_pkRenderState->GetPrimitiveTopology());
    m_kCoreState.SetAlphaTest(_pkRenderState->IsAlphaTestEnable());
    m_kCoreState.SetAlphaTestRef(_pkRenderState->GetAlphaTestRef());

    return true;
}

const IRenderState* COpenGLRenderComponent::GetRenderState() const
{
    return &m_kCoreState;
}

bool COpenGLRenderComponent::ValidatePipeline(IRenderer& _rkRenderer)
{
    // AlphaFunc and ALPHA_TEST are deprecated since 3.0 (and removed in 4) 
    bool bFixedAlphaTest = (_rkRenderer.GetMajorVersion() < 3);
    if (!m_pkMaterial)
        return false;

    COpenGLMaterial* pkOpenGLMat = dynamic_cast <COpenGLMaterial*>(m_pkMaterial);
    if (!pkOpenGLMat)
        return false;

    if (!m_pkMaterial->LinkShader(m_pkVertexBuffer))
        return false;

    if (!m_pkVertexBuffer) {
        return false;
    }
    // SetBuffer
    if (!m_pkVertexBuffer->Bind()) {
        return false;
    }

    IVertexBuffer* pkVertexBuffer = static_cast<IVertexBuffer*>(m_pkVertexBuffer);
    if (!pkVertexBuffer->SetVertexBuffer(pkOpenGLMat)) {
        return false;
    }
    if (m_pkIndexBuffer) {
        if (!m_pkIndexBuffer->Bind())
            return false;
    }
    // Update Constant
    int nTextureStage = 0;
    const size_t nConstantCount = m_pkMaterial->GetConstantCount();
    for (size_t nIndex = 0; nIndex < nConstantCount; nIndex++) {
        IConstantSemantic* pkSemantic = m_pkMaterial->GetConstant(nIndex);
        if (!pkSemantic)
            continue;
#if USE_UBO
        if (!pkSemantic->isDirty()) {
            continue;
        }
#endif
        //GLint nLoc = glGetUniformLocation(pkOpenGLMat->GetProgram(), pkSemantic->GetVaribleName());
        GLint nLoc = -1;
        IOpenGLUniformLocInterface* pkILoc = dynamic_cast <IOpenGLUniformLocInterface*>(pkSemantic);
        if (pkILoc)
            nLoc = pkILoc->GetLocation();
        else
            nLoc = pkOpenGLMat->GetUniformLocation(pkSemantic->GetVaribleName());
        EConstantType eType = pkSemantic->GetType();
        switch (eType)
        {
            case eConstant_bool:
            {
                GLint nBool = (*(bool*)pkSemantic->GetRawData()) ? GL_TRUE : GL_FALSE;
                glUniform1i(nLoc, nBool);
                break;
            }
            case eConstant_short:
            case eConstant_int:
                glUniform1i(nLoc, *(GLint*)pkSemantic->GetRawData());
                break;
            case eConstant_ushort:
            case eConstant_uint:
                glUniform1ui(nLoc, *(GLuint*)pkSemantic->GetRawData());
                break;
            case eConstant_float:
                glUniform1f(nLoc, *(GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_floatVec2:
                glUniform2fv(nLoc, 1, (const GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_floatVec3:
                glUniform3fv(nLoc, 1, (const GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_floatVec4:
                glUniform4fv(nLoc, 1, (const GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_intVec2:
                glUniform2iv(nLoc, 1, (const GLint*)pkSemantic->GetRawData());
                break;
            case eConstant_intVec3:
                glUniform3iv(nLoc, 1, (const GLint*)pkSemantic->GetRawData());
                break;
            case eConstant_intVec4:
                glUniform4iv(nLoc, 1, (const GLint*)pkSemantic->GetRawData());
                break;
            case eConstant_Matrix2x2:
                glUniformMatrix2fv(nLoc, 1, false, (GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_Matrix3x3:
                glUniformMatrix3fv(nLoc, 1, false, (GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_Matrix4x4:
                glUniformMatrix4fv(nLoc, 1, false, (GLfloat*)pkSemantic->GetRawData());
                break;
            case eConstant_Texture:
            {
                TConstantSemantic <ITexture*>* pkTextureSemantic = dynamic_cast <TConstantSemantic <ITexture*>*>(pkSemantic);
                if (!pkTextureSemantic)
                    continue;
                ITexture* pkTexture = pkTextureSemantic->GetValue();
                if (!pkTexture) {
                    glActiveTexture(GL_TEXTURE0 + nTextureStage);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glUniform1i(nLoc, nTextureStage);
                    nTextureStage++;
                    continue;
                }
                if (!pkTexture->Active(nTextureStage))
                    continue;
                if (!pkTexture->BindTexture())
                    continue;
                glUniform1i(nLoc, nTextureStage);
                nTextureStage++;
                break;
            }
            default:
            {
                continue;
            }
        }
    }

    if (m_kCoreState.IsDataModified()) {
        // Depth Test
        if (m_kCoreState.IsDepthTestEnable()) {
            glEnable(GL_DEPTH_TEST);
            GLenum eCompareType = CovToOpenGLDepthTestOp(m_kCoreState.GetDepthCompareType());
            glDepthFunc(eCompareType);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
        // Depth Write
        glDepthMask(m_kCoreState.GetDepthOpMask());
        // Cull Mode
        if (m_kCoreState.IsCullFaceEnable()) {
            glEnable(GL_CULL_FACE);
            GLenum eCullFace = CovToOpenGLCullMode(m_kCoreState.GetCullMode());
            glCullFace(eCullFace);
            GLenum eFrontFace = CovToOpenGLFrontFace(m_kCoreState.GetFrontFace());
#if 1
            glFrontFace(eFrontFace);
#else
            GLenum ePolygonMode = CovToOpenGLPolygonMode(m_kCoreState.GetPolygonMode());
            glPolygonMode(eFrontFace, ePolygonMode);
#endif
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        // Blend
        if (m_kCoreState.IsBlendEnable()) {
            glEnable(GL_BLEND);
            GLenum eSrcColor = CovToOpenGLBlendFactor(m_kCoreState.GetBlendFactor(true, false));
            GLenum eDstColor = CovToOpenGLBlendFactor(m_kCoreState.GetBlendFactor(false, false));
            GLenum eSrcAlpha = CovToOpenGLBlendFactor(m_kCoreState.GetBlendFactor(true, true));
            GLenum eDstAlpha = CovToOpenGLBlendFactor(m_kCoreState.GetBlendFactor(false, true));
            glBlendFuncSeparate(eSrcColor, eDstColor, eSrcAlpha, eDstAlpha);
            GLenum eColorOp = CovToOpenGLBlendFuncOp(m_kCoreState.GetBlendOp(false));
            GLenum eAlphaOp = CovToOpenGLBlendFuncOp(m_kCoreState.GetBlendOp(true));
            glBlendEquationSeparate(eColorOp, eAlphaOp);
        }
        else {
            glDisable(GL_BLEND);
        }
        // Alpha
        if (bFixedAlphaTest) {
            if (m_kCoreState.IsAlphaTestEnable()) {
                glEnable(GL_ALPHA_TEST);
                GLenum eAlphaTestOp = CovToOpenGLAlphaTestOp(m_kCoreState.GetAlphaTestMode());
                glAlphaFunc(eAlphaTestOp, m_kCoreState.GetAlphaTestRef());
            }
            else {
                glDisable(GL_ALPHA_TEST);
            }
        }
        m_kCoreState.ClearDirty();
    }

    return true;
}