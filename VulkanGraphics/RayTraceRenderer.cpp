#include "RayTraceRenderer.h"

RayTraceRenderer::RayTraceRenderer()
{
	
}

RayTraceRenderer::RayTraceRenderer(VulkanEngine& Engine)
{
	InitializeBottomLevelAccelerationStructure(Engine);
	InitializeTopLevelAccelerationStructure(Engine);
	InitializeTopLevelAccelerationStructure(Engine);
	InitializeRayTracingDescriptorSet(Engine);
	InitializeRayTracingPipeline(Engine);
	InitializeRayTracingShaderBindingTable(Engine);
}

RayTraceRenderer::~RayTraceRenderer()
{
}

void RayTraceRenderer::InitializeBottomLevelAccelerationStructure(VulkanEngine& Engine)
{
	VkAccelerationStructureGeometryKHR GeometryAccelerationStructure = {};
	GeometryAccelerationStructure.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	GeometryAccelerationStructure.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	GeometryAccelerationStructure.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	GeometryAccelerationStructure.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
}

void RayTraceRenderer::InitializeTopLevelAccelerationStructure(VulkanEngine& Engine)
{
}

void RayTraceRenderer::InitializeRayTracingDescriptorSet(VulkanEngine& Engine)
{
}

void RayTraceRenderer::InitializeRayTracingPipeline(VulkanEngine& Engine)
{
}

void RayTraceRenderer::InitializeRayTracingShaderBindingTable(VulkanEngine& Engine)
{
}

void RayTraceRenderer::Draw()
{
}