#ifndef VK_USER_INTERFACE_H
#define VK_USER_INTERFACE_H


#include "vk_render_data.h"

class VKUserInterface
{
public:
	bool init(VkRenderData& renderData);
	void createFrame(VkRenderData& renderData);
	void render(VkRenderData& renderData);
	void cleanup(VkRenderData& renderData);
private:
	float m_FramesPerSecond = 0.0f;
	float m_AveragingAlpha = 0.96f;
};

#endif