#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vk_renderer.h"

class Window
{
public:
	bool init(unsigned int width, unsigned int height, std::string title);

	void mainLoop();
	void cleanup();


	GLFWwindow* get() { return m_Window; };

private:
	void handleWindowCloseEvents();
	void handleKeyEvents(int key, int scancode, int action, int mods);
private:
	GLFWwindow* m_Window = nullptr;
	std::unique_ptr<VkRenderer> m_VkRenderer;
};


#endif