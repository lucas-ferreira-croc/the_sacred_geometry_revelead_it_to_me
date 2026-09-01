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
	gltfRenderData.meshes.resize(m_Model->meshes.size());
	for(size_t m = 0; m < m_Model->meshes.size(); m++)
	{
		const tinygltf::Mesh& mesh = m_Model->meshes.at(m);
		gltfRenderData.meshes.at(m).primitives.resize(mesh.primitives.size());

		for(size_t p = 0; p < mesh.primitives.size(); p++)
		{
			const tinygltf::Primitive& primitives = mesh.primitives.at(p);
			gltfRenderData.meshes.at(m).primitives.at(p).rendererGltfVertexBufferData.resize(3);
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
				int targetBinding = attributes.at(attributeType);
				VkVertexBufferData& vertexBufferData = gltfRenderData.meshes.at(m).primitives.at(p).rendererGltfVertexBufferData.at(targetBinding);

				VkVertexBuffer::init(renderData, vertexBufferData, bufferView.byteLength);
			}
		}
	}

}

void GltfModel::createIndexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	// TODO: must change to work with models with > 1 mesh

	for (size_t m = 0; m < m_Model->meshes.size(); m++)
	{
		const tinygltf::Mesh& mesh = m_Model->meshes.at(m);
		for (size_t p = 0; p < mesh.primitives.size(); p++)
		{
			const tinygltf::Primitive& primitive = mesh.primitives.at(p);

			VkGltfPrimitiveData& primitiveData = gltfRenderData.meshes.at(m).primitives.at(p);
			const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitive.indices);
			const tinygltf::BufferView& indexBufferView = m_Model->bufferViews.at(indexAcessor.bufferView);
			VkIndexBuffer::init(renderData, primitiveData.rendererGltfIndexBufferData, indexBufferView.byteLength);

			primitiveData.indexCount = static_cast<uint32_t>(indexAcessor.count);
			primitiveData.indexType = VK_INDEX_TYPE_UINT32;
			if (indexAcessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
				primitiveData.indexType = VK_INDEX_TYPE_UINT8_EXT;
			}
			else if (indexAcessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
				primitiveData.indexType = VK_INDEX_TYPE_UINT16;
			}
		}
	}

	/*const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitives.indices);
	const tinygltf::BufferView& indexBufferView = m_Model->bufferViews.at(indexAcessor.bufferView);
	const tinygltf::Buffer& buffer = m_Model->buffers.at(indexBufferView.buffer);

	VkIndexBuffer::init(renderData, gltfRenderData.rendererGltfIndexBufferData, indexBufferView.byteLength);*/
}

void GltfModel::uploadVertexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	// TODO: must change if going to use > 3 buffers
	
	for (size_t m = 0; m < m_Model->meshes.size(); m++)
	{
		const tinygltf::Mesh& mesh = m_Model->meshes.at(m);

		for (size_t p = 0; p < mesh.primitives.size(); p++)
		{
			const tinygltf::Primitive& primitive = mesh.primitives.at(p);
			for (const auto& attribute : primitive.attributes)
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

				VkVertexBufferData vertexBufferdata = gltfRenderData.meshes.at(m).primitives.at(p).rendererGltfVertexBufferData.at(targetBinding);

				VkVertexBuffer::uploadData(renderData, vertexBufferdata, buffer, bufferView);
			}
		}
	}
}

void GltfModel::uploadIndexBuffers(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	for (size_t m = 0; m < m_Model->meshes.size(); m++)
	{
		const tinygltf::Mesh& mesh = m_Model->meshes.at(m);

		for (size_t p = 0; p < mesh.primitives.size(); p++)
		{
			const tinygltf::Primitive& primitive = mesh.primitives.at(p);

			const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitive.indices);
			const tinygltf::BufferView& indexBufferView = m_Model->bufferViews.at(indexAcessor.bufferView);
			const tinygltf::Buffer& buffer = m_Model->buffers.at(indexBufferView.buffer);

			VkIndexBufferData& indexBufferData = gltfRenderData.meshes.at(m).primitives.at(p).rendererGltfIndexBufferData;
			VkIndexBuffer::uploadData(renderData, indexBufferData, buffer, indexBufferView);
		}
	}
}


void GltfModel::draw(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	const tinygltf::Primitive& primitives = m_Model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAcessor = m_Model->accessors.at(primitives.indices);

	vkCmdBindDescriptorSets(renderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		renderData.rendererGltfPipelineLayout, 0, 1,
		&gltfRenderData.rendererGltfModelTexture.textureDescriptorSet, 0, nullptr);

	vkCmdBindPipeline(renderData.rendererCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderData.rendererGltfPipeline);

	for(size_t m = 0; m < gltfRenderData.meshes.size(); m++)
	{
		const VkGltfMesh& meshData = gltfRenderData.meshes.at(m);

		for(size_t p = 0; p < meshData.primitives.size(); p++)
		{
			const VkGltfPrimitiveData& primitiveData = gltfRenderData.meshes.at(m).primitives.at(p);

			VkDeviceSize offset = 0;
			for(size_t i = 0; i < primitiveData.rendererGltfVertexBufferData.size(); i++)
			{
				vkCmdBindVertexBuffers(
					renderData.rendererCommandBuffer,
					static_cast<uint32_t>(i),
					1,
					&primitiveData.rendererGltfVertexBufferData.at(i).rendererVertexBuffer,
					&offset
				);

			}
			vkCmdBindIndexBuffer(
				renderData.rendererCommandBuffer, 
				primitiveData.rendererGltfIndexBufferData.rendererIndexBuffer,
				0,
				primitiveData.indexType
			);

			vkCmdDrawIndexed(
				renderData.rendererCommandBuffer,
				static_cast<uint32_t>(primitiveData.indexCount), 1, 0, 0, 0);

		}
	}
}

void GltfModel::cleanup(VkRenderData& renderData, VkGltfRenderData& gltfRenderData)
{
	for (auto& mesh : gltfRenderData.meshes)
	{
		for (auto& primitive : mesh.primitives)
		{
			for (int i = 0; i < primitive.rendererGltfVertexBufferData.size(); i++)
			{
				VkVertexBuffer::cleanup(renderData, primitive.rendererGltfVertexBufferData.at(i));
			}
			VkIndexBuffer::cleanup(renderData, primitive.rendererGltfIndexBufferData);
		}
	}

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
