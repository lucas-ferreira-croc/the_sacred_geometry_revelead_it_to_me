#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H

#include <vector>
#include <vulkan/vulkan.h>

#include <vk_render_data.h>

class VkRendererFramebuffer 
{
public:
	static bool init(VkRenderData& renderData);
	static void cleanup(VkRenderData& renderData);
};

#endif