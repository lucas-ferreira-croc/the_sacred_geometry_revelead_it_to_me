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
	glm::vec3 cameraPosition;
};


struct VkTextureData 
{
	VkImage textureImage = VK_NULL_HANDLE;
	VkImageView textureImageView = VK_NULL_HANDLE;
	VkSampler textureSampler = VK_NULL_HANDLE;
	VmaAllocation textureImageAllocation = VK_NULL_HANDLE;

	VkDescriptorPool textureDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout textureDescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorSet textureDescriptorSet = VK_NULL_HANDLE;
};

struct VkVertexBufferData 
{
	unsigned int rendererVertexBufferSize = 0;
	VkBuffer rendererVertexBuffer = VK_NULL_HANDLE;
	VmaAllocation rendererVertexBufferAllocation = nullptr;
	VkBuffer rendererStagingBuffer = VK_NULL_HANDLE;
	VmaAllocation rendererStagingBufferAllocation = nullptr;
};

struct VkIndexBufferData
{
	unsigned int rendererIndexBufferSize = 0;
	VkBuffer rendererIndexBuffer = VK_NULL_HANDLE;
	VmaAllocation rendererIndexBufferAllocation = nullptr;
	VkBuffer rendererStagingBuffer = VK_NULL_HANDLE;
	VmaAllocation rendererStagingBufferAllocation = nullptr;
};

struct VkGltfPrimitiveData
{
	std::vector<VkVertexBufferData> rendererGltfVertexBufferData{};
	VkIndexBufferData rendererGltfIndexBufferData{};
	uint32_t indexCount = 0;
	VkIndexType indexType;
};

struct VkGltfMesh
{
	std::vector<VkGltfPrimitiveData> primitives{};
};

struct VkGltfRenderData
{
	std::vector<VkGltfMesh> meshes{};
	VkTextureData rendererGltfModelTexture{};
};



struct VkRenderData
{
	GLFWwindow* rendererWindow = nullptr;

	int rendererWidth = 0;
	int rendererHeight = 0;

	unsigned int rendererTringleCount = 0;
	unsigned int gltfTriangleCount = 0;

	int rendererFieldOfView = 90;

	float rendererFrameTime = 0.0f;
	float rendererUIGenerateTime = 0.0f;
	float matrixGenerateTime = 0.0f;
	float uplaodToUBOTime = 0.0f;
	float uiDrawTime = 0.0f;
	bool rendererUseChangedShader = false;
	
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

	VkPipelineLayout rendererGltfPipelineLayout = VK_NULL_HANDLE;
	VkPipeline rendererGltfPipeline = VK_NULL_HANDLE;

	VkCommandPool rendererCommandPool = VK_NULL_HANDLE;
	VkCommandBuffer rendererCommandBuffer = VK_NULL_HANDLE;

	VkSemaphore rendererPresentSemaphore = VK_NULL_HANDLE;
	VkSemaphore rendererRenderSemaphore = VK_NULL_HANDLE;
	VkFence rendererPresentFence = VK_NULL_HANDLE;
	VkFence rendererRenderFence = VK_NULL_HANDLE;

	VkTextureData rendererModelTexture{};
	VkVertexBufferData rendererVertexBufferData{};

	VkBuffer rendererUboBuffer = VK_NULL_HANDLE;
	VmaAllocation rendererUboBufferAlloc = nullptr;

	VkDescriptorPool rendererUBODescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout rendererUBODescriptorLayout = VK_NULL_HANDLE;
	VkDescriptorSet rendererUboDescriptorSet = VK_NULL_HANDLE;

	VkDescriptorPool rendererImguiDescriptorPool = VK_NULL_HANDLE;
};

#endif