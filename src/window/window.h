#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vk_renderer.h"
#include "vk_model.h"

class Window
{
public:
	bool init(unsigned int width, unsigned int height, std::string title);

	void mainLoop();
	void cleanup();

private:
	void handleWindowCloseEvents();
	void handleKeyEvents(int key, int scancode, int action, int mods);
private:
	GLFWwindow* m_Window = nullptr;
	std::string m_ApplicationName;
	bool m_Vulkan;

	int m_Width;
	int m_Height;

	VkInstance m_Instance{};
	VkSurfaceKHR m_Surface{};

	std::unique_ptr<VkRenderer> m_VkRenderer;
	std::unique_ptr<VkModel> m_VkModel;
};


#endif