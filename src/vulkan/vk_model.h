#ifndef VK_MODEL_H
#define VK_MODEL_H

#include <vector>
#include <glm/glm.hpp>

#include "vk_render_data.h"

class VkModel
{
public:
	void init();

	VkMesh getVertexData();

private:
	VkMesh m_VertexData;
};

#endif