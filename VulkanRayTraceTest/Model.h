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
#include <unordered_map>
#include "Animation3D.h"
#include "AnimationPlayer3D.h"
#include "MeshManager.h"
#include "AnimatorCompute.h"

const unsigned int MAX_BONE_VERTEX_COUNT = 4;

class Node
{
	unsigned int NodeID;
	std::shared_ptr<Node> ParentNode;
	std::vector<std::shared_ptr<Node>> Children;
};

class Model
{
private:
	void LoadNodeTree(const aiNode* Node, int parentNodeID = -1);
	void LoadAnimations(const aiScene* scene);
	void LoadMesh(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materialManager, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<Vertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	uint32_t LoadMaterial(VulkanEngine& engine, MaterialManager& materailManager, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene);
	void LoadBones(VulkanEngine& engine, const aiNode* RootNode, const aiMesh* mesh, std::vector<Vertex>& VertexList);
	void BoneWeightPlacement(std::vector<Vertex>& VertexList, unsigned int vertexID, unsigned int bone_id, float weight);
	void LoadMeshTransform(const int NodeID, const glm::mat4 ParentMatrix);

	std::vector<Animation3D> AnimationList;
	std::vector<NodeMap> NodeMapList;
	glm::mat4 GlobalInverseTransformMatrix;
	AnimationPlayer3D AnimationPlayer;
	AnimatorCompute AnimationRenderer;
public:
	uint32_t ModelID = 0;

	glm::vec3 ModelPosition = glm::vec3(0.0f);
	glm::vec3 ModelRotation = glm::vec3(0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	std::vector<std::shared_ptr<Mesh>> MeshList;
	std::vector<std::shared_ptr<Bone>> BoneList;

	std::vector<Vertex> ModelVertices;
	std::vector<uint32_t> ModelIndices;
	glm::mat4 ModelTransform;

	Model();
	Model(VulkanEngine& engine, MeshManager& meshManager, TextureManager& textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	Model(VulkanEngine& engine, MeshManager& meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t materialID);
	Model(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath);
	~Model();

	void Update(VulkanEngine& engine, MaterialManager& materialManager);
	void AddMesh(VulkanEngine& engine, std::shared_ptr<Mesh> mesh);

	glm::mat4 AssimpToGLMMatrixConverter(aiMatrix4x4 AssMatrix);
	VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix);
};

