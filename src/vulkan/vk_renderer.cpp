#include <cstring>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"


#include "vk_renderer.h"
#include "vk_gltf_pipeline.h"
#include "logger.h"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui_impl_glfw.h"

VkRenderer::VkRenderer(GLFWwindow* window)
{
	m_RenderData.rendererWindow = window;
	m_Matrices.viewMatrix = glm::mat4(1.0f);
	m_Matrices.projectionMatrix = glm::mat4(1.0f);
}

bool VkRenderer::init(unsigned int width, unsigned int height)
{
	if(!m_RenderData.rendererWindow)
	{
		Logger::log(1, "%error: invalid GLFWwindow handle\n", __FUNCTION__);
		return false;
	}

	if (!deviceInit()) {
		return false;
	}

	if (!initVma()) {
		return false;
	}

	if (!getQueue()) {
		return false;
	}

	if (!createSwapchain()) {
		return false;
	}

	/* must be done AFTER swapchain as we need data from it */
	if (!createDepthBuffer()) {
		return false;
	}

	if (!createCommandPool()) {
		return false;
	}

	if (!createCommandBuffer()) {
		return false;
	}

	/* we need the command pool */
	if (!loadTexture()) {
		return false;
	}

	if (!createUBO()) {
		return false;
	}

	if (!createVBO()) {
		return false;
	}

	if (!createRenderPass()) {
		return false;
	}

	if(!createPipelineLayout())
	{
		return false;
	}


	/* pipeline needs texture layout */
	if (!createPipelines()) {
		return false;
	}

	if(!loadGltfModel())
	{
		return false;
	}

	if(!createGltfPipelineLayout())
	{
		return false;
	}

	if(!createGltfPipeline())
	{
		return false;
	}

	if (!createFramebuffer()) {
		return false;
	}

	if (!createSyncObjects()) {
		return false;
	}

	m_RenderData.rendererWidth = m_RenderData.rendererVkbSwapchain.extent.width;
	m_RenderData.rendererHeight = m_RenderData.rendererVkbSwapchain.extent.height;
	m_UserInterface.init(m_RenderData);
	Logger::log(1, "%s: user inteface initialized to \n", __FUNCTION__);

	Logger::log(1, "%s: Vulkan renderer initialized to %ix%i\n", __FUNCTION__, width, height);
	
	m_Model = std::make_unique<VkModel>();
	m_AllMeshes = std::make_unique<VkMesh>();

	m_FrameTimer.start();
	return true;
}

void VkRenderer::setSize(unsigned int width, unsigned int height)
{
	m_RenderData.rendererWidth = m_RenderData.rendererVkbSwapchain.extent.width;
	m_RenderData.rendererHeight = m_RenderData.rendererVkbSwapchain.extent.height;
	Logger::log(1, "%s: resized window to %ix%i\n", __FUNCTION__, width, height);
}


