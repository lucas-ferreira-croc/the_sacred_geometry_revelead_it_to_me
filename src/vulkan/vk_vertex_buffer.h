#ifndef VK_VERTEX_BUFFER_H
#define VK_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include <tiny_gltf.h>

#include "vk_render_data.h"


class VkVertexBuffer
{
public:
	static bool init(VkRenderData& renderData, VkVertexBufferData& vertexBufferData, unsigned int vertexBufferSize);
	static bool uploadData(VkRenderData& renderData, VkVertexBufferData& vertexBufferData, VkMesh vertexData);
	static bool uploadData(VkRenderData& renderData, VkVertexBufferData& vertexBufferData, const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView);

	static void cleanup(VkRenderData& renderData, VkVertexBufferData& vertexBufferData);
};

#endif