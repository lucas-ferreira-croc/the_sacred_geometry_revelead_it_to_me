#ifndef GLTF_MODEL_H
#define GLTF_MODEL_H

#include <string>
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <tiny_gltf.h>

#include "vk_texture.h"
#include "vk_render_data.h"

class GltfModel
{
public:
	bool loadModel(VkRenderData& renderData, VkGltfRenderData& gltfRenderData,
		std::string modelFilename, std::string textureFilename);

	void draw(VkRenderData& renderData, VkGltfRenderData& gltfRenderData);
	void cleanup(VkRenderData& renderData, VkGltfRenderData& gltfRenderData);

	void uploadVertexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData);
	void uploadIndexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData);

private:
	void createVertexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData);
	void createIndexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData);

	int getTriangleCount();

	std::shared_ptr<tinygltf::Model> m_Model = nullptr;

	std::map<std::string, int> attributes = {
		{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}
	};

};


#endif