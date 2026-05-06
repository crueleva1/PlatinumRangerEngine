#pragma once

#include "ECSFrameworkPCH.h"

#ifdef ECSGLM_BUILD_SHARED
#ifdef _WIN32
#ifdef ECSGLM_BUILD
#define ECSGLM_API __declspec(dllexport)
#else
#define ECSGLM_API __declspec(dllimport)
#endif
#else
#define ECSGLM_API __attribute__ ((visibility("default")))
#endif
#else
#define ECSGLM_API 
#endif

#define GLM_FORCE_SWIZZLE 1

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gli/gli.hpp"
#include <string>
#include <map>