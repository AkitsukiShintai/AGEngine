#pragma once
#include <stdio.h>
#pragma comment (lib, "gdi32.lib") 
#define VK_USE_PLATFORM_WIN32_KHR

#ifdef GLFW
	#define GLFW_INCLUDE_VULKAN
	#include <glfw3.h>
#endif
//#include <glfw3native.h>
#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
#include <Singleton.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <regex>
#include <istream>
#include <sstream>
#include <fstream>

#include <optional>
#include <vector>
#include <array>
#include <queue>
#include <stack>
#include <set>
#include <map>

#include <memory>

#include <shaderc.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "gtx/quaternion.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <VmaUsage.h>
#include <Common.h>

#include <CompilerHelpFunction.h>
#include <Structs.h>
#include <Define.h>
#include <Enum.h>