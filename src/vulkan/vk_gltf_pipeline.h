#ifndef GLTF_PIPELINE_H
#define GLTF_PIPELINE_H

#include <string>
#include <vulkan/vulkan.h>

#include "vk_render_data.h"

class GltfPipeline
{
public:
	static bool init(VkRenderData& renderData,
		VkPipelineLayout& pipelineLayout, VkPipeline& pipeline,
		VkPrimitiveTopology topology,
		std::string vertexShaderFilename, std::string fragmentShaderFilename);

	static void cleanup(VkRenderData& renderData, VkPipeline& pipeline);
};

#endif