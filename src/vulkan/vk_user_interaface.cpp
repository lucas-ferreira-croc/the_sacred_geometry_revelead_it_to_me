#include "vk_user_interface.h"

#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "command_buffer.h"
#include "logger.h"


bool VKUserInterface::init(VkRenderData& renderData)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    VkDescriptorPoolSize imguiPoolSizes[] =
    {
      { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo imguiPoolInfo{};
    imguiPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    imguiPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    imguiPoolInfo.maxSets = 1000;
    imguiPoolInfo.poolSizeCount = std::size(imguiPoolSizes);
    imguiPoolInfo.pPoolSizes = imguiPoolSizes;

    if (vkCreateDescriptorPool(renderData.rendererVkbDevice.device, &imguiPoolInfo, nullptr, &renderData.rendererImguiDescriptorPool)) {
        Logger::log(1, "%s error: could not init ImGui descriptor pool \n", __FUNCTION__);
        return false;
    }

    ImGui_ImplGlfw_InitForVulkan(renderData.rendererWindow, true);

    ImGui_ImplVulkan_InitInfo imguiIinitInfo{};
    imguiIinitInfo.Instance = renderData.rendererVkbInstance.instance;
    imguiIinitInfo.PhysicalDevice = renderData.rendererVkbDevice.physical_device;
    imguiIinitInfo.Device = renderData.rendererVkbDevice.device;
    imguiIinitInfo.Queue = renderData.rendererGraphicsQueue;
    imguiIinitInfo.DescriptorPool = renderData.rendererImguiDescriptorPool;
    imguiIinitInfo.MinImageCount = 2;
    imguiIinitInfo.ImageCount = renderData.rendererSwapchainImages.size();
    imguiIinitInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    imguiIinitInfo.PipelineInfoMain.RenderPass = renderData.rendererRenderpass;

    if (!ImGui_ImplVulkan_Init(&imguiIinitInfo)) {
        Logger::log(1, "%s error: could not init ImGui for Vulkan \n", __FUNCTION__);
        return false;
    }

    ImGui::StyleColorsDark();

    return true;
}

void VKUserInterface::createFrame(VkRenderData& renderData)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags imguiWindowFlags = 0;
    imguiWindowFlags |= ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGui::Begin("Control", nullptr, imguiWindowFlags);

	static float newFps = 0.0f;
	if (renderData.rendererFrameTime > 0.0f)
	{
		newFps = 1.0f / renderData.rendererFrameTime * 1000.0f;
	}
	m_FramesPerSecond = (m_AveragingAlpha * m_FramesPerSecond) + (1.0f - m_AveragingAlpha) * newFps;

	ImGui::Text("FPS:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(m_FramesPerSecond).c_str());
	ImGui::Separator();

	ImGui::Text("Frame Time:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.rendererFrameTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");

	ImGui::Text("Matrix Generation Time:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.matrixGenerateTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");

	ImGui::Text("Matrix Upload Time:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.uplaodToUBOTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");

	ImGui::Text("UI Generation Time:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.matrixGenerateTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");

	ImGui::Text("UI Draw Time:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.uiDrawTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");


	ImGui::Separator();

	ImGui::Text("Camera position:");
	ImGui::SameLine();
	ImGui::Text("%s:", glm::to_string(renderData.m_RendererCameraWorldPos).c_str());

	ImGui::Text("View yaw:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.rendererViewYaw).c_str());

	ImGui::Text("View pitch:");
	ImGui::SameLine();
	ImGui::Text("%s", std::to_string(renderData.rendererViewPitch).c_str());

	ImGui::Separator();

	ImGui::Text("Triangles:");
	ImGui::SameLine();

	ImGui::Text(std::to_string(renderData.rendererTringleCount).c_str());
	std::string windowDimensions = std::to_string(renderData.rendererWidth) + "x" + std::to_string(renderData.rendererHeight);
	ImGui::Text("Window Dimensions:");
	ImGui::SameLine();
	ImGui::Text(windowDimensions.c_str());

	std::string imguiWindowPos = std::to_string(static_cast<int>(ImGui::GetWindowPos().x)) + "/" + std::to_string(static_cast<int>(ImGui::GetWindowPos().y));
	ImGui::Text("ImGui Window Position:");
	ImGui::SameLine();
	ImGui::Text(imguiWindowPos.c_str());
	ImGui::Separator();

	static bool checkBoxChecked = false;
	ImGui::Checkbox("Check me", &checkBoxChecked);

	if (checkBoxChecked)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Yes");
		ImGui::PopStyleColor();
	}
	ImGui::Separator();

	if (ImGui::Button("Toggle Shader"))
	{
		renderData.rendererUseChangedShader = !renderData.rendererUseChangedShader;
	}
	ImGui::SameLine();
	if (!renderData.rendererUseChangedShader)
	{
		ImGui::Text("Basic shader");
	}
	else
	{
		ImGui::Text("Changed shader");
	}
	ImGui::Separator();

	ImGui::Text("Field of view");
	ImGui::SameLine();
	ImGui::SliderInt("##FOV", &renderData.rendererFieldOfView, 40, 150);
    ImGui::End();
}

void VKUserInterface::render(VkRenderData& renderData)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderData.rendererCommandBuffer);
}

void VKUserInterface::cleanup(VkRenderData& renderData)
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	vkDestroyDescriptorPool(renderData.rendererVkbDevice.device, renderData.rendererImguiDescriptorPool, nullptr);
	ImGui::DestroyContext();
}