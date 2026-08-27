#ifndef VK_SHADER_H
#define VK_SHADER_H

#include <string>
#include <vulkan/vulkan.h>

class VkRendererShader
{
public:
	static VkShaderModule loadShader(VkDevice device, std::string filename);
private:
	static std::string loadFileToString(std::string filename);
};

#endif