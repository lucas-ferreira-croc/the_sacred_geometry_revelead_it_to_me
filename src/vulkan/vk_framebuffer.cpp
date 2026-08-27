#include "vk_framebuffer.h"
#include "logger.h"

bool VkRendererFramebuffer::init(VkRenderData& renderData)
{
	renderData.rendererSwapchainImages = renderData.rendererVkbSwapchain.get_images().value();
	renderData.rendererSwapchainImageViews = renderData.rendererVkbSwapchain.get_image_views().value();

	renderData.rendererFramebuffers.resize(renderData.rendererSwapchainImageViews.size());

	for (unsigned int i = 0; i < renderData.rendererSwapchainImageViews.size(); i++) 
	{
		VkImageView attachments[] = { renderData.rendererSwapchainImageViews.at(i), renderData.rendererDepthImageView };

		VkFramebufferCreateInfo fboInfo{};
		fboInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fboInfo.renderPass = renderData.rendererRenderpass;
		fboInfo.attachmentCount = 2;
		fboInfo.pAttachments = attachments;
		fboInfo.width = renderData.rendererVkbSwapchain.extent.width;
		fboInfo.height = renderData.rendererVkbSwapchain.extent.height;
		fboInfo.layers = 1;

		if(vkCreateFramebuffer(renderData.rendererVkbDevice.device, &fboInfo, nullptr, &renderData.rendererFramebuffers[i]) != VK_SUCCESS)
		{
			Logger::log(1, "%s error: failed to create framebuffer %i\n", __FUNCTION__, i);
			return false;
		}
	}

	return true;
}

void VkRendererFramebuffer::cleanup(VkRenderData& renderData)
{
	for(auto& framebuffer : renderData.rendererFramebuffers)
	{
		vkDestroyFramebuffer(renderData.rendererVkbDevice.device, framebuffer, nullptr);
	}
}