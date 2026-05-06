#include "ECSOpenGLPCH.h"
#include "OpenGLMaterial.h"
#include "OpenGLRenderComponent.h"
#include "OpenGLMaterialSystem.h"
#include "GLMBaseCameraComponent.h"
#include "ITexture.h"

COpenGLShader::COpenGLShader(EShaderType _eType)
    :CGLSLShader(_eType, EShMsgDefault)
    ,m_nShader(0)
{
    
}

COpenGLShader::~COpenGLShader()
{
    if (m_nShader)
        glDeleteShader(m_nShader);
}

bool COpenGLShader::SetSource(const char* _pcCode)
{
    if (!CGLSLShader::SetSource(_pcCode))
        return false;

    if (m_nShader) {
        glDeleteShader(m_nShader);
    }

    GLenum eShaderType = 0;
    switch (m_eType) {
    case eShader_Vertex:
        eShaderType = GL_VERTEX_SHADER;
        break;
    case eShader_Pixel:
        eShaderType = GL_FRAGMENT_SHADER;
        break;
    case eShader_Geomtery:
        eShaderType = GL_GEOMETRY_SHADER;
        break;
    default:
        return false;
    }

    m_nShader = glCreateShader(eShaderType);

    return true;
}

bool COpenGLShader::Compile()
{
    if (!m_nShader)
        return false;

    if (!CGLSLShader::Compile()) {
        return false;
    }

    GLint nLen = m_kShaderCode.size();
    const GLint* pnLen = &nLen;
    const char* pcChar = m_kShaderCode.c_str();
    glShaderSource(m_nShader, 1, &pcChar, pnLen);
    glCompileShader(m_nShader);

    int nState = GL_TRUE;
    glGetShaderiv(m_nShader, GL_COMPILE_STATUS, &nState);
    if (nState == GL_FALSE) {
        int nLen = 0;
        glGetShaderiv(m_nShader, GL_INFO_LOG_LENGTH, &nLen);
        if (nLen > 0) {
            m_kCompileLog.resize(nLen);
            char* pcChar = (char*)m_kCompileLog.data();
            glGetShaderInfoLog(m_nShader, nLen, &nLen, pcChar);
        }
        m_bError = true;
        return false;
    }

    m_bError = false;
    return true;
}

COpenGLMaterial::COpenGLMaterial(CGLSLMaterialSystem& _rkSystem)
    : CGLSLMaterial(_rkSystem)
{
}

COpenGLMaterial::~COpenGLMaterial()
{
    if (m_nProgram) {
        glDeleteProgram(m_nProgram);
    }
}

bool COpenGLMaterial::LinkShader(IBuffer* _pkAttr)
{
    if (!m_bRelink) {
        if (m_nProgram != -1) {
            glUseProgram(m_nProgram);
        }
        return true;
    }

    if (!_pkAttr)
        return false;

    if (m_nProgram != -1) {
        glDeleteProgram(m_nProgram);
    }
    m_nProgram = glCreateProgram();
    // Check
    for (int nIndex = eShader_Vertex; nIndex < eShader_Max; nIndex++) {
        std::shared_ptr <IShader>& spkShader = m_apkShaders [nIndex];
        if (!spkShader)
            continue;
        if (!spkShader->IsCompiled()) {
            if (!spkShader->Compile())
                return false;
        }
        if (spkShader->IsError())
            return false;
        COpenGLShader* pkGLShader = dynamic_cast <COpenGLShader*>(spkShader.get());
        if (!pkGLShader)
            return false;
    }

    for (int nIndex = eShader_Vertex; nIndex < eShader_Max; nIndex++) {
        std::shared_ptr <IShader>& spkShader = m_apkShaders [nIndex];
        COpenGLShader* pkGLShader = dynamic_cast <COpenGLShader*>(spkShader.get());
        if (!pkGLShader)
            continue;

        glAttachShader(m_nProgram, pkGLShader->GetShader());
    }
    IVertexBuffer* pkVertexBuffer = dynamic_cast <IVertexBuffer*>(_pkAttr);
    if (pkVertexBuffer == nullptr)
        return false;
    const size_t nAttrCount = pkVertexBuffer->GetAttrSemanticCount();
    for (size_t nIndex = 0; nIndex < nAttrCount; nIndex++) {
        const IVariableSemantic* pkSemantic = pkVertexBuffer->GetAttrSemantic(nIndex);
        if (!pkSemantic)
            continue;
        const char* pcVarName = GetActiveVariableName(pkSemantic->GetName());
        if (!pcVarName)
            continue;
        glBindAttribLocation(m_nProgram, nIndex, pcVarName);
    }

    glLinkProgram(m_nProgram);
    int nState = GL_TRUE;
    glGetProgramiv(m_nProgram, GL_LINK_STATUS, &nState);
    if (nState == GL_FALSE) {
        int nLen = 0;
        glGetProgramiv(m_nProgram, GL_INFO_LOG_LENGTH, &nLen);
        if (nLen > 0) {
            m_kProgramLog.resize(nLen);
            char* pcLog = (char*)m_kProgramLog.data();
            glGetProgramInfoLog(m_nProgram, nLen, &nLen, pcLog);
        }
        return false;
    }

    m_bRelink = false;
    glUseProgram(m_nProgram);

    m_kUniformLocation.clear();
    for (auto spkConstantSemantic : m_kConstants) {
        GLint nLoc = glGetUniformLocation(m_nProgram, spkConstantSemantic->GetVaribleName());
        if (nLoc != -1) {
            IOpenGLUniformLocInterface* pkILoc = dynamic_cast <IOpenGLUniformLocInterface*>(spkConstantSemantic.get());
            if (pkILoc) {
                pkILoc->SetLocation(nLoc);
            }
            m_kUniformLocation.insert(std::make_pair(spkConstantSemantic->GetVaribleName(), nLoc));
        }
    }
    
    return true;
}