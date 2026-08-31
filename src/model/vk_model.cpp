#include "vk_model.h"
#include "logger.h"

void VkModel::init()
{
	m_VertexData.vertices.resize(36);

	/* front */
	m_VertexData.vertices[0].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[1].position = glm::vec3(0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[2].position = glm::vec3(-0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[3].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[4].position = glm::vec3(0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[5].position = glm::vec3(0.5f, 0.5f, 0.5f);

	m_VertexData.vertices[0].color = glm::vec3(1.0f, 0.5f, 0.5f);
	m_VertexData.vertices[1].color = glm::vec3(1.0f, 0.5f, 0.5f);
	m_VertexData.vertices[2].color = glm::vec3(1.0f, 0.5f, 0.5f);
	m_VertexData.vertices[3].color = glm::vec3(1.0f, 0.5f, 0.5f);
	m_VertexData.vertices[4].color = glm::vec3(1.0f, 0.5f, 0.5f);
	m_VertexData.vertices[5].color = glm::vec3(1.0f, 0.5f, 0.5f);

	m_VertexData.vertices[0].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[1].uv = glm::vec2(1.0, 0.0);
	m_VertexData.vertices[2].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[3].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[4].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[5].uv = glm::vec2(1.0, 0.0);

	/* back */
	m_VertexData.vertices[6].position = glm::vec3(-0.5f, -0.5f, -0.5f);
	m_VertexData.vertices[7].position = glm::vec3(-0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[8].position = glm::vec3(0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[9].position = glm::vec3(-0.5f, -0.5f, -0.5f);
	m_VertexData.vertices[10].position = glm::vec3(0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[11].position = glm::vec3(0.5f, -0.5f, -0.5f);

	m_VertexData.vertices[6].color = glm::vec3(0.5f, 1.0f, 0.5f);
	m_VertexData.vertices[7].color = glm::vec3(0.5f, 1.0f, 0.5f);
	m_VertexData.vertices[8].color = glm::vec3(0.5f, 1.0f, 0.5f);
	m_VertexData.vertices[9].color = glm::vec3(0.5f, 1.0f, 0.5f);
	m_VertexData.vertices[10].color = glm::vec3(0.5f, 1.0f, 0.5f);
	m_VertexData.vertices[11].color = glm::vec3(0.5f, 1.0f, 0.5f);

	m_VertexData.vertices[6].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[7].uv = glm::vec2(1.0, 0.0);
	m_VertexData.vertices[8].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[9].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[10].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[11].uv = glm::vec2(0.0, 1.0);

	/* left */
	m_VertexData.vertices[12].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[13].position = glm::vec3(-0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[14].position = glm::vec3(-0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[15].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[16].position = glm::vec3(-0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[17].position = glm::vec3(-0.5f, -0.5f, -0.5f);

	m_VertexData.vertices[12].color = glm::vec3(0.5f, 0.5f, 1.0f);
	m_VertexData.vertices[13].color = glm::vec3(0.5f, 0.5f, 1.0f);
	m_VertexData.vertices[14].color = glm::vec3(0.5f, 0.5f, 1.0f);
	m_VertexData.vertices[15].color = glm::vec3(0.5f, 0.5f, 1.0f);
	m_VertexData.vertices[16].color = glm::vec3(0.5f, 0.5f, 1.0f);
	m_VertexData.vertices[17].color = glm::vec3(0.5f, 0.5f, 1.0f);

	m_VertexData.vertices[12].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[13].uv = glm::vec2(1.0, 0.0);
	m_VertexData.vertices[14].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[15].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[16].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[17].uv = glm::vec2(0.0, 1.0);

	/* right */
	m_VertexData.vertices[18].position = glm::vec3(0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[19].position = glm::vec3(0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[20].position = glm::vec3(0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[21].position = glm::vec3(0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[22].position = glm::vec3(0.5f, -0.5f, -0.5f);
	m_VertexData.vertices[23].position = glm::vec3(0.5f, 0.5f, -0.5f);

	m_VertexData.vertices[18].color = glm::vec3(0.0f, 0.5f, 0.5f);
	m_VertexData.vertices[19].color = glm::vec3(0.0f, 0.5f, 0.5f);
	m_VertexData.vertices[20].color = glm::vec3(0.0f, 0.5f, 0.5f);
	m_VertexData.vertices[21].color = glm::vec3(0.0f, 0.5f, 0.5f);
	m_VertexData.vertices[22].color = glm::vec3(0.0f, 0.5f, 0.5f);
	m_VertexData.vertices[23].color = glm::vec3(0.0f, 0.5f, 0.5f);

	m_VertexData.vertices[18].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[19].uv = glm::vec2(1.0, 0.0);
	m_VertexData.vertices[20].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[21].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[22].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[23].uv = glm::vec2(1.0, 0.0);

	/* top */
	m_VertexData.vertices[24].position = glm::vec3(0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[25].position = glm::vec3(-0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[26].position = glm::vec3(-0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[27].position = glm::vec3(0.5f, 0.5f, 0.5f);
	m_VertexData.vertices[28].position = glm::vec3(0.5f, 0.5f, -0.5f);
	m_VertexData.vertices[29].position = glm::vec3(-0.5f, 0.5f, -0.5f);

	m_VertexData.vertices[24].color = glm::vec3(0.5f, 0.0f, 0.5f);
	m_VertexData.vertices[25].color = glm::vec3(0.5f, 0.0f, 0.5f);
	m_VertexData.vertices[26].color = glm::vec3(0.5f, 0.0f, 0.5f);
	m_VertexData.vertices[27].color = glm::vec3(0.5f, 0.0f, 0.5f);
	m_VertexData.vertices[28].color = glm::vec3(0.5f, 0.0f, 0.5f);
	m_VertexData.vertices[29].color = glm::vec3(0.5f, 0.0f, 0.5f);

	m_VertexData.vertices[24].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[25].uv = glm::vec2(1.0, 0.0);
	m_VertexData.vertices[26].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[27].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[28].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[29].uv = glm::vec2(1.0, 0.0);

	/* bottom */
	m_VertexData.vertices[30].position = glm::vec3(0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[31].position = glm::vec3(-0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[32].position = glm::vec3(-0.5f, -0.5f, -0.5f);
	m_VertexData.vertices[33].position = glm::vec3(0.5f, -0.5f, 0.5f);
	m_VertexData.vertices[34].position = glm::vec3(-0.5f, -0.5f, -0.5f);
	m_VertexData.vertices[35].position = glm::vec3(0.5f, -0.5f, -0.5f);

	m_VertexData.vertices[30].color = glm::vec3(0.5f, 0.5f, 0.0f);
	m_VertexData.vertices[31].color = glm::vec3(0.5f, 0.5f, 0.0f);
	m_VertexData.vertices[32].color = glm::vec3(0.5f, 0.5f, 0.0f);
	m_VertexData.vertices[33].color = glm::vec3(0.5f, 0.5f, 0.0f);
	m_VertexData.vertices[34].color = glm::vec3(0.5f, 0.5f, 0.0f);
	m_VertexData.vertices[35].color = glm::vec3(0.5f, 0.5f, 0.0f);

	m_VertexData.vertices[30].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[31].uv = glm::vec2(0.0, 1.0);
	m_VertexData.vertices[32].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[33].uv = glm::vec2(0.0, 0.0);
	m_VertexData.vertices[34].uv = glm::vec2(1.0, 1.0);
	m_VertexData.vertices[35].uv = glm::vec2(1.0, 0.0);
	Logger::log(1, "%s: loaded %d vertices\n", __FUNCTION__, m_VertexData.vertices.size());
}

VkMesh VkModel::getVertexData() 
{
	if(m_VertexData.vertices.size() == 0)
	{
		init();
	}
	return m_VertexData;
}