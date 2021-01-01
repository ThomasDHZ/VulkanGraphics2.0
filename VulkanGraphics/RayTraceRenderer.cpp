#include "RayTraceRenderer.h"
#include "VertexBuffer.h"
#include "IndicesBuffer.h"
#include "TransformBuffer.h"

RayTraceRenderer::RayTraceRenderer()
{
	
}

RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine)
{
	InitializeBottomLevelAccelerationStructure(engine);
	InitializeTopLevelAccelerationStructure(engine);
	InitializeTopLevelAccelerationStructure(engine);
	InitializeRayTracingDescriptorSet(engine);
	InitializeRayTracingPipeline(engine);
	InitializeRayTracingShaderBindingTable(engine);
}

RayTraceRenderer::~RayTraceRenderer()
{
}

void RayTraceRenderer::InitializeBottomLevelAccelerationStructure(VulkanEngine& engine)
{

	const std::vector<Vertex> vertices = {
		{ {  1.0f,  1.0f, 0.0f } },
		{ { -1.0f,  1.0f, 0.0f } },
		{ {  0.0f, -1.0f, 0.0f } }
	};

	// Setup indices
	std::vector<uint32_t> indices = { 0, 1, 2 };

	// Setup identity transform matrix
	VkTransformMatrixKHR transformMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	//VkBuffer vertexBuffer = VertexBuffer(engine, vertices).GetVertexBuffer();
	//VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress = engine.BufferToHostAddress(vertexBuffer);

	//VkBuffer indicesBuffer = IndicesBuffer(engine, indices).GetIndiceBuffer();
	//VkDeviceOrHostAddressConstKHR IndicesBufferDeviceAddress = engine.BufferToHostAddress(indicesBuffer);

	//VkBuffer transformBuffer = TransformBuffer(engine, transformMatrix).GetTransformBuffer();
	//VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress = engine.BufferToHostAddress(indicesBuffer);

	//VkAccelerationStructureGeometryKHR GeometryAccelerationStructure = {};
	//GeometryAccelerationStructure.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	//GeometryAccelerationStructure.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	//GeometryAccelerationStructure.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	//GeometryAccelerationStructure.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	//GeometryAccelerationStructure.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
 //   GeometryAccelerationStructure.geometry.triangles.vertexData = VertexBufferDeviceAddress;
	//GeometryAccelerationStructure.geometry.triangles.maxVertex = vertices.size();
	//GeometryAccelerationStructure.geometry.triangles.vertexStride = sizeof(Vertex);
	//GeometryAccelerationStructure.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	//GeometryAccelerationStructure.geometry.triangles.indexData = IndicesBufferDeviceAddress;
}

void RayTraceRenderer::InitializeTopLevelAccelerationStructure(VulkanEngine& engine)
{
}

void RayTraceRenderer::InitializeRayTracingDescriptorSet(VulkanEngine& engine)
{
}

void RayTraceRenderer::InitializeRayTracingPipeline(VulkanEngine& engine)
{
}

void RayTraceRenderer::InitializeRayTracingShaderBindingTable(VulkanEngine& engine)
{
}

void RayTraceRenderer::Draw()
{
}