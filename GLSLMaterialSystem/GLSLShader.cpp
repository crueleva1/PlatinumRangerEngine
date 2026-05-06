#include "GLSLSystemPCH.h"
#include "GLSLShader.h"
#include "GLSLMaterialSystem.h"

CGLSLVariableAttrDesc::CGLSLVariableAttrDesc(const char* _pcVariable, int _nVariableType, int _nLoc)
    :m_kVariableName(_pcVariable)
    ,m_nVariableType(_nVariableType)
    ,m_nLoc(_nLoc)
{

}

CGLSLVariableAttrDesc::~CGLSLVariableAttrDesc()
{
}

const char* CGLSLVariableAttrDesc::GetVariableName()
{
    return m_kVariableName.c_str();
}

int CGLSLVariableAttrDesc::GetVariableType()
{
    return m_nVariableType;
}

size_t CGLSLVariableAttrDesc::GetVariableSize()
{
    size_t nSize = 0;
    switch (m_nVariableType)
    {
    case GL_FLOAT_VEC2:
        nSize = sizeof(float) * 2;
        break;
    case GL_FLOAT_VEC3:
        nSize = sizeof(float) * 3;
        break;
    case GL_FLOAT_VEC4:
        nSize = sizeof(float) * 4;
        break;
    case GL_INT_VEC2:
        nSize = sizeof(int) * 2;
        break;
    case GL_INT_VEC3:
        nSize = sizeof(int) * 3;
        break;
    case GL_INT_VEC4:
        nSize = sizeof(int) * 4;
        break;
    case GL_FLOAT_MAT2:
        nSize = sizeof(float) * 2 * 2;
        break;
    case GL_FLOAT_MAT3:
        nSize = sizeof(float) * 3 * 3;
        break;
    case GL_FLOAT_MAT4:
        nSize = sizeof(float) * 4 * 4;
        break;
    case GL_FLOAT:
        nSize = sizeof(float);
        break;
    case GL_INT:
        nSize = sizeof(int);
        break;
    case GL_UNSIGNED_INT:
        nSize = sizeof(unsigned int);
        break;
    case GL_SHORT:
        nSize = sizeof(short);
        break;
    case GL_UNSIGNED_SHORT:
        nSize = sizeof(unsigned short);
        break;
    case GL_BOOL:
        nSize = sizeof(bool);
        break;
    default:
        assert(0);
    }
    return nSize;
}

size_t CGLSLVariableAttrDesc::GetVariableCount()
{
    size_t nCount = 0;
    switch (m_nVariableType) {
    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
        nCount = 2;
        break;
    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
        nCount = 3;
        break;
    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
        nCount = 4;
        break;
    case GL_FLOAT_MAT2:
        nCount = 2 * 2;
        break;
    case GL_FLOAT_MAT3:
        nCount = 3 * 3;
        break;
    case GL_FLOAT_MAT4:
        nCount = 4 * 4;
        break;
    case GL_FLOAT:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_BOOL:
        nCount = 1;
        break;
    default:
        assert(0);
    }
    return nCount;
}

EShLanguage g_aeShaderLangType[eShader_Max] =
{
    EShLangVertex,
    EShLangFragment,
    EShLangGeometry,
};

CGLSLShader::CGLSLShader(EShaderType _eType, unsigned int _eComplieFlag)
    : m_kShaderParser(g_aeShaderLangType[_eType])
    , m_eType(_eType)
    , m_bError(false)
    , m_bCompiled(false)
    , m_eCompilerFlag(_eComplieFlag)
{

}

CGLSLShader::~CGLSLShader()
{
}

bool CGLSLShader::SetSource(const char* _pcCode)
{
    if (!_pcCode)
        return false;
    m_kShaderCode = _pcCode;
    m_bCompiled = false;
    m_bError = false;
    return true;
}

bool CGLSLShader::LoadSource(const char* _pcSourceFileName)
{

    return true;
}

bool CGLSLShader::Compile()
{
    if (m_bCompiled)
        return !m_bError;
    if (m_kShaderCode.empty())
        return false;
    const char* apcShaderString [1] = { m_kShaderCode.c_str() };
    m_kShaderParser.setStrings(apcShaderString, 1);
    TBuiltInResource kResource;
    GetParserBuildResource(kResource);
    m_bError = m_kShaderParser.parse(&kResource, 100, false, (EShMessages)m_eCompilerFlag);
    m_kCompileLog = m_kShaderParser.getInfoLog();
    m_bCompiled = true;
    return true;
}

int CGLSLShader::GetShaderType()
{
    return m_eType;
}

bool CGLSLShader::IsError()
{
    return m_bError;
}

bool CGLSLShader::IsCompiled()
{
    return m_bCompiled;
}

const char* CGLSLShader::GetSource()
{
    return m_kShaderCode.c_str();
}

const char* CGLSLShader::GetCompileInfo()
{
    return m_kCompileLog.c_str();
}

glslang::TShader& CGLSLShader::GetShaderParser()
{
    return m_kShaderParser;
}