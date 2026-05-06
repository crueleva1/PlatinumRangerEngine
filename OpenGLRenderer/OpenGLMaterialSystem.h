#pragma once

#include "GLSLMaterialSystem.h"


class ECS_OGL_API COpenGLMaterialSystem : public CGLSLMaterialSystem
{
public:
    COpenGLMaterialSystem();

    virtual ~COpenGLMaterialSystem();

    virtual void configure(entityx::EntityManager& EntityMgr, entityx::EventManager& _rkEvents);
};