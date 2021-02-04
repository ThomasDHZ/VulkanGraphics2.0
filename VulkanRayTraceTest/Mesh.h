#pragma once
#include "VulkanEngine.h"
#include "VulkanBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"
#include "Texture.h"
#include "ForwardRenderingPipeline.h"

struct Material
{
	alignas(16) glm::vec3 Ambient = glm::vec3(0.2f);
	alignas(16) glm::vec3 Diffuse = glm::vec3(0.6f);
	alignas(16) glm::vec3 Specular = glm::vec3(1.0f);
	alignas(4) float Shininess = 32;
	alignas(4) float Reflectivness = 0;
	alignas(4) float Dissolveness = 0.5f;

	alignas(4) uint32_t DiffuseMapID = 0;
	alignas(4) uint32_t SpecularMapID = 0;
	alignas(4) uint32_t NormalMapID = 0;
	alignas(4) uint32_t DepthMapID = 0;
	alignas(4) uint32_t AlphaMapID = 0;
	alignas(4) uint32_t EmissionMapID = 0;
};

struct MeshDetails
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct MeshOffsets
{
	uint32_t VertexOffset;
	uint32_t IndiceOffset;
};

class Mesh
{
private:
	VkDescriptorPool DescriptorPool;
	std::vector<VkDescriptorSet> DescriptorSets;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine, GraphicsPipeline& pipeline, std::shared_ptr<Texture> texture);



public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 Transform;
	Material material;

	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;
	VulkanBuffer UniformBufferList;
	VulkanBuffer TransformBuffer;
	VulkanBuffer MaterialBuffer;

	uint32_t MeshID = 0;
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;
	uint32_t VertexOffset = 0;
	uint32_t FirstIndex = 0;

	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress{};

	Mesh();
	Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList);
	Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList);
	Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, Material MeshMaterial);
	Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList, Material MeshMaterial);
	~Mesh();

	VkDescriptorPoolSize AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType);
	VkDescriptorImageInfo AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture);
	VkDescriptorBufferInfo AddBufferDescriptorInfo(VulkanEngine& engine, VulkanBuffer buffer);
	VkWriteDescriptorSet AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
	VkWriteDescriptorSet AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
};
