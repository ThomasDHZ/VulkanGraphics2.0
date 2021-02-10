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

	alignas(4) uint32_t DiffuseMapID = 0;
	alignas(4) uint32_t SpecularMapID = 0;
	alignas(4) uint32_t NormalMapID = 0;
	alignas(4) uint32_t DepthMapID = 0;
	alignas(4) uint32_t AlphaMapID = 0;
	alignas(4) uint32_t EmissionMapID = 0;
};

struct DirectionalLight {
	alignas(16) glm::vec3 direction;

	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
};

struct PointLight {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float constant = 1.0f;
	alignas(4) float linear = 0.09f;
	alignas(4) float quadratic = 0.032f;
};

struct SceneDataBufferData {
	alignas(16) glm::mat4 viewInverse;
	alignas(16) glm::mat4 projInverse;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::mat4 model;
	DirectionalLight dlight;
	alignas(16) glm::vec3 viewPos;
	PointLight plight;
	alignas(4) int vertexSize;
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
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;

public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		glm::mat4 Transform;
		Material material;
	
		VulkanBuffer IndexBuffer;
		VulkanBuffer VertexBuffer;
		VulkanBuffer UniformBuffer;
		VulkanBuffer TransformBuffer;
		VulkanBuffer MaterialBuffer;
	
		uint32_t TriangleCount;
		uint32_t VertexCount;
		uint32_t IndexCount;
	
		VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress{};

	Mesh();
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material MeshMaterial);
	~Mesh();

	//void SetUpMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);

	//VkDescriptorPoolSize AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType);
	//VkDescriptorImageInfo AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture);
	//VkDescriptorBufferInfo AddBufferDescriptorInfo(VulkanEngine& engine, VulkanBuffer buffer);
	//VkWriteDescriptorSet AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
	//VkWriteDescriptorSet AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo> TextureImageList);

	void Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int index);
	void Destory(VulkanEngine& engine);
};
