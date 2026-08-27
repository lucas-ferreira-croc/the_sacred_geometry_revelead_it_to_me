#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix(VkRenderData& renderData)
{
	float yawRad = glm::radians(renderData.rendererViewYaw);
	float pitchRad = glm::radians(renderData.rendererViewPitch);

	float sinYaw = glm::sin(yawRad);
	float cosYaw = glm::cos(yawRad);
	float sinPitch = glm::sin(pitchRad);
	float cosPitch = glm::cos(pitchRad);

	m_ViewDirection = glm::normalize(glm::vec3(sinYaw * cosPitch, -sinPitch, -cosYaw * cosPitch));

	m_RightDirection = glm::normalize(glm::cross(m_ViewDirection, m_WorldUpVector));
	m_UpDirection = glm::normalize(glm::cross(m_RightDirection, m_ViewDirection));

	renderData.m_RendererCameraWorldPos += renderData.rendererMoveForward * renderData.rendererTickDiff * m_ViewDirection +
										   renderData.rendererMoveRight * renderData.rendererTickDiff * m_RightDirection +
										   renderData.rendererMoveUp * renderData.rendererTickDiff * m_UpDirection;

	return glm::lookAt(renderData.m_RendererCameraWorldPos, renderData.m_RendererCameraWorldPos + m_ViewDirection, m_WorldUpVector);
}