#pragma once

#ifdef ENTITYX_BUILD_SHARED
#ifdef _WIN32
#ifdef ENTITYX_BUILD
#define ENTITYX_API __declspec(dllexport)
#else
#define ENTITYX_API __declspec(dllimport)
#endif
#else
#define ENTITYX_API __attribute__ ((visibility("default")))
#endif
#else
#define ENTITYX_API 
#endif
