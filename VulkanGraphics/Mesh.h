#pragma once
#include "Vulkanengine.h"
#include "VulkanBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"
#include "Texture.h"
#include "SceneData.h"
#include "AccelerationStructure.h"
#include "MaterialManager.h"
#include "bone.h"
#include "Camera.h"
#include "InputManager.h"
#include "Timer.h"

enum RendererDrawFlag
{
	Renderer_Draw_None = 0,
	Renderer_Draw_Main_Pass = 1 << 0,
	Renderer_Draw_Shadow_Pass = 1 << 1,
	Renderer_Draw_Reflection_Pass = 1 << 2,
	Renderer_Draw_SkyBox = 1 << 3,
};
enum MeshTypeFlag
{
	Mesh_Type_Normal = 0,
	Mesh_Type_2D_Level_Tile = 1 << 0,
	Mesh_Type_2D_Level_Animated_Tile = 1 << 1,
	Mesh_Type_2D_Sprite = 1 << 2,
	Mesh_Type_Water = 1 << 3,
	Mesh_Type_SkyBox = 1 << 4,
	Mesh_Type_Billboard = 1 << 5
};

struct MeshBoneWeights
{
	glm::ivec4 BoneID = glm::ivec4(0);
	glm::vec4 BoneWeights = glm::vec4(0.0f);
};

class Mesh
{
private:
protected:
	std::shared_ptr<Timer> MeshTimer;

	void MeshBottomLevelAccelerationStructure();
	VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix)
	{
		return VkTransformMatrixKHR
		{
			matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
			matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
			matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
		};
	}
public:
	int DrawFlags = Renderer_Draw_Main_Pass | Renderer_Draw_Shadow_Pass | Renderer_Draw_Reflection_Pass;
	MeshTypeFlag MeshType = Mesh_Type_Normal;

	uint32_t ParentModelID = 0;
	uint32_t MeshID = 0;
	uint32_t MeshBufferIndex = 0;
	uint32_t BoneCount = 0;
	uint32_t VertexCount = 0;
	uint32_t IndexCount = 0;
	uint32_t PrimitiveCount; //TriangleCount

	glm::vec2 UVOffset = glm::vec2(0.0f);
	glm::vec2 UVScale = glm::vec2(1.0f);
	glm::vec2 UVFlip = glm::vec2(0.0f);
	std::shared_ptr<Mesh> ParentMesh;
	std::vector<std::shared_ptr<Mesh>> ChildMesh;
	std::shared_ptr<Material> MeshMaterial;

	std::vector<Vertex> VertexList;
	glm::mat4 MeshTransform;

	glm::vec3 MeshPosition = glm::vec3(0.0f);
	glm::vec3 MeshRotation = glm::vec3(0.0f);
	glm::vec3 MeshScale = glm::vec3(1.0f);
	std::vector<glm::mat4> BoneTransform;
	std::vector<MeshBoneWeights> BoneWeightList;

	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;
	VulkanBuffer BoneWeightBuffer;
	VulkanBuffer BoneTransformBuffer;
	VulkanBuffer TransformBuffer;
	VulkanBuffer TransformInverseBuffer;

	MeshPropertiesUniformBuffer MeshProperties;

	AccelerationStructure BottomLevelAccelerationBuffer;

	std::string MeshName;

	bool ShowMesh = true;

	VkAccelerationStructureGeometryKHR AccelerationStructureGeometry{};
	VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};

	Mesh();
	Mesh(std::vector<Vertex>& VertexList);
	Mesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	Mesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material);
	Mesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, std::vector<MeshBoneWeights>& boneWeights, uint32_t boneCount);
	~Mesh();

	void SetUpMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);

	virtual void Update();
	virtual void Update(const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList);

	virtual void Draw(VkCommandBuffer& commandBuffer);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView);
	virtual void DepthDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> lightViewCamera);
	
	virtual void Destory();
};
