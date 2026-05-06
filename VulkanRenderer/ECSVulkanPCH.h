#pragma once


#include "ECSFrameworkPCH.h"
#include "GLMMainPCH.h"
#include "GLSLSystemPCH.h"

#include "VulkanImporter.h"
#include "VulkanTools.h"
#include "SPIRV/GlslangToSpv.h"

#define vkAlgin(size, alignsize) (size + alignsize - 1) & ~(alignsize - 1)