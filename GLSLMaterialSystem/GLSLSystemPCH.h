#pragma once

#include "ECSFrameworkPCH.h"
#include "GLMMainPCH.h"

#ifdef ECS_GLSL_BUILD_SHARED
#ifdef _WIN32
#ifdef ECS_GLSL_BUILD
#define ECS_GLSL_API __declspec(dllexport)
#else
#define ECS_GLSL_API __declspec(dllimport)
#endif
#else
#define ECS_GLSL_API __attribute__ ((visibility("default")))
#endif
#else
#define ECS_GLSL_API 
#endif

#define GLM_FORCE_SWIZZLE 1

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gli/gli.hpp"
#include "glslang/Public/ShaderLang.h"
#include "glslang/Include/Types.h"
#include "Factory.h"
#include <string>
#include <map>

#define GL_SHORT                          0x1402
#define GL_FLOAT                          0x1406
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT                            0x1404
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_UNSIGNED_INT                   0x1405
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_BOOL                           0x8B56
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60