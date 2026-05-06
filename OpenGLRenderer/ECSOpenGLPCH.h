// ECSOpenGLPCH.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "ECSFrameworkPCH.h"
#include "GL/glew.h"
#include "GL/wglext.h"
#if 0
#define GLM_FORCE_SWIZZLE 1
#include "entityx/entityx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gli/gli.hpp"
#include "glslang/Public/ShaderLang.h"
#else
#include "GLMMainPCH.h"
#include "GLSLSystemPCH.h"
#endif

#ifdef ECS_OGL_BUILD_SHARED
#ifdef _WIN32
#ifdef ECS_OGL_BUILD
#define ECS_OGL_API __declspec(dllexport)
#else
#define ECS_OGL_API __declspec(dllimport)
#endif
#else
#define ECS_OGL_API __attribute__ ((visibility("default")))
#endif
#else
#define ECS_OGL_API 
#endif

// TODO: reference additional headers your program requires here
