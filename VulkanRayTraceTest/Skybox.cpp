#include "Skybox.h"

Skybox::Skybox() : Mesh()
{
}

Skybox::Skybox(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass) : Mesh()
{
	const std::vector<Vertex> SkyBoxVertices =
	{
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},

		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},

		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},

		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},

		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},

		{{-1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}}
	};

	MeshID = engine.GenerateID();
	MaterialID = 0;

	MeshProperties = MeshPropertiesUniformBuffer(engine);
	ParentModelID = 0;
	VertexList = SkyBoxVertices;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = SkyBoxVertices.size();
	IndexCount = 0;
	PrimitiveCount = 0;

	MeshProperties.UniformDataInfo.MaterialIndex = assetManager.materialManager.GetMaterialBufferIDByMaterialID(MaterialID);
	MeshProperties.UniformDataInfo.ModelTransform = glm::mat4(1.0f);
	MeshProperties.Update(engine);

    VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
    TransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
    TransformInverseBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
}

Skybox::~Skybox()
{
}
