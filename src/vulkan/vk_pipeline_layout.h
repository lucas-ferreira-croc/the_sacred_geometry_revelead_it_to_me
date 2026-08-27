#ifndef PIPELINE_LAYOUT_H
#define PIPELINE_LAYOUT_H

#include "vk_render_data.h"

class VkRendererPipelineLayout
{
public:
	static bool init(VkRenderData& renderData, VkPipelineLayout& pipelineLayout);
	static void cleanup(VkRenderData& renderData, VkPipelineLayout& pipelineLayout);
};

#endif
