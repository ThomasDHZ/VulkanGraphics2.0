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
	void BottomLevelAccelerationStructure(VulkanEngine& engine);

	void LoadNodeTree(const aiNode* Node, int parentNodeID = -1);
	void LoadAnimations(const aiScene* scene);
	void LoadMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<Vertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	Material LoadMaterial(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene);
	void LoadBones(VulkanEngine& engine, const aiNode* RootNode, const aiMesh* mesh, std::vector<Vertex>& VertexList);
	void BoneWeightPlacement(std::vector<Vertex>& VertexList, unsigned int vertexID, unsigned int bone_id, float weight);
	void LoadMeshTransform(const int NodeID, const glm::mat4 ParentMatrix);

	std::vector<Animation3D> AnimationList;
	std::vector<NodeMap> NodeMapList;
	glm::mat4 GlobalInverseTransformMatrix;
	AnimationPlayer3D AnimationPlayer;

public:
	glm::vec3 ModelPosition = glm::vec3(0.0f);
	glm::vec3 ModelRotation = glm::vec3(0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	std::vector<Mesh> MeshList;
	std::vector<std::shared_ptr<Bone>> BoneList;

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

	std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};

	Model();
	Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material& material);
	Model(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath);
	~Model();

	void Update(VulkanEngine& engine, std::shared_ptr<SceneDataStruct> scenedata);
	void Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
	void Destory(VulkanEngine& engine);

	glm::mat4 AssimpToGLMMatrixConverter(aiMatrix4x4 AssMatrix);
	VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix);
};

