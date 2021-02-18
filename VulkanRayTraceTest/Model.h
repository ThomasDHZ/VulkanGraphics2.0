#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>
#include "PerspectiveCamera.h"
#include "VulkanBuffer.h"
#include "TextureManager.h"
#include "Vertex.h"
#include "Mesh.h"

class Model
{
private:
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
	PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
	PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

	void LoadMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<Vertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	Material LoadMaterial(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene);

public:
	glm::vec3 ModelPosition = glm::vec3(0.0f);
	glm::vec3 ModelRotation = glm::vec3(0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	std::vector<Mesh> MeshList;
	std::vector<MeshOffsets> MeshOffsetList;
	std::vector<VulkanBuffer> MeshOffsetBufferList;

	std::vector<Vertex> ModelVertices;
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

	Model();
	Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material& material);
	Model(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath);
	~Model();

	void Update();
	void Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
	void Destory(VkDevice& device);
};

