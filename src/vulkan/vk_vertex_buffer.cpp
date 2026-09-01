#include "vk_vertex_buffer.h"
#include "command_buffer.h"
#include "logger.h"

bool VkVertexBuffer::init(VkRenderData& renderData, VkVertexBufferData& vertexBufferData, unsigned int vertexBufferSize)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = vertexBufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo bufferAllocInfo{};
	bufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	if(vmaCreateBuffer(renderData.rendererAllocator, &bufferInfo, &bufferAllocInfo, &vertexBufferData.rendererVertexBuffer, &vertexBufferData.rendererVertexBufferAllocation, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not allocate vertex buffer via VMA\n", __FUNCTION__);
		return false;
	}

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = vertexBufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo staggingBufferAllocInfo{};
	staggingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	if (vmaCreateBuffer(renderData.rendererAllocator, &stagingBufferInfo, &staggingBufferAllocInfo, &vertexBufferData.rendererStagingBuffer, &vertexBufferData.rendererStagingBufferAllocation, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not allocate vertex staging buffer via VMA\n", __FUNCTION__);
		return false;
	}

	vertexBufferData.rendererVertexBufferSize = vertexBufferSize;
	return true;
}

bool VkVertexBuffer::uploadData(VkRenderData& renderData, VkVertexBufferData& vertexBufferData, VkMesh vertexData)
{	
	unsigned int vertexDataSize = vertexData.vertices.size() * sizeof(VkVertex);

	if(vertexBufferData.rendererVertexBufferSize < vertexDataSize)
	{
		vertexBufferData.rendererVertexBufferSize = vertexDataSize;
		cleanup(renderData, vertexBufferData);

		if(!init(renderData, vertexBufferData, vertexDataSize))
		{
			Logger::log(1, "%s error: could not create vertex buffer of size %i bytes\n", __FUNCTION__, vertexDataSize);
		}
	}

	void* data;
	vmaMapMemory(renderData.rendererAllocator, vertexBufferData.rendererStagingBufferAllocation, &data);
	std::memcpy(data, vertexData.vertices.data(), vertexDataSize);
	vmaUnmapMemory(renderData.rendererAllocator, vertexBufferData.rendererStagingBufferAllocation);

	VkBufferMemoryBarrier vertexBufferBarrier{};
	vertexBufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	vertexBufferBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;;
	vertexBufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	vertexBufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vertexBufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vertexBufferBarrier.buffer = vertexBufferData.rendererStagingBuffer;
	vertexBufferBarrier.offset = 0;
	vertexBufferBarrier.size = vertexDataSize;

	VkBufferCopy stagingBufferCopy{};
	stagingBufferCopy.srcOffset = 0;
	stagingBufferCopy.dstOffset = 0;
	stagingBufferCopy.size = vertexDataSize;

	vkCmdCopyBuffer(renderData.rendererCommandBuffer, vertexBufferData.rendererStagingBuffer, vertexBufferData.rendererVertexBuffer, 1, &stagingBufferCopy);
	vkCmdPipelineBarrier(renderData.rendererCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &vertexBufferBarrier, 0, nullptr);

	return true;
}

bool VkVertexBuffer::uploadData(VkRenderData& renderData, VkVertexBufferData& vertexBufferData,
	const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView)
{
	if (vertexBufferData.rendererVertexBufferSize < bufferView.byteLength) 
	{
		cleanup(renderData, vertexBufferData);

		if(!init(renderData, vertexBufferData, bufferView.byteLength))
		{
			Logger::log(1, "%s error: could not create vertex buffer of size %i bytes\n", __FUNCTION__, bufferView.byteLength);
			return false;
		}
		Logger::log(1, "%s: vertex buffer was resized to %i bytes \n", __FUNCTION__, bufferView.byteLength);
		vertexBufferData.rendererVertexBufferSize = bufferView.byteLength;
	}

	void* data;
	vmaMapMemory(renderData.rendererAllocator, vertexBufferData.rendererStagingBufferAllocation, &data);
	std::memcpy(data, &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
	vmaUnmapMemory(renderData.rendererAllocator, vertexBufferData.rendererStagingBufferAllocation);

	VkBufferMemoryBarrier vertexBufferBarrier{};
	vertexBufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	vertexBufferBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;;
	vertexBufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	vertexBufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vertexBufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vertexBufferBarrier.buffer = vertexBufferData.rendererStagingBuffer;
	vertexBufferBarrier.offset = 0;
	vertexBufferBarrier.size = vertexBufferData.rendererVertexBufferSize;

	VkBufferCopy stagingBufferCopy{};
	stagingBufferCopy.srcOffset = 0;
	stagingBufferCopy.dstOffset = 0;
	stagingBufferCopy.size = vertexBufferData.rendererVertexBufferSize;

	vkCmdCopyBuffer(renderData.rendererCommandBuffer, vertexBufferData.rendererStagingBuffer, vertexBufferData.rendererVertexBuffer, 1, &stagingBufferCopy);
	vkCmdPipelineBarrier(renderData.rendererCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &vertexBufferBarrier, 0, nullptr);

	return true;
}

void VkVertexBuffer::cleanup(VkRenderData& renderData, VkVertexBufferData& vertexBufferData)
{
	vmaDestroyBuffer(renderData.rendererAllocator, vertexBufferData.rendererStagingBuffer, vertexBufferData.rendererStagingBufferAllocation);
	vmaDestroyBuffer(renderData.rendererAllocator, vertexBufferData.rendererVertexBuffer, vertexBufferData.rendererVertexBufferAllocation);
}