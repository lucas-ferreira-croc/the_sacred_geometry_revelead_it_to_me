#include "window.h"
#include "logger.h"

#include <vector>

bool Window::init(unsigned int width, unsigned int height, std::string title)
{
	if(!glfwInit())
	{
		Logger::log(1, "%s: glfwUnit() error\n", __FUNCTION__);
		return false;
	}

	if (!glfwVulkanSupported())
	{
		Logger::log(1, "%s: Vulkan is not supported\n", __FUNCTION__);
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (!m_Window)
	{
		Logger::log(1, "%s: Could not create window\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}

	m_VkRenderer = std::make_unique<VkRenderer>(m_Window);
	if(!m_VkRenderer->init(width, height))
	{
		glfwTerminate();
		Logger::log(1, "%s error: could not init vulkan", __FUNCTION__);
		return false;
	}

	glfwSetWindowUserPointer(m_Window, m_VkRenderer.get());
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
	{
		auto renderer = static_cast<VkRenderer*>(glfwGetWindowUserPointer(window));
		renderer->setSize(width, height);
	});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto renderer = static_cast<VkRenderer*>(glfwGetWindowUserPointer(window));
			renderer->handleKeyEvents(key, scancode, action, mods);
		});


	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto renderer = static_cast<VkRenderer*>(glfwGetWindowUserPointer(window));
			renderer->handleMouseButtonEvents(button, action, mods);
		});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			auto renderer = static_cast<VkRenderer*>(glfwGetWindowUserPointer(window));
			renderer->handleMousePositionEvents(xpos, ypos);
		});


	Logger::log(1, "%s: mockup model data loaded", __FUNCTION__);
	Logger::log(1, "%s: Window with vulkan successfully initialized", __FUNCTION__);
	return true;
	
	return true;
}



void Window::mainLoop()
{
	while (!glfwWindowShouldClose(m_Window))
	{
		if(!m_VkRenderer->draw())
		{
			break;
		}
		glfwPollEvents();
	}
}

void Window::cleanup()
{
	m_VkRenderer->cleanup();
	Logger::log(1, "%s: Terminating window\n", __FUNCTION__);
	
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::handleWindowCloseEvents()
{
	Logger::log(1, "%s: Window got close event...tchau!!\n", __FUNCTION__);
}

void Window::handleKeyEvents(int key, int scancode, int action, int
	mods) {
	std::string actionName;
	switch (action) {
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	case GLFW_REPEAT:
		actionName = "repeated";
		break;
	default:
		actionName = "invalid";
		break;
	}
	const char* keyName = glfwGetKeyName(key, 0);
	Logger::log(1, "%s: key %s (key %i, scancode %i) %s\n", __FUNCTION__, keyName, key, scancode, actionName.c_str());
}

