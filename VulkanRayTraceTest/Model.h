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
	void BottomLevelAccelerationStructure(VulkanEngine& engine);
	void TopLevelAccelerationStructure(VulkanEngine& engine);
	void LoadMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<Vertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	Material LoadMaterial(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene);

public:
	glm::vec3 ModelPosition = glm::vec3(0.0f);
	glm::vec3 ModelRotation = glm::vec3(0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	std::vector<Mesh> MeshList;

	std::vector<Vertex> ModelVertices;
	std::vector<uint32_t> ModelIndices;
	glm::mat4 ModelTransform;

	VulkanBuffer ModelIndexBuffer;
	VulkanBuffer ModelVertexBuffer;
	VulkanBuffer ModelTransformBuffer;

	uint32_t ModelTriangleCount;
	uint32_t ModelVertexCount;
	uint32_t ModelIndexCount;

	AccelerationStructure BottomLevelAccelerationBuffer;
	AccelerationStructure TopLevelAccelerationBuffer;

	VkDeviceOrHostAddressConstKHR ModelVertexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR ModelIndexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR ModelTransformBufferDeviceAddress{};

	std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};

	Model();
	Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material& material);
	Model(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath);
	~Model();

	void Update(VulkanEngine& engine);
	void Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
	void Destory(VulkanEngine& engine);

	VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix)
	{
		return VkTransformMatrixKHR
		{
			matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
			matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
			matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
		};
	}
};

