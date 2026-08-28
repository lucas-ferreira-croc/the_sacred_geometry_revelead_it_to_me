#ifndef VK_VERTEX_BUFFER_H
#define VK_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>
#include "vk_render_data.h"

class VkVertexBuffer
{
public:
	static bool init(VkRenderData& renderData);
	static bool uploadData(VkRenderData& renderData, VkMesh vertexData);
	static void cleanup(VkRenderData& renderData);
};

#endif