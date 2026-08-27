#include "sync_objects.h"
#include "logger.h"

#include <VkBootstrap.h>

bool SyncObjects::init(VkRenderData& renderData)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(renderData.rendererVkbDevice.device, &semaphoreInfo, nullptr, &renderData.rendererPresentSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(renderData.rendererVkbDevice.device, &semaphoreInfo, nullptr, &renderData.rendererRenderSemaphore) != VK_SUCCESS ||
        vkCreateFence(renderData.rendererVkbDevice.device, &fenceInfo, nullptr, &renderData.rendererPresentFence) != VK_SUCCESS ||
        vkCreateFence(renderData.rendererVkbDevice.device, &fenceInfo, nullptr, &renderData.rendererRenderFence) != VK_SUCCESS)
    {
        Logger::log(1, "%s error: failed to init sync objects\n", __FUNCTION__);
        return false;
    }
    return true;
}

void SyncObjects::cleanup(VkRenderData& renderData)
{
    vkDestroySemaphore(renderData.rendererVkbDevice.device, renderData.rendererPresentSemaphore, nullptr);
    vkDestroySemaphore(renderData.rendererVkbDevice.device, renderData.rendererRenderSemaphore, nullptr);
    vkDestroyFence(renderData.rendererVkbDevice.device, renderData.rendererPresentFence, nullptr);
    vkDestroyFence(renderData.rendererVkbDevice.device, renderData.rendererRenderFence, nullptr);
}