bool VkRenderer::draw() {

	double tickTime = glfwGetTime();
	m_RenderData.rendererTickDiff = tickTime - lastTickTime;

	m_RenderData.rendererFrameTime = m_FrameTimer.stop();
	m_FrameTimer.start();

	handleMovementKeys();

	m_AllMeshes->vertices.clear();

	if (vkWaitForFences(m_RenderData.rendererVkbDevice.device, 1, &m_RenderData.rendererRenderFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
		Logger::log(1, "%s error: waiting for fence failed\n", __FUNCTION__);
		return false;
	}

	if (vkWaitForFences(m_RenderData.rendererVkbDevice.device, 1, &m_RenderData.rendererPresentFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
		Logger::log(1, "%s error: waiting for present fence failed\n", __FUNCTION__);
		return false;
	}

	static float prevFrameStartTime = 0.0f;
	float frameStartTime = glfwGetTime();

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(m_RenderData.rendererVkbDevice.device,
		m_RenderData.rendererVkbSwapchain.swapchain,
		UINT64_MAX,
		m_RenderData.rendererPresentSemaphore,
		VK_NULL_HANDLE,
		&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		return VK_LINE_RASTERIZATION_MODE_RECTANGULAR;
	}
	else {
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			Logger::log(1, "%s error: failed to acquire swapchain image. Error is '%i'\n", __FUNCTION__, result);
			return false;
		}
	}

	if (vkResetFences(m_RenderData.rendererVkbDevice.device, 1, &m_RenderData.rendererRenderFence) != VK_SUCCESS) {
		Logger::log(1, "%s error: fence reset failed\n", __FUNCTION__);
		return false;
	}

	if (vkResetFences(m_RenderData.rendererVkbDevice.device, 1, &m_RenderData.rendererPresentFence) != VK_SUCCESS) {
		Logger::log(1, "%s error: presence fence reset failed\n", __FUNCTION__);
		return false;
	}

	if (vkResetCommandBuffer(m_RenderData.rendererCommandBuffer, 0) != VK_SUCCESS) {
		Logger::log(1, "%s error: failed to reset command buffer\n", __FUNCTION__);
		return false;
	}

	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(m_RenderData.rendererCommandBuffer, &cmdBeginInfo) != VK_SUCCESS) {
		Logger::log(1, "%s error: failed to begin command buffer\n", __FUNCTION__);
		return false;
	}

	VkClearValue colorClearValue;
	colorClearValue.color = { { 0.25f, 0.25f, 0.25f, 1.0f } };

	VkClearValue depthValue;
	depthValue.depthStencil.depth = 1.0f;

	VkClearValue clearValues[] = { colorClearValue, depthValue };

	VkRenderPassBeginInfo rpInfo{};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.renderPass = m_RenderData.rendererRenderpass;

	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = m_RenderData.rendererVkbSwapchain.extent;
	rpInfo.framebuffer = m_RenderData.rendererFramebuffers[imageIndex];

	rpInfo.clearValueCount = 2;
	rpInfo.pClearValues = clearValues;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_RenderData.rendererVkbSwapchain.extent.width);
	viewport.height = static_cast<float>(m_RenderData.rendererVkbSwapchain.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_RenderData.rendererVkbSwapchain.extent;

	auto& vertexData = m_Model->getVertexData();
	m_RenderData.rendererTringleCount = vertexData.vertices.size() / 3;
	m_AllMeshes->vertices.insert(m_AllMeshes->vertices.end(), vertexData.vertices.begin(), vertexData.vertices.end());

	m_MatrixGenerateTimer.start();
	glm::vec3 cameraPosition = glm::vec3(0.4f, 0.3f, 1.0f);
	glm::vec3 cameraLookAtPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

	m_Matrices.projectionMatrix = glm::perspective(glm::radians(static_cast<float>(m_RenderData.rendererFieldOfView)), static_cast<float>(m_RenderData.rendererWidth) / static_cast<float>(m_RenderData.rendererHeight), 0.1f, 100.0f);

	float t = glfwGetTime();
	glm::mat4 model = glm::mat4(1.0f);
	if (m_RenderData.rendererUseChangedShader)
	{
		//model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), -t, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 flipX = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = model /** rotationY */* flipX;
	}
	m_Matrices.viewMatrix = m_Camera.getViewMatrix(m_RenderData) * model;
	m_RenderData.matrixGenerateTime = m_MatrixGenerateTimer.stop();
	VkVertexBuffer::uploadData(m_RenderData, m_RenderData.rendererVertexBufferData, *m_AllMeshes);

	m_GltfModel->uploadVertexBuffers(m_RenderData, m_GltfRenderData);
	m_GltfModel->uploadIndexBuffers(m_RenderData, m_GltfRenderData);

	m_UploadToUBOTimer.start();
	m_Matrices.cameraPosition = m_RenderData.m_RendererCameraWorldPos;
	VkRendererUniformBuffer::uploadData(m_RenderData, m_Matrices);
	m_RenderData.uplaodToUBOTime = m_UploadToUBOTimer.stop();

	vkCmdBeginRenderPass(m_RenderData.rendererCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	auto pipeline = m_RenderData.rendererUseChangedShader ? m_RenderData.rendererChangedPipeline : m_RenderData.rendererPipeline;

	/* the rendering itself happens here */
	vkCmdBindPipeline(m_RenderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	/* required for dynamic viewport */
	vkCmdSetViewport(m_RenderData.rendererCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(m_RenderData.rendererCommandBuffer, 0, 1, &scissor);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(m_RenderData.rendererCommandBuffer, 0, 1, &m_RenderData.rendererVertexBufferData.rendererVertexBuffer, &offset);
	vkCmdBindDescriptorSets(m_RenderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_RenderData.rendererPipelineLayout, 0, 1, & m_RenderData.rendererModelTexture.textureDescriptorSet, 0, nullptr);
	vkCmdBindDescriptorSets(m_RenderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_RenderData.rendererPipelineLayout, 1, 1, &m_RenderData.rendererUboDescriptorSet, 0, nullptr);

	//vkCmdDraw(m_RenderData.rendererCommandBuffer, m_RenderData.rendererTringleCount * 3, 1, 0, 0);

	

	m_GltfModel->draw(m_RenderData, m_GltfRenderData);

	m_UIGeneratorTimer.start();
	m_UserInterface.createFrame(m_RenderData);
	m_RenderData.rendererUIGenerateTime = m_UIGeneratorTimer.stop();

	m_UIDrawTimer.start();
	m_UserInterface.render(m_RenderData);
	m_RenderData.uiDrawTime = m_UIDrawTimer.stop();

	vkCmdEndRenderPass(m_RenderData.rendererCommandBuffer);

	if (vkEndCommandBuffer(m_RenderData.rendererCommandBuffer) != VK_SUCCESS) {
		Logger::log(1, "%s error: failed to end command buffer\n", __FUNCTION__);
		return false;
	}



	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submitInfo.pWaitDstStageMask = &waitStage;

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_RenderData.rendererPresentSemaphore;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_RenderData.rendererRenderSemaphore;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_RenderData.rendererCommandBuffer;

	if (vkQueueSubmit(m_RenderData.rendererGraphicsQueue, 1, &submitInfo, m_RenderData.rendererRenderFence) != VK_SUCCESS) {
		Logger::log(1, "%s error: failed to submit draw command buffer\n", __FUNCTION__);
		return false;
	}

	/* Extra fence for presentation */
	VkSwapchainPresentFenceInfoEXT presentFenceInfo{};
	presentFenceInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_FENCE_INFO_EXT;
	presentFenceInfo.swapchainCount = 1;
	presentFenceInfo.pFences = &m_RenderData.rendererPresentFence;

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = &presentFenceInfo;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_RenderData.rendererRenderSemaphore;

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_RenderData.rendererVkbSwapchain.swapchain;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_RenderData.rendererPresentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return recreateSwapChain();
	}
	else {
		if (result != VK_SUCCESS) {
			Logger::log(1, "%s error: failed to present swapchain image\n", __FUNCTION__);
			return false;
		}
	}

	m_RenderData.rendererFrameTime = frameStartTime - prevFrameStartTime;
	prevFrameStartTime = frameStartTime;

	lastTickTime = tickTime;

	return true;
}

void VkRenderer::cleanup() 
{
	vkDeviceWaitIdle(m_RenderData.rendererVkbDevice.device);

	m_GltfModel->cleanup(m_RenderData, m_GltfRenderData);
	m_GltfModel.reset();

	m_UserInterface.cleanup(m_RenderData);
	Logger::log(1, "%s: Vulkan user inteface destroyed\n", __FUNCTION__);

	SyncObjects::cleanup(m_RenderData);
	CommandBuffer::cleanup(m_RenderData, m_RenderData.rendererCommandBuffer);
	CommandPool::cleanup(m_RenderData);
	VkRendererFramebuffer::cleanup(m_RenderData);
	GltfPipeline::cleanup(m_RenderData, m_RenderData.rendererGltfPipeline);
	VkRendererPipeline::cleanup(m_RenderData, m_RenderData.rendererPipeline);
	VkRendererPipeline::cleanup(m_RenderData, m_RenderData.rendererChangedPipeline);
	VkRendererPipelineLayout::cleanup(m_RenderData, m_RenderData.rendererPipelineLayout);
	VkRendererPipelineLayout::cleanup(m_RenderData, m_RenderData.rendererGltfPipelineLayout);
	VkRendererRenderPass::cleanup(m_RenderData);
	VkRendererUniformBuffer::cleanup(m_RenderData);
	VkVertexBuffer::cleanup(m_RenderData, m_RenderData.rendererVertexBufferData);
	VkRendererTexture::cleanup(m_RenderData, m_RenderData.rendererModelTexture);


	vkDestroyImageView(m_RenderData.rendererVkbDevice.device, m_RenderData.rendererDepthImageView, nullptr);
	vmaDestroyImage(m_RenderData.rendererAllocator, m_RenderData.rendererDepthImage, m_RenderData.rendererDepthImageAlloc);
	vmaDestroyAllocator(m_RenderData.rendererAllocator);
	
	m_RenderData.rendererVkbSwapchain.destroy_image_views(m_RenderData.rendererSwapchainImageViews);
	vkb::destroy_swapchain(m_RenderData.rendererVkbSwapchain);

	vkb::destroy_device(m_RenderData.rendererVkbDevice);
	vkb::destroy_surface(m_RenderData.rendererVkbInstance.instance, m_Surface);
	vkb::destroy_instance(m_RenderData.rendererVkbInstance);
	
	Logger::log(1, "%s: Vulkan renderer destroyed\n", __FUNCTION__);
}


bool VkRenderer::deviceInit()
{
	vkb::InstanceBuilder instanceBuilder;
	auto instRet = instanceBuilder
		.use_default_debug_messenger()
		.request_validation_layers()
		.enable_extension(VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME)
		.enable_extension(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)
		.require_api_version(1, 1, 0)
		.build();

	if (!instRet) 
	{
		Logger::log(1, "%s error; could not build vk instance\n", __FUNCTION__);
		return false;
	}

	m_RenderData.rendererVkbInstance = instRet.value();

	VkResult result = VK_ERROR_UNKNOWN;
	result = glfwCreateWindowSurface(m_RenderData.rendererVkbInstance, m_RenderData.rendererWindow, nullptr, &m_Surface);
	if (result != VK_SUCCESS) 
	{
		Logger::log(1, "%s error: Could not create Vulkan surface\n", __FUNCTION__);
		return false;
	}

	VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT swapchainMaintenance1{};
	swapchainMaintenance1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT;
	swapchainMaintenance1.swapchainMaintenance1 = VK_TRUE;

	vkb::PhysicalDeviceSelector physicalDevSel{ m_RenderData.rendererVkbInstance };
	auto physicalDevSelRet = physicalDevSel
		.set_surface(m_Surface)
		.add_required_extension(VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME)
		.add_required_extension_features(swapchainMaintenance1)
		.select();

	if (!physicalDevSelRet) {
		Logger::log(1, "%s error: could not get physical devices\n", __FUNCTION__);
		return false;
	}
	m_RenderData.rendererVkbPhysicalDevice = physicalDevSelRet.value();

	Logger::log(1, "%s: found physical device '%s'\n", __FUNCTION__, m_RenderData.rendererVkbPhysicalDevice.name.c_str());

	vkb::DeviceBuilder devBuilder{ m_RenderData.rendererVkbPhysicalDevice };
	auto devBuilderRet = devBuilder.build();
	if (!devBuilderRet) {
		Logger::log(1, "%s error: could not get devices\n", __FUNCTION__);
		return false;
	}
	m_RenderData.rendererVkbDevice = devBuilderRet.value();

	return true;
}

bool VkRenderer::getQueue() {
	auto graphQueueRet = m_RenderData.rendererVkbDevice.get_queue(vkb::QueueType::graphics);
	if (!graphQueueRet.has_value()) {
		Logger::log(1, "%s error: could not get graphics queue\n", __FUNCTION__);
		return false;
	}
	m_RenderData.rendererGraphicsQueue = graphQueueRet.value();

	auto presentQueueRet = m_RenderData.rendererVkbDevice.get_queue(vkb::QueueType::present);
	if (!presentQueueRet.has_value()) {
		Logger::log(1, "%s error: could not get present queue\n", __FUNCTION__);
		return false;
	}
	m_RenderData.rendererPresentQueue = presentQueueRet.value();

	return true;
}


bool VkRenderer::createDepthBuffer()
{
	VkExtent3D depthImageExtent =
	{
		  m_RenderData.rendererVkbSwapchain.extent.width,
		  m_RenderData.rendererVkbSwapchain.extent.height,
		  1
	};

	m_RenderData.rendererDepthFormat = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo depthImageInfo{};
	depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
	depthImageInfo.format = m_RenderData.rendererDepthFormat;
	depthImageInfo.extent = depthImageExtent;
	depthImageInfo.mipLevels = 1;
	depthImageInfo.arrayLayers = 1;
	depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VmaAllocationCreateInfo depthAllocInfo{};
	depthAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	depthAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vmaCreateImage(m_RenderData.rendererAllocator, &depthImageInfo, &depthAllocInfo, &m_RenderData.rendererDepthImage, &m_RenderData.rendererDepthImageAlloc, nullptr) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not allocate depth buffer memory\n", __FUNCTION__);
		return false;
	}

	VkImageViewCreateInfo depthImageViewinfo{};
	depthImageViewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthImageViewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageViewinfo.image = m_RenderData.rendererDepthImage;
	depthImageViewinfo.format = m_RenderData.rendererDepthFormat;
	depthImageViewinfo.subresourceRange.baseMipLevel = 0;
	depthImageViewinfo.subresourceRange.levelCount = 1;
	depthImageViewinfo.subresourceRange.baseArrayLayer = 0;
	depthImageViewinfo.subresourceRange.layerCount = 1;
	depthImageViewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	if (vkCreateImageView(m_RenderData.rendererVkbDevice.device, &depthImageViewinfo, nullptr, &m_RenderData.rendererDepthImageView) != VK_SUCCESS)
	{
		Logger::log(1, "%s error: could not create depth buffer image view\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createSwapchain() 
{
	vkb::SwapchainBuilder swapChainBuild{ m_RenderData.rendererVkbDevice };
	VkSurfaceFormatKHR surfaceFormat;

	/* set surface to non-sRGB */
	surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;

	/* VK_PRESENT_MODE_FIFO_KHR enables vsync */
	auto swapChainBuildRet = swapChainBuild
		.set_old_swapchain(m_RenderData.rendererVkbSwapchain)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_format(surfaceFormat)
		.build();

	if (!swapChainBuildRet)
	{
		Logger::log(1, "%s error: could not init swapchain\n", __FUNCTION__);
		return false;
	}

	vkb::destroy_swapchain(m_RenderData.rendererVkbSwapchain);
	m_RenderData.rendererVkbSwapchain = swapChainBuildRet.value();

	return true;
}

bool VkRenderer::recreateSwapChain()
{
	/* handle minimize */
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_RenderData.rendererWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(m_RenderData.rendererWindow, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_RenderData.rendererVkbDevice.device);

	/* cleanup */
	VkRendererFramebuffer::cleanup(m_RenderData);
	vkDestroyImageView(m_RenderData.rendererVkbDevice.device, m_RenderData.rendererDepthImageView, nullptr);
	vmaDestroyImage(m_RenderData.rendererAllocator, m_RenderData.rendererDepthImage, m_RenderData.rendererDepthImageAlloc);

	m_RenderData.rendererVkbSwapchain.destroy_image_views(m_RenderData.rendererSwapchainImageViews);

	/* and recreate */
	if (!createSwapchain()) 
	{
		Logger::log(1, "%s error: could not recreate swapchain\n", __FUNCTION__);
		return false;
	}

	if (!createDepthBuffer())
	{
		Logger::log(1, "%s error: could not recreate depth buffer\n", __FUNCTION__);
		return false;
	}

	if (!createFramebuffer()) 
	{
		Logger::log(1, "%s error: could not recreate framebuffers\n", __FUNCTION__);
		return false;
	}

	return true;
}

bool VkRenderer::createRenderPass() 
{
	if (!VkRendererRenderPass::init(m_RenderData))
	{
		Logger::log(1, "%s error: could not init renderpass\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createPipelineLayout()
{
	if(!VkRendererPipelineLayout::init(m_RenderData, m_RenderData.rendererModelTexture, m_RenderData.rendererPipelineLayout))
	{
		Logger::log(1, "%s error: could not init pipeline layout", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createPipelines()
{
	std::string vertexShaderFile = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\shaders\\basic_vk.vert.spv";
	std::string fragmentShaderFile = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\shaders\\basic_vk.frag.spv";
	if (!VkRendererPipeline::init(m_RenderData, m_RenderData.rendererPipelineLayout, m_RenderData.rendererPipeline, vertexShaderFile, fragmentShaderFile))
	{
		Logger::log(1, "%s error: could not init default pipeline\n", __FUNCTION__);
		return false;
	}
	
	vertexShaderFile = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\shaders\\changed_vk.vert.spv";
	fragmentShaderFile = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\shaders\\changed_vk.frag.spv";
	if (!VkRendererPipeline::init(m_RenderData, m_RenderData.rendererPipelineLayout, m_RenderData.rendererChangedPipeline, vertexShaderFile, fragmentShaderFile))
	{
		Logger::log(1, "%s error: could not init alternate pipeline\n", __FUNCTION__);
		return false;
	}

	return true;
}


bool VkRenderer::loadGltfModel()
{
	m_GltfModel = std::make_shared<GltfModel>();
	std::string modelFilename = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\assets\\models\\sacred.gltf";
	std::string textureFilename = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\assets\\models\\photo_cubas.png";

	if (!m_GltfModel->loadModel(m_RenderData, m_GltfRenderData, modelFilename, textureFilename))
	{
		Logger::log(1, "%s: loading gltf model '%s' failed \n", __FUNCTION__, modelFilename.c_str());
		return false;
	}

	return true;
}

bool VkRenderer::createGltfPipelineLayout()
{
	if(!VkRendererPipelineLayout::init(m_RenderData, m_GltfRenderData.rendererGltfModelTexture, m_RenderData.rendererGltfPipelineLayout))
	{
		Logger::log(1, "%s: could not initgltf pipeline layout\n", __FUNCTION__);
		return false;
	}
	
	return true;
}

bool VkRenderer::createGltfPipeline()
{
	
	std::string vertexShaderFile = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\shaders\\gltf.vert.spv";
	std::string fragmentShaderFile = "C:\\dev\\the_sacred_geometry_revelead_it_to_me\\shaders\\gltf.frag.spv";
	if (!GltfPipeline::init(m_RenderData, m_RenderData.rendererGltfPipelineLayout, m_RenderData.rendererGltfPipeline, 
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		vertexShaderFile, fragmentShaderFile))
	{
		Logger::log(1, "%s error: could not init gltf pipeline\n", __FUNCTION__);
		return false;
	}

	return true;
}

bool VkRenderer::createFramebuffer() 
{
	if (!VkRendererFramebuffer::init(m_RenderData)) 
	{
		Logger::log(1, "%s error: could not init framebuffer\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createCommandPool() 
{
	if (!CommandPool::init(m_RenderData))
	{
		Logger::log(1, "%s error: could not create command pool\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createCommandBuffer() 
{
	if (!CommandBuffer::init(m_RenderData, m_RenderData.rendererCommandBuffer)) {
		Logger::log(1, "%s error: could not create command buffers\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createSyncObjects() 
{
	if (!SyncObjects::init(m_RenderData)) 
	{
		Logger::log(1, "%s error: could not create sync objects\n", __FUNCTION__);
		return false;
	}

	return true;
}


bool VkRenderer::loadTexture()
{
	std::string textureFileName = "C:\\dev\\game_animation\\textures\\crate.png";
	if (!VkRendererTexture::loadTexture(m_RenderData, m_RenderData.rendererModelTexture, textureFileName)) {
		Logger::log(1, "%s error: could not load texture\n", __FUNCTION__);
		return false;
	}

	return true;
}

bool VkRenderer::createVBO()
{
	if (!VkVertexBuffer::init(m_RenderData, m_RenderData.rendererVertexBufferData, 2048))
	{
		Logger::log(1, "%s error: could not create vertex buffer object\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::createUBO()
{
	if(!VkRendererUniformBuffer::init(m_RenderData))
	{
		Logger::log(1, "%s error: could not create uniform buffers\n", __FUNCTION__);
		return false;
	}
	return true;
}

bool VkRenderer::initVma() 
{
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = m_RenderData.rendererVkbPhysicalDevice.physical_device;
	allocatorInfo.device = m_RenderData.rendererVkbDevice.device;
	allocatorInfo.instance = m_RenderData.rendererVkbInstance.instance;
	if (vmaCreateAllocator(&allocatorInfo, &m_RenderData.rendererAllocator) != VK_SUCCESS) {
		Logger::log(1, "%s error: could not init VMA\n", __FUNCTION__);
		return false;
	}

	return true;
}

void VkRenderer::handleKeyEvents(int key, int scancode, int action, int mods) 
{
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_RenderData.rendererUseChangedShader = !m_RenderData.rendererUseChangedShader;
	}
}

void VkRenderer::handleMouseButtonEvents(int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
	{
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	}

	if (io.WantCaptureMouse)
	{
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		m_MouseLock = !m_MouseLock;
	}

	if (m_MouseLock)
	{
		glfwSetInputMode(m_RenderData.rendererWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_RenderData.rendererWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
	}
	else
	{
		glfwSetInputMode(m_RenderData.rendererWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void VkRenderer::handleMousePositionEvents(double xPos, double yPos)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(xPos, yPos);
	if (io.WantCaptureMouse)
	{
		return;
	}

	int mouseMoveRelativeX = static_cast<int>(xPos) - m_MouseXPos;
	int mouseMoveRelativeY = static_cast<int>(yPos) - m_MouseYPos;

	if (m_MouseLock)
	{
		m_RenderData.rendererViewYaw += mouseMoveRelativeX / 10.0f;
		if (m_RenderData.rendererViewYaw < 0.0f)
		{
			m_RenderData.rendererViewYaw += 360.0f;
		}
		if (m_RenderData.rendererViewYaw >= 360.0f)
		{
			m_RenderData.rendererViewYaw -= 360.0f;
		}

		m_RenderData.rendererViewPitch -= mouseMoveRelativeY / 10.0f;
		if (m_RenderData.rendererViewPitch > 89.0f)
		{
			m_RenderData.rendererViewPitch = 89.0f;
		}
		if (m_RenderData.rendererViewPitch < -89.0f)
		{
			m_RenderData.rendererViewPitch = -89.0f;
		}
	}
	m_MouseXPos = static_cast<int>(xPos);
	m_MouseYPos = static_cast<int>(yPos);
}

void VkRenderer::handleMovementKeys()
{
	m_RenderData.rendererMoveForward = 0;
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_RenderData.rendererMoveForward += 1;
	}
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_RenderData.rendererMoveForward -= 1;
	}

	m_RenderData.rendererMoveRight = 0;
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_RenderData.rendererMoveRight -= 1;
	}
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_RenderData.rendererMoveRight += 1;
	}

	m_RenderData.rendererMoveUp = 0;
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		m_RenderData.rendererMoveUp += 1;
	}
	if (glfwGetKey(m_RenderData.rendererWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		m_RenderData.rendererMoveUp -= 1;
	}
}