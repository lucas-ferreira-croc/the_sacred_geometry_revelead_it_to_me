#ifndef VK_UNIFORM_BUFFER_H
#define VK_UNIFORM_BUFFER_H

#include <glm/glm.hpp>

#include "vk_render_data.h"

class VkRendererUniformBuffer 
{
public:
	static bool init(VkRenderData& renderData);
	static void uploadData(VkRenderData& renderData, VkUploadMatrices matrices);
	static void cleanup(VkRenderData& renderData);
};

#endif