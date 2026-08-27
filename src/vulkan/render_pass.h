#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <vulkan/vulkan.h>

#include "vk_render_data.h"

class VkRendererRenderPass 
{
public:
	static bool init(VkRenderData& renderData);
	static void cleanup(VkRenderData& renderData);
};


#endif