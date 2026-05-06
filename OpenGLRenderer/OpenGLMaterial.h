#pragma once

#include "TComponentInterface.h"
#include "IMaterialComponent.h"
#include "DataModifier.h"
#include "GLSLShader.h"
#include "GLSLMaterial.h"
#include <vector>
#include <string>

class ECS_OGL_API COpenGLShader : public CGLSLShader
{
    GLint m_nShader;
public:
    COpenGLShader(EShaderType);

    virtual ~COpenGLShader();

    virtual bool SetSource(const char* _pcCode);

    virtual bool Compile();

    inline GLint GetShader()
    {
        if (m_bError)
            return 0;
        return m_nShader;
    }
};

class ECS_OGL_API COpenGLMaterial : public CGLSLMaterial
{
    GLuint m_nProgram;
public:
    COpenGLMaterial(class ECS_OGL_API CGLSLMaterialSystem& _rkSystem);

    virtual ~COpenGLMaterial();

    virtual bool LinkShader(IBuffer* _pkAttr);

    GLuint GetProgram()
    {
        return m_nProgram;
    }
};