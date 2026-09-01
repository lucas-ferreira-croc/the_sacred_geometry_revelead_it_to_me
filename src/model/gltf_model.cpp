#include "gltf_model.h"

#include "vk_vertex_buffer.h"
#include "vk_index_buffer.h"

#include "logger.h"


bool GltfModel::loadModel(VkRenderData& renderData, VkGltfRenderData& gltfRenderData, std::string modelFilename, std::string textureFilename)
{
	if(!VkRendererTexture::loadTexture(renderData, gltfRenderData.rendererGltfModelTexture, textureFilename))
	{
		Logger::log(1, "%s error: texture loading failed for texture '%s'\n", __FUNCTION__, textureFilename.c_str());
		return false;
	}
	Logger::log(1, "%s: gltf model texture '%s' successfully loaded %s\n", __FUNCTION__, textureFilename.c_str());

	m_Model = std::make_shared<tinygltf::Model>();
	
	tinygltf::TinyGLTF gltfLoader;
	std::string loadedErrors;
	std::string lodadedWarnings;
	bool result = false;

	result = gltfLoader.LoadASCIIFromFile(m_Model.get(), &loadedErrors, &lodadedWarnings, modelFilename);

	if(!lodadedWarnings.empty())
	{
		Logger::log(1, "%s: warnings found while loading gltf model:\n%s\n", __FUNCTION__, lodadedWarnings.c_str());
	}

	if (!loadedErrors.empty())
	{
		Logger::log(1, "%s: errors found while loading gltf model:\n%s\n", __FUNCTION__, loadedErrors.c_str());
	}

	if(!result)
	{
		Logger::log(1, "%s: error coult not load file '%s' \n", __FUNCTION__, modelFilename.c_str());
		return false;
	}

	createVertexBuffers(renderData, gltfRenderData);
	createIndexBuffers(renderData, gltfRenderData);

	renderData.gltfTriangleCount = getTriangleCount();
	return true;
}

void GltfModel::createVertexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	// TODO: must change to work with models with > 1 mesh
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	gltfRenderData.rendererGltfVertexBufferData.resize(primitives.attributes.size());

	for(const auto& attribute : primitives.attributes)
	{
		const std::string attributeType = attribute.first;
		const int accessorNumber = attribute.second;

		const tinygltf::Accessor& acessor = m_Model->accessors.at(accessorNumber);
		const tinygltf::BufferView& bufferView = m_Model->bufferViews.at(acessor.bufferView);
		const tinygltf::Buffer& buffer = m_Model->buffers.at(bufferView.buffer);

		if((attributeType.compare("POSITION") != 0) && (attributeType.compare("NORMAL") != 0) && (attributeType.compare("TEXCOORD_0") != 0))
		{
			Logger::log(1, "%s: skipping attribute type %s \n", __FUNCTION__, attributeType.c_str());
			continue;
		}

		VkVertexBuffer::init(renderData, gltfRenderData.rendererGltfVertexBufferData.at(attributes.at(attributeType)), bufferView.byteLength);
	}
}

void GltfModel::createIndexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	// TODO: must change to work with models with > 1 mesh
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitives.indices);
	const tinygltf::BufferView& indexBufferView = m_Model->bufferViews.at(indexAcessor.bufferView);
	const tinygltf::Buffer& buffer = m_Model->buffers.at(indexBufferView.buffer);

	VkIndexBuffer::init(renderData, gltfRenderData.rendererGltfIndexBufferData, indexBufferView.byteLength);
}

void GltfModel::uploadVertexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	// TODO: must change if going to use > 3 buffers
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);

	for(const auto& attribute : primitives.attributes)
	{
		const std::string attributeType = attribute.first;
		const int accessorNumber = attribute.second;

		if ((attributeType.compare("POSITION") != 0) && (attributeType.compare("NORMAL") != 0) && (attributeType.compare("TEXCOORD_0") != 0))
		{
			continue;

		}
		int targetBinding = attributes.at(attributeType);

		const tinygltf::Accessor& acessor = m_Model->accessors.at(accessorNumber);
		const tinygltf::BufferView& bufferView = m_Model->bufferViews.at(acessor.bufferView);
		const tinygltf::Buffer& buffer = m_Model->buffers.at(bufferView.buffer);
		VkVertexBuffer::uploadData(renderData, gltfRenderData.rendererGltfVertexBufferData.at(targetBinding), buffer, bufferView);
	}

	//for (int i = 0; i < 3; i++)
	//{
	//	const tinygltf::Accessor& acessor = m_Model->accessors.at(i);
	//	const tinygltf::BufferView& bufferView = m_Model->bufferViews.at(acessor.bufferView);
	//	const tinygltf::Buffer& buffer = m_Model->buffers.at(bufferView.buffer);

	//	VkVertexBuffer::uploadData(renderData, gltfRenderData.rendererGltfVertexBufferData.at(i), buffer, bufferView);
	//}
}

void GltfModel::uploadIndexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitives.indices);
	const tinygltf::BufferView& indexBufferView = m_Model->bufferViews.at(indexAcessor.bufferView);
	const tinygltf::Buffer& buffer = m_Model->buffers.at(indexBufferView.buffer);

	VkIndexBuffer::uploadData(renderData, gltfRenderData.rendererGltfIndexBufferData, buffer, indexBufferView);
}


void GltfModel::draw(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitives.indices);

	vkCmdBindDescriptorSets(renderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		renderData.rendererGltfPipelineLayout, 0, 1,
		&gltfRenderData.rendererGltfModelTexture.textureDescriptorSet, 0, nullptr);

	// TODO: must change if going to use > 3 buffers
	VkDeviceSize offset = 0;
	for(int i = 0; i < 3; i++)
	{
		vkCmdBindVertexBuffers(renderData.rendererCommandBuffer, i, 1, &gltfRenderData.rendererGltfVertexBufferData.at(i).rendererVertexBuffer, &offset);
	}

	VkIndexType indexType = VK_INDEX_TYPE_UINT32;
	if (indexAcessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
		indexType = VK_INDEX_TYPE_UINT8_EXT;
	}

	vkCmdBindIndexBuffer(renderData.rendererCommandBuffer, gltfRenderData.rendererGltfIndexBufferData.rendererIndexBuffer, 0, indexType);

	vkCmdBindPipeline(renderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderData.rendererGltfPipeline);

	vkCmdDrawIndexed(renderData.rendererCommandBuffer, static_cast<uint32_t>(renderData.gltfTriangleCount * 3), 1, 0, 0, 0);
}

void GltfModel::cleanup(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	// TODO: must change if going to use > 3 buffers
	for (int i = 0; i < 3; i++)
	{
		VkVertexBuffer::cleanup(renderData, gltfRenderData.rendererGltfVertexBufferData.at(i));;
	}

	VkIndexBuffer::cleanup(renderData, gltfRenderData.rendererGltfIndexBufferData);

	VkRendererTexture::cleanup(renderData, gltfRenderData.rendererGltfModelTexture);
	m_Model.reset();
}



int GltfModel::getTriangleCount()
{
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitives.indices);

	unsigned int triangles = 0;
	switch(primitives.mode)
	{
	case TINYGLTF_MODE_TRIANGLES:
		triangles = indexAcessor.count / 3;
		break;
	default:
		Logger::log(1, "%s error: unknown draw mode %i \n", __FUNCTION__, primitives.mode);
		break;
	}

	return triangles;
}
