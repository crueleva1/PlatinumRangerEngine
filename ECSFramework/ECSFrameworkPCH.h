#pragma once

#ifdef ECS_BUILD_SHARED
#ifdef _WIN32
#ifdef ECS_BUILD
#define ECS_API __declspec(dllexport)
#else
#define ECS_API __declspec(dllimport)
#endif
#else
#define ECS_API __attribute__ ((visibility("default")))
#endif
#else
#define ECS_API 
#endif


#include "entityx/entityx.h"
#include <memory.h>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <stdio.h>
//#include <functional>
#include "TComponentInterface.h"
#include "TSystemInterface.h"
#include "Util.h"
#include "Delegate.h"
#include "Logger.h"