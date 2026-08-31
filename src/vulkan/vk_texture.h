#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include <string>
#include <vulkan/vulkan.h>

#include "vk_render_data.h"

class VkRendererTexture
{
public:
	static bool loadTexture(VkRenderData& renderData, VkTextureData& textureData, std::string textureFilename);
	static void cleanup(VkRenderData& renderData, VkTextureData& textureData);
};

#endif