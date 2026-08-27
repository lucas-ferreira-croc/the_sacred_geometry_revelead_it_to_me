#include "vk_pipeline_layout.h"
#include "logger.h"

#include <VkBootstrap.h>

bool VkRendererPipelineLayout::init(VkRenderData& renderData, VkPipelineLayout& pipelineLayout)
{
	VkDescriptorSetLayout layouts[] = { renderData.rendererTextureDescriptorLayout, renderData.rendererUBODescriptorLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 2;
	pipelineLayoutInfo.pSetLayouts = layouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if(vkCreatePipelineLayout(renderData.rendererVkbDevice.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not create pipeline layout\n", __FUNCTION__);
		return false;
	}

	return true;
}

void VkRendererPipelineLayout::cleanup(VkRenderData& renderData, VkPipelineLayout& pipelineLayout)
{
	vkDestroyPipelineLayout(renderData.rendererVkbDevice.device, pipelineLayout, nullptr);
}