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
#include "AnimatorCompute.h"

const unsigned int MAX_BONE_VERTEX_COUNT = 4;

enum ModelTypeEnum
{
	Model_Type_Normal = 0x00,
	Model_Type_2D_Level = 0x01
};

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
	void LoadMesh(const std::string& FilePath, aiNode* node, const aiScene* scene, MeshDrawFlags DrawFlags);
	std::vector<Vertex> LoadVertices(aiMesh* mesh);
	std::vector<MeshBoneWeights> LoadBoneWeights(aiMesh* mesh, std::vector<Vertex>& VertexList);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	std::shared_ptr<Material> LoadMaterial(const std::string& FilePath, aiMesh* mesh, const aiScene* scene);
	void LoadBones(const aiNode* RootNode, const aiMesh* mesh, std::vector<Vertex>& VertexList);
	void BoneWeightPlacement(std::vector<MeshBoneWeights>& VertexList, unsigned int vertexID, unsigned int bone_id, float weight);
	void LoadMeshTransform(const int NodeID, const glm::mat4 ParentMatrix);

	std::vector<Animation3D> AnimationList;
	std::vector<NodeMap> NodeMapList;
	glm::mat4 GlobalInverseTransformMatrix;
	AnimationPlayer3D AnimationPlayer;
	AnimatorCompute AnimationRenderer;

	bool AnimatedModel = false;

public:
	uint32_t ModelID = 0;
	ModelTypeEnum ModelType = ModelTypeEnum::Model_Type_Normal;

	glm::vec3 ModelPosition = glm::vec3(0.0f);
	glm::vec3 ModelRotation = glm::vec3(0.0f);
	glm::vec3 ModelScale = glm::vec3(1.0f);

	std::vector<std::shared_ptr<Mesh>> MeshList;
	std::vector<std::shared_ptr<Bone>> BoneList;

	std::vector<Vertex> ModelVertices;
	std::vector<uint32_t> ModelIndices;
	glm::mat4 ModelTransform;

	Model();
	Model(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags = Mesh_Draw);
	Model(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags = Mesh_Draw);
	Model(const std::string& FilePath, MeshDrawFlags DrawFlags = Mesh_Draw);
	~Model();

	void Update(bool RayTraceFlag);
	virtual void Update();
	void SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList);
	void AddMesh(std::shared_ptr<Mesh> mesh);
	void Destory();
	
	glm::mat4 AssimpToGLMMatrixConverter(aiMatrix4x4 AssMatrix);
	VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix);
};

