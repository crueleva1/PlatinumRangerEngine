// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
#include <ECSFrameworkPCH.h>
#include <GLMMainPCH.h>
#include <GLSLSystemPCH.h>
#include <imgui.h>
#include <imconfig.h>

// This is have releation of EntityX global variable issue
#ifdef IMGUI_SYSTEM_SHARED
#ifdef _WIN32
#ifdef IMGUI_SYSTEM_BUILD
#define IMGUI_SYSTEM_API __declspec(dllexport)
#else
#define IMGUI_SYSTEM_API __declspec(dllimport)
#endif
#else
#define IMGUI_SYSTEM_API __attribute__ ((visibility("default")))
#endif
#else
#define IMGUI_SYSTEM_API 
#endif