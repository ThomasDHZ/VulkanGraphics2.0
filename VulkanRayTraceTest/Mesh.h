#pragma once
#include "VulkanEngine.h"
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

enum MeshDrawFlags
{
	Mesh_Draw_All = 0x00,
	Mesh_Skip_Forward_Renderer = 0x01,
	Mesh_Skip_RayTrace_Renderer = 0x02,
	Mesh_Skip_Water_Renderer = 0x04,
	Mesh_SkyBox = 0x08,
};
enum MeshTypeFlag
{
	Mesh_Type_Normal = 0x00,
	Mesh_Type_2D_Level_Tile = 0x01,
	Mesh_Type_2D_Level_Animated_Tile = 0x02,
	Mesh_Type_2D_Sprite = 0x04,
	Mesh_Type_Water = 0x08,
	Mesh_Type_SkyBox = 0x16,
	Mesh_Type_Billboard = 0x32
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

	void MeshBottomLevelAccelerationStructure(VulkanEngine& engine);
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
	MeshDrawFlags DrawFlags = Mesh_Draw_All;
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
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshTypeFlag MeshType = Mesh_Type_Normal, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, std::vector<MeshBoneWeights>& boneWeights, uint32_t boneCount, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	~Mesh();

	void SetUpMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager);
	virtual void Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceModeFlag);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID);
	virtual void Destory(VulkanEngine& engine);
};
