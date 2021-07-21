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

struct GUIProperties
{
	alignas(4) uint32_t MaterialID = 0;
	alignas(16) glm::vec3 Color = glm::vec3(1.0f);
	alignas(16) glm::vec3 Translation = glm::vec3(1.0f);
	alignas(16) glm::vec3 Scale = glm::vec3(1.0f);
};

class GUIMesh
{
private:

protected:
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;
	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;

	std::shared_ptr<Material> material;

public:
	glm::vec3 Color = glm::vec3(1.0f);
	glm::vec3 Translation = glm::vec3(1.0f);
	glm::vec3 Scale = glm::vec3(1.0f);

	GUIMesh();
	GUIMesh(VulkanEngine& engine, std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList);
	GUIMesh(VulkanEngine& engine, std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> meshmaterial);
	~GUIMesh();

	virtual void Update(VulkanEngine& engine, InputManager& inputManager);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	virtual void Destory(VulkanEngine& engine);
};

