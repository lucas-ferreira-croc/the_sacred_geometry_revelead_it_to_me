#ifndef VK_RENDERER_H
#define VK_RENDERER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include "vk_render_data.h"
#include "render_pass.h"
#include "pipeline.h"
#include "vk_framebuffer.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "sync_objects.h"
#include "vk_texture.h"
#include "vk_uniform_buffer.h"
#include "vk_pipeline_layout.h"
#include "vk_user_interface.h"
#include "vk_model.h"
#include "vk_vertex_buffer.h"
#include "gltf_model.h"

#include "tools/timer.h"

#include "camera.h"

class VkRenderer 
{
public:
	VkRenderer(GLFWwindow* window);

	bool init(unsigned int width, unsigned int height);
	void setSize(unsigned int width, unsigned int height);
	bool draw();
	void cleanup();

	void handleKeyEvents(int key, int scancode, int action, int mods);
	void handleMouseButtonEvents(int button, int action, int mods);
	void handleMousePositionEvents(double xPos, double yPos);
	void handleMovementKeys();

private:
	VkRenderData m_RenderData{};
	VkGltfRenderData m_GltfRenderData{};

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	VkDeviceSize m_MinUniformBufferOssetAligment = 0;
	VkUploadMatrices m_Matrices;
	VKUserInterface m_UserInterface;

	Timer m_UIGeneratorTimer{};
	Timer m_FrameTimer{};
	Timer m_MatrixGenerateTimer{};
	Timer m_UploadToUBOTimer{};
	Timer m_UIDrawTimer{};

	Camera m_Camera{};
	bool m_MouseLock = false;
	int m_MouseXPos = 0;
	int m_MouseYPos = 0;

	double lastTickTime = 0.0f;

	bool deviceInit();
	bool getQueue();
	bool createDepthBuffer();
	bool createVBO();
	bool createUBO();
	bool createSwapchain();
	bool createRenderPass();
	bool createPipelineLayout();
	bool createPipelines();
	bool createFramebuffer();
	bool createCommandPool();
	bool createCommandBuffer();
	bool createSyncObjects();
	bool loadTexture();
	bool initVma();
	bool recreateSwapChain();

	bool loadGltfModel();
	bool createGltfPipelineLayout();
	bool createGltfPipeline();

	std::unique_ptr<VkModel> m_Model = nullptr;
	std::unique_ptr<VkMesh> m_AllMeshes = nullptr;

	std::shared_ptr<GltfModel> m_GltfModel = nullptr;
};

#endif
