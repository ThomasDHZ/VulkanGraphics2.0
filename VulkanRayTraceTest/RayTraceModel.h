#pragma once
#include "RayTraceMesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>

struct Mesh
{
	std::vector<RTVertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 Transform;

	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;
	VulkanBuffer TransformBuffer;

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

	void LoadMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<RTVertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);

public:
	std::vector<Mesh> MeshList;

	RayTraceModel();
	RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath);
	~RayTraceModel();
};

