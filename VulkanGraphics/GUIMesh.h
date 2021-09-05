#pragma once
#include "Vertex.h"
#include "VulkanBuffer.h"
#include "AccelerationStructure.h"
#include "Material.h"
#include "InputManager.h"

struct GUIVertex
{
	glm::vec2 Position = glm::vec3(0.0f);
	glm::vec2 TexureCoord = glm::vec2(0.0f);

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription BindingDescription = {};
		BindingDescription.binding = 0;
		BindingDescription.stride = sizeof(GUIVertex);
		BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return BindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> AttributeDescriptions = {};

		VkVertexInputAttributeDescription AttributeDescription;

		AttributeDescription.binding = 0;
		AttributeDescription.location = 0;
		AttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
		AttributeDescription.offset = offsetof(GUIVertex, Position);
		AttributeDescriptions.emplace_back(AttributeDescription);

		AttributeDescription.binding = 0;
		AttributeDescription.location = 1;
		AttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
		AttributeDescription.offset = offsetof(GUIVertex, TexureCoord);
		AttributeDescriptions.emplace_back(AttributeDescription);

		return AttributeDescriptions;
	}
};

struct GUIMeshProperties
{
	alignas(4) uint32_t MaterialID = 0;
	alignas(16) glm::vec4 Color = glm::vec4(1.0f);
	alignas(8) glm::vec2 Position = glm::vec2(0.0f);
	alignas(8) glm::vec2 Scale = glm::vec2(1.0f);
	alignas(8) glm::vec2 UVOffset = glm::vec2(0.0f);
};

class GUIMesh
{
private:

protected:
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;
	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;

	GUIMesh* ParentMesh;
	std::vector<std::shared_ptr<GUIMesh>> ChildrenMeshList;
	std::shared_ptr<Material> material;

	glm::vec2 ObjectPosition = glm::vec2(0.0f);

public:
	glm::vec2 MeshPosition = glm::vec2(0.0f);
	glm::vec2 Scale = glm::vec2(1.0f);
	glm::vec4 Color = glm::vec4(1.0f);
	glm::vec2 UVOffset = glm::vec2(0.0f);

	GUIMesh();
	GUIMesh(std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList);
	GUIMesh(std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList, glm::vec2 Position);
	GUIMesh(std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> meshmaterial);
	GUIMesh(std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> meshmaterial, glm::vec2 Position);
	~GUIMesh();

	virtual void Update(glm::vec2 objectPosition);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	virtual void Destory();
	void AddChildMesh(std::shared_ptr<GUIMesh> mesh);
};

