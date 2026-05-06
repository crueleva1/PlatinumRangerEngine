#pragma once

#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "Util.h"
#include "ECSOpenGLPCH.h"
#include "ImguiSystemPCH.h"
#include "entityx/entityx.h"

#ifdef ECS_MODELVIEW_BUILD_SHARED
#ifdef _WIN32
#ifdef ECS_MODELVIEW_BUILD
#define ECS_MODELVIEW_API __declspec(dllexport)
#else
#define ECS_MODELVIEW_API __declspec(dllimport)
#endif
#else
#define ECS_MODELVIEW_API __attribute__ ((visibility("default")))
#endif
#else
#define ECS_MODELVIEW_API 
#endif