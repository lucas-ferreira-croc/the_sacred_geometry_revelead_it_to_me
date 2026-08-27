#ifndef VK_COMMAND_BUFFER_H
#define VK_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include "vk_render_data.h"

class CommandBuffer
{
public:
	static bool init(VkRenderData& renderData, VkCommandBuffer& commandBuffer);
	static void cleanup(VkRenderData& renderData, VkCommandBuffer& commandBuffer);
};

#endif