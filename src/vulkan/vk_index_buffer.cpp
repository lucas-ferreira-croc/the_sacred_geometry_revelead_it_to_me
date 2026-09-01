#include "vk_index_buffer.h"

#include <cstring>

#include "command_buffer.h"

#include "logger.h"

bool VkIndexBuffer::init(VkRenderData& renderData, VkIndexBufferData& indexBufferData, unsigned int bufferSize)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo bufferAllocInfo{};
	bufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	if (vmaCreateBuffer(renderData.rendererAllocator, &bufferInfo, &bufferAllocInfo,
		&indexBufferData.rendererIndexBuffer, &indexBufferData.rendererIndexBufferAllocation, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s: error: could not allocate index buffer via VMA\n", __FUNCTION__);
		return false;
	}


	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingBufferAllocInfo{};
	stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	if (vmaCreateBuffer(renderData.rendererAllocator, &stagingBufferInfo, &stagingBufferAllocInfo,
		&indexBufferData.rendererStagingBuffer, &indexBufferData.rendererStagingBufferAllocation, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s: error: could not allocate index staging buffer via VMA\n", __FUNCTION__);
		return false;
	}
	
	indexBufferData.rendererIndexBufferSize = bufferSize;
	return true;
}

bool VkIndexBuffer::uploadData(VkRenderData& renderData, VkIndexBufferData& indexBufferData, const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView)
{
	if(indexBufferData.rendererIndexBufferSize < bufferView.byteLength)
	{
		cleanup(renderData, indexBufferData);
		if(!init(renderData, indexBufferData, bufferView.byteLength))
		{
			Logger::log(1, "%s error: could not create index buffer os size %i bytes", __FUNCTION__, bufferView.byteLength);
			return false;
		}
		Logger::log(1, "%s index buffer resized to %i bytes", __FUNCTION__, bufferView.byteLength);
		indexBufferData.rendererIndexBufferSize = bufferView.byteLength;
	}

	void* data;
	vmaMapMemory(renderData.rendererAllocator, indexBufferData.rendererStagingBufferAllocation, &data);
	std::memcpy(data, &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
	vmaUnmapMemory(renderData.rendererAllocator, indexBufferData.rendererStagingBufferAllocation);

	VkBufferMemoryBarrier vertexBufferBarrier{};
	vertexBufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	vertexBufferBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;;
	vertexBufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	vertexBufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vertexBufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vertexBufferBarrier.buffer = indexBufferData.rendererStagingBuffer;
	vertexBufferBarrier.offset = 0;
	vertexBufferBarrier.size = indexBufferData.rendererIndexBufferSize;


	VkBufferCopy stagingBufferCopy{};
	stagingBufferCopy.srcOffset = 0;
	stagingBufferCopy.dstOffset = 0;
	stagingBufferCopy.size = indexBufferData.rendererIndexBufferSize;

	vkCmdCopyBuffer(renderData.rendererCommandBuffer, indexBufferData.rendererStagingBuffer, indexBufferData.rendererIndexBuffer, 1, &stagingBufferCopy);
	vkCmdPipelineBarrier(renderData.rendererCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &vertexBufferBarrier, 0, nullptr);
	
	return true;
}


void VkIndexBuffer::cleanup(VkRenderData& renderData, VkIndexBufferData& indexBufferData)
{
	vmaDestroyBuffer(renderData.rendererAllocator, indexBufferData.rendererStagingBuffer, indexBufferData.rendererStagingBufferAllocation);
	vmaDestroyBuffer(renderData.rendererAllocator, indexBufferData.rendererIndexBuffer, indexBufferData.rendererIndexBufferAllocation);
}
