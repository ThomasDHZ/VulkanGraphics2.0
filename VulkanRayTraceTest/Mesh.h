#pragma once
#include "VulkanEngine.h"
#include "VulkanBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"
#include "Texture.h"
#include "ForwardRenderingPipeline.h"
#include "SceneData.h"

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
	alignas(4) uint32_t ShadowMapID = 0;
};

struct MeshDetails
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct MeshOffsets
{
	uint32_t VertexOffset;
	uint32_t IndiceOffset;
};

struct AccelerationStructure {
	VkAccelerationStructureKHR handle = VK_NULL_HANDLE;
	uint64_t deviceAddress = 0;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkBuffer buffer = VK_NULL_HANDLE;
};

class Mesh
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

	void MeshBottomLevelAccelerationStructure(VulkanEngine& engine);
	void AcclerationCommandBuffer(VulkanEngine& engine, VkAccelerationStructureBuildGeometryInfoKHR& AccelerationStructureBuildGeometryInfo, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& AccelerationStructureBuildRangeInfo);
	void CreateAccelerationStructure(VulkanEngine& engine, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR& buildSizeInfo);
public:
		glm::mat4 MeshTransform;
		glm::vec3 MeshPosition = glm::vec3(0.0f);
		glm::vec3 MeshRotation = glm::vec3(0.0f);
		glm::vec3 MeshScale = glm::vec3(1.0f);

		Material material;
	
		VulkanBuffer IndexBuffer;
		VulkanBuffer VertexBuffer;
		VulkanBuffer TransformBuffer;
		VulkanBuffer MaterialBuffer;
	
		uint32_t MeshID;
		uint32_t VertexCount;
		uint32_t IndexCount;
		uint32_t PrimitiveCount; //TriangleCount
		uint32_t FirstIndex;     //primitiveOffset
		uint32_t VertexOffset;   //firstVertex

		bool ShowMesh = true;
	
		AccelerationStructure BottomLevelAccelerationStructure;

		VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress{};

		VkAccelerationStructureGeometryKHR AccelerationStructureGeometry{};
		VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};
		VkAccelerationStructureInstanceKHR AccelerationStructureInstance{};

	Mesh();
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t meshID);
	Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material MeshMaterial, uint32_t meshID);
	~Mesh();

	void SetUpMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);

	void Update(VulkanEngine& engine);
	void Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
	void Destory(VulkanEngine& engine);
};
