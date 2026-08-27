#ifndef VK_COMMAND_POOL_H
#define VK_COMMAND_POOL_H

#include <vulkan/vulkan.h>

#include "vk_render_data.h"

class CommandPool
{
public:
	static bool init(VkRenderData& renderData);
	static void cleanup(VkRenderData& renderData);
};

#endif