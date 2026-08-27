#ifndef VK_RENDER_DATA_H
#define VK_RENDER_DATA_H

#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

struct VkVertex 
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
};

struct VkMesh
{
	std::vector<VkVertex> vertices;
};

struct VkUploadMatrices
{
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};

struct VkRenderData
{
	GLFWwindow* rendererWindow = nullptr;
	int rendererWidth = 0;
	int rendererHeight = 0;

	unsigned int rendererTringleCount = 0;
	float rendererFrameTime = 0.0f;
	float rendererUIGenerateTime = 0.0f;
	float matrixGenerateTime = 0.0f;
	float uplaodToUBOTime = 0.0f;
	float uiDrawTime = 0.0f;
	bool rendererUseChangedShader = false;
	int rendererFieldOfView = 90;
	float rendererViewYaw;
	float rendererViewPitch;

	int rendererMoveForward = 0;
	int rendererMoveRight = 0;
	int rendererMoveUp = 0;
	float rendererTickDiff = 0.0f;
	glm::vec3 m_RendererCameraWorldPos = glm::vec3(0.5f, 0.25f, 1.0f);

	VmaAllocator rendererAllocator;

	vkb::Instance rendererVkbInstance{};
	vkb::PhysicalDevice rendererVkbPhysicalDevice{};
	vkb::Device rendererVkbDevice{};
	vkb::Swapchain rendererVkbSwapchain{};
	
	std::vector<VkImage> rendererSwapchainImages;
	std::vector<VkImageView> rendererSwapchainImageViews;
	std::vector<VkFramebuffer> rendererFramebuffers;

	VkQueue rendererGraphicsQueue = VK_NULL_HANDLE;
	VkQueue rendererPresentQueue = VK_NULL_HANDLE;

	VkImage rendererDepthImage = VK_NULL_HANDLE;
	VkImageView rendererDepthImageView = VK_NULL_HANDLE;
	VkFormat rendererDepthFormat;
	VmaAllocation rendererDepthImageAlloc = VK_NULL_HANDLE;

	VkRenderPass rendererRenderpass = VK_NULL_HANDLE;
	VkPipelineLayout rendererPipelineLayout = VK_NULL_HANDLE;
	VkPipeline rendererPipeline = VK_NULL_HANDLE;
	VkPipeline rendererChangedPipeline = VK_NULL_HANDLE;

	VkCommandPool rendererCommandPool = VK_NULL_HANDLE;
	VkCommandBuffer rendererCommandBuffer = VK_NULL_HANDLE;

	VkSemaphore rendererPresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore rendererRenderSemaphore = VK_NULL_HANDLE;
	VkFence rendererPresentFence = VK_NULL_HANDLE;
	VkFence rendererRenderFence = VK_NULL_HANDLE;

	VkImage rendererTextureImage = VK_NULL_HANDLE;
	VkImageView rendererTextureImageView = VK_NULL_HANDLE;
	VkSampler rendererTextureSampler = VK_NULL_HANDLE;
	VmaAllocation rendererTextureImageAlloc = VK_NULL_HANDLE;

	VkDescriptorPool rendererTextureDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout rendererTextureDescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorSet rendererTextureDescriptorSet = VK_NULL_HANDLE;

	VkBuffer rendererUboBuffer = VK_NULL_HANDLE;
	VmaAllocation rendererUboBufferAlloc = nullptr;

	VkDescriptorPool rendererUBODescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout rendererUBODescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorSet rendererUboDescriptorSet = VK_NULL_HANDLE;

	VkDescriptorPool rendererImguiDescriptorPool = VK_NULL_HANDLE;
};

#endif