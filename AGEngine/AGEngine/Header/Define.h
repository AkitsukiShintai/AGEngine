#pragma once
#define  COLOR_ATTACHMENT_FORMAT VK_FORMAT_R8G8B8A8_UNORM
#define  DEPTH_ATTACHMENT_FORMAT VK_FORMAT_D32_SFLOAT_S8_UINT
#define DATA_ATTACHMENT_FORMAT VK_FORMAT_R32G32B32A32_SFLOAT

using BufferUpdateCallback = void (*)();
using MouseUpdateCallback = void (*)(int x, int y);