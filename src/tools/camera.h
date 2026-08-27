#ifndef  CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "vk_render_data.h"

class Camera
{
public:
	glm::mat4 getViewMatrix(VkRenderData& renderData);

private:
	glm::vec3 m_ViewDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_WorldUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_RightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_UpDirection = glm::vec3(0.0f, 0.0f, 0.0f);
};

#endif // ! CAMERA_H
