#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>
#include "PerspectiveCamera.h"
#include "VulkanBuffer.h"
#include "TextureManager.h"

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

struct RTVertex
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Normal = glm::vec3(0.0f);
	glm::vec2 TexureCoord = glm::vec2(0.0f);
	glm::vec4 Tangant = glm::vec4(0.0f);
	glm::vec4 BiTangant = glm::vec4(0.0f);
	glm::vec4 Color = glm::vec4(0.0f);
	glm::ivec4 BoneID = glm::ivec4(0);
	glm::vec4 BoneWeights = glm::vec4(0.0f);
};

struct MeshDetails
{
	std::vector<RTVertex> vertices;
	std::vector<uint32_t> indices;
};

struct MeshOffsets
{
	uint32_t VertexOffset;
	uint32_t IndiceOffset;
};

struct Mesh
{
	std::vector<RTVertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 Transform;
	Material material;

	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;
	VulkanBuffer TransformBuffer;
	VulkanBuffer MaterialBuffer;

	uint32_t TriangleCount;
	uint32_t VertexCount;
	uint32_t IndexCount;

	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress{};
};

class RayTraceModel
{
private:
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;

	void LoadMesh(TextureManager& textureManager, VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<RTVertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	Material LoadMaterial(TextureManager& textureManager, VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& FilePath, aiMesh* mesh, const aiScene* scene);
	uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);

public:
	glm::vec3 ModelPosition = glm::vec3(0.0f);
	glm::vec3 ModelRotation = glm::vec3(0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	std::vector<Mesh> MeshList;
	std::vector<MeshOffsets> MeshOffsetList;
	std::vector<VulkanBuffer> MeshOffsetBufferList;

	std::vector<RTVertex> ModelVertices;
	std::vector<uint32_t> ModelIndices;
	glm::mat4 ModelTransform;

	VulkanBuffer ModelIndexBuffer;
	VulkanBuffer ModelVertexBuffer;
	VulkanBuffer ModelTransformBuffer;
	VulkanBuffer ModelUniformBuffer;

	uint32_t ModelTriangleCount;
	uint32_t ModelVertexCount;
	uint32_t ModelIndexCount;

	VkDeviceOrHostAddressConstKHR ModelVertexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR ModelIndexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR ModelTransformBufferDeviceAddress{};

	RayTraceModel();
	RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshDetails& meshDetails);
	RayTraceModel(TextureManager& textureManager, VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& FilePath);
	~RayTraceModel();

	void Update();
	void Destory(VkDevice& device);
};

