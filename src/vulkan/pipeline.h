#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vulkan/vulkan.h>

#include "vk_render_data.h"

class VkRendererPipeline 
{
public:
	static bool init(VkRenderData& renderData, VkPipelineLayout& pipelineLayout, VkPipeline& pipeline,
		std::string vertexShaderFilename, std::string fragmentShaderFilename);
	static void cleanup(VkRenderData& renderData, VkPipeline& pipeline);
};

#endif