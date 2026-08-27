#include "command_buffer.h"
#include "logger.h"

#include <VkBootstrap.h>

bool CommandBuffer::init(VkRenderData& renderData, VkCommandBuffer& commandBuffer)
{
	VkCommandBufferAllocateInfo bufferAllocInfo{};
	bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocInfo.commandPool = renderData.rendererCommandPool;
	bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferAllocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(renderData.rendererVkbDevice.device, &bufferAllocInfo, &commandBuffer) != VK_SUCCESS) 
	{
		Logger::log(1, "%s error: could not allocate command buffers\n", __FUNCTION__);
		return false;
	}

	return true;
}

void CommandBuffer::cleanup(VkRenderData& renderData, VkCommandBuffer& commandBuffer) 
{
	vkFreeCommandBuffers(renderData.rendererVkbDevice.device, renderData.rendererCommandPool, 1, &commandBuffer);
}