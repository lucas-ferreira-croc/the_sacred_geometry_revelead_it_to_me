#include "vk_uniform_buffer.h"
#include "logger.h"

#include <VkBootstrap.h>

bool VkRendererUniformBuffer::init(VkRenderData& renderData)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(VkUploadMatrices);
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo vmaAllocInfo{};
	vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	if (vmaCreateBuffer(renderData.rendererAllocator, &bufferInfo, &vmaAllocInfo, &renderData.rendererUboBuffer, &renderData.rendererUboBufferAlloc, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not allocate uniform buffer via VMA\n", __FUNCTION__);
		return false;
	}

	VkDescriptorSetLayoutBinding uboBind{};
	uboBind.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboBind.binding = 0;
	uboBind.descriptorCount = 1;
	uboBind.pImmutableSamplers = nullptr;
	uboBind.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo uboCreateInfo{};
	uboCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	uboCreateInfo.bindingCount = 1;
	uboCreateInfo.pBindings = &uboBind;

	if (vkCreateDescriptorSetLayout(renderData.rendererVkbDevice.device, &uboCreateInfo, nullptr, &renderData.rendererUBODescriptorLayout) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not create UBO descritpor set layout\n", __FUNCTION__);
		return false;
	}

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPool{};
	descriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPool.poolSizeCount = 1;
	descriptorPool.pPoolSizes = &poolSize;
	descriptorPool.maxSets = 1;

	if (vkCreateDescriptorPool(renderData.rendererVkbDevice.device, &descriptorPool, nullptr, &renderData.rendererUBODescriptorPool) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not create UBO descritpor pool\n", __FUNCTION__);
		return false;
	}

	VkDescriptorSetAllocateInfo descritporAllocateInfo{};
	descritporAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descritporAllocateInfo.descriptorPool = renderData.rendererUBODescriptorPool;
	descritporAllocateInfo.descriptorSetCount = 1;
	descritporAllocateInfo.pSetLayouts = &renderData.rendererUBODescriptorLayout;

	if (vkAllocateDescriptorSets(renderData.rendererVkbDevice.device, &descritporAllocateInfo, &renderData.rendererUboDescriptorSet) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not allocate UBO descriptor set\n", __FUNCTION__);
		return false;
	}

	VkDescriptorBufferInfo uboInfo{};
	uboInfo.buffer = renderData.rendererUboBuffer;
	uboInfo.offset = 0;
	uboInfo.range = sizeof(VkUploadMatrices);

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.dstSet = renderData.rendererUboDescriptorSet;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.pBufferInfo = &uboInfo;

	vkUpdateDescriptorSets(renderData.rendererVkbDevice.device, 1, &writeDescriptorSet, 0, nullptr);

	return true;
}

void VkRendererUniformBuffer::uploadData(VkRenderData& renderData, VkUploadMatrices matrices)
{
	void* data;
	vmaMapMemory(renderData.rendererAllocator, renderData.rendererUboBufferAlloc, &data);
	std::memcpy(data, &matrices, sizeof(matrices));
	vmaUnmapMemory(renderData.rendererAllocator, renderData.rendererUboBufferAlloc);
	vmaFlushAllocation(renderData.rendererAllocator, renderData.rendererUboBufferAlloc, 0, sizeof(VkUploadMatrices));
}

void VkRendererUniformBuffer::cleanup(VkRenderData& renderData)
{
	vkDestroyDescriptorPool(renderData.rendererVkbDevice.device, renderData.rendererUBODescriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(renderData.rendererVkbDevice.device, renderData.rendererUBODescriptorLayout, nullptr);
	vmaDestroyBuffer(renderData.rendererAllocator, renderData.rendererUboBuffer, renderData.rendererUboBufferAlloc);
}