#ifndef VK_INDEX_BUFFER
#define VK_INDEX_BUFFER

#include <vulkan/vulkan.h>
#include <tiny_gltf.h>

#include "vk_render_data.h"

class VkIndexBuffer
{
public:
	static bool init(VkRenderData& renderData, VkIndexBufferData& indexBufferData, unsigned int bufferSize);
	static bool uploadData(VkRenderData& renderData, VkIndexBufferData& indexBufferData,
		const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView);
	static void cleanup(VkRenderData& renderData, VkIndexBufferData& indexBufferData);
};


#endif