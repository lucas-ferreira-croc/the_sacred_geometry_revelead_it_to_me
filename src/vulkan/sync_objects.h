#ifndef SYNC_OBJECTS_H
#define SYNC_OBJECTS_H

#include <vulkan/vulkan.h>

#include <vk_render_data.h>

class SyncObjects
{
public:
	static bool init(VkRenderData& renderData);
	static void cleanup(VkRenderData& renderData);
};

#endif