#pragma once
#include "RayTraceMesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>

class RayTraceModel
{
private:
	glm::mat4 ModelTransformMatrix = glm::mat4(1.0f);

public:
	std::vector<MeshDetails> MeshInfoList;
	std::vector<RayTraceMesh> MeshList;
	
	glm::vec3 ModelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ModelRotate = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	RayTraceModel();
	RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshDetails meshContainer);
	RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath);
	~RayTraceModel();

	void LoadMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<RTVertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	void Update();
};

