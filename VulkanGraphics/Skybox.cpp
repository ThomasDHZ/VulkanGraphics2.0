#include "Skybox.h"

Skybox::Skybox() : Mesh()
{
}

Skybox::Skybox(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager) : Mesh()
{
	const std::vector<Vertex> SkyBoxVertices =
	{
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f,  1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

		{{-1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f, -1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{-1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
		{{ 1.0f, -1.0f,  1.0f }, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }}
	};

	 std::vector<uint32_t> IndexPlaceholder= { 0xFFFFFFFF };

	MeshID = engine->GenerateID();
	MeshMaterial = MaterialManagerPtr::GetMaterialManagerPtr()->GetDefaultMaterial();
	DrawFlags = MeshDrawFlags::Mesh_SkyBox;
	MeshType = MeshTypeFlag::Mesh_Type_SkyBox;

	MeshProperties = MeshPropertiesUniformBuffer(engine);
	ParentModelID = 0;
	VertexList = SkyBoxVertices;
            
	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = SkyBoxVertices.size();
	IndexCount = 0;
	PrimitiveCount = 0;

	MeshProperties.UniformDataInfo.ModelTransform = glm::mat4(1.0f);
	MeshProperties.Update();

    VertexBuffer.CreateBuffer(VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(IndexPlaceholder.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexPlaceholder.data());
	TransformBuffer.CreateBuffer(sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
    TransformInverseBuffer.CreateBuffer(sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
}

Skybox::~Skybox()
{
}
