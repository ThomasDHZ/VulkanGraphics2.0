#include "RayTraceRenderer.h"
#include "RayTraceVertexBuffer.h"
#include "RayTraceIndicesBuffer.h"

RayTraceRenderer::RayTraceRenderer()
{
	
}

RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine)
{
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetBufferDeviceAddressKHR"));
	vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(engine.Device, "vkCmdBuildAccelerationStructuresKHR"));
	vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(engine.Device, "vkBuildAccelerationStructuresKHR"));
	vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(engine.Device, "vkCreateAccelerationStructureKHR"));
	vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(engine.Device, "vkDestroyAccelerationStructureKHR"));
	vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetAccelerationStructureBuildSizesKHR"));
	vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetAccelerationStructureDeviceAddressKHR"));
	vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(engine.Device, "vkCmdTraceRaysKHR"));
	vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetRayTracingShaderGroupHandlesKHR"));
	vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(engine.Device, "vkCreateRayTracingPipelinesKHR"));


	//BottomLevelAccelerationBuffer = VulkanBuffer(engine);

	InitializeBottomLevelAccelerationStructure(engine);
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

 std::vector<Vertex> vertices = {
		{ {  1.0f,  1.0f, 0.0f } },
		{ { -1.0f,  1.0f, 0.0f } },
		{ {  0.0f, -1.0f, 0.0f } }
	};

	std::vector<uint32_t> indices = { 0, 1, 2 };

	VkTransformMatrixKHR transformMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	VkBuffer vertexBuffer = RayTraceVertexBuffer(engine, vertices).GetVertexBuffer();
	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress = engine.BufferToDeviceAddress(vertexBuffer);
	
	VkBuffer indicesBuffer = RayTraceIndicesBuffer(engine, indices).GetIndiceBuffer();
	VkDeviceOrHostAddressConstKHR IndicesBufferDeviceAddress = engine.BufferToDeviceAddress(indicesBuffer);

	VkBuffer transformBuffer = TransformBuffer(engine, transformMatrix).GetTransformBuffer();
	VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress = engine.BufferToDeviceAddress(transformBuffer);
	
	VkAccelerationStructureGeometryKHR GeometryAccelerationStructure = {};
	GeometryAccelerationStructure.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	GeometryAccelerationStructure.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	GeometryAccelerationStructure.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	GeometryAccelerationStructure.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	GeometryAccelerationStructure.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    GeometryAccelerationStructure.geometry.triangles.vertexData = VertexBufferDeviceAddress;
	GeometryAccelerationStructure.geometry.triangles.maxVertex = vertices.size();
	GeometryAccelerationStructure.geometry.triangles.vertexStride = sizeof(Vertex);
	GeometryAccelerationStructure.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	GeometryAccelerationStructure.geometry.triangles.indexData = IndicesBufferDeviceAddress;
	GeometryAccelerationStructure.geometry.triangles.transformData.deviceAddress = 0;
	GeometryAccelerationStructure.geometry.triangles.transformData.hostAddress = nullptr;
	GeometryAccelerationStructure.geometry.triangles.transformData = TransformBufferDeviceAddress;

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometry = {};
	AccelerationBuildGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationBuildGeometry.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationBuildGeometry.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	AccelerationBuildGeometry.geometryCount = 1;
	AccelerationBuildGeometry.pGeometries = &GeometryAccelerationStructure;

	uint32_t geoCount = 1;
	VkAccelerationStructureBuildSizesInfoKHR AccelerationBuildInfo = {};
	AccelerationBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationBuildGeometry, &geoCount, &AccelerationBuildInfo);

	VkResult result = BottomLevelAccelerationBuffer.CreateBuffer(engine, AccelerationBuildInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkAccelerationStructureCreateInfoKHR AccelerationStructureInfo = {};
	AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	AccelerationStructureInfo.buffer = BottomLevelAccelerationBuffer.GetBuffer();
	AccelerationStructureInfo.size = BottomLevelAccelerationBuffer.GetBufferSize();
	AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	result = vkCreateAccelerationStructureKHR(engine.Device, &AccelerationStructureInfo, nullptr, &BottomLevelAccelerationStructure);

	VulkanBuffer ScratchBuffer = VulkanBuffer(engine);
	ScratchBuffer.CreateBuffer(engine, AccelerationBuildInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkDeviceOrHostAddressConstKHR ScratchBufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.GetBuffer());

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
	AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	AccelerationBuildGeometryInfo.dstAccelerationStructure = BottomLevelAccelerationStructure;
	AccelerationBuildGeometryInfo.geometryCount = 1;
	AccelerationBuildGeometryInfo.pGeometries = &GeometryAccelerationStructure;
	AccelerationBuildGeometryInfo.scratchData.deviceAddress = ScratchBufferDeviceAddress.deviceAddress;

	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> AcclerationBuildRangeList;
	VkAccelerationStructureBuildRangeInfoKHR AcclerationBuildRange = {};
	AcclerationBuildRange.primitiveCount = geoCount;
	AcclerationBuildRange.primitiveOffset = 0;
	AcclerationBuildRange.firstVertex = 0;
	AcclerationBuildRange.transformOffset = 0;
	AcclerationBuildRangeList.emplace_back(&AcclerationBuildRange);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = engine.GetRenderCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &RayTraceCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(RayTraceCommandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBuildAccelerationStructuresKHR(RayTraceCommandBuffer, 1, &AccelerationBuildGeometryInfo, AcclerationBuildRangeList.data());

	VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo{};
	AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	AccelerationDeviceAddressInfo.accelerationStructure = BottomLevelAccelerationStructure;
	BottomLevelAccelerationDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &AccelerationDeviceAddressInfo);


	//VkDebugUtilsObjectNameInfoEXT DebugName = {};
	//DebugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	//DebugName.objectType = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR;
	//DebugName.objectHandle = BottomLevelAccelerationDeviceAddress;
	//DebugName.pObjectName = "BSRayTrace";
	//DebugName.pNext = VK_NULL_HANDLE;
 //   result = engine.vkSetDebugUtilsObjectNameEXT(engine.Device, &DebugName);
	int a = 34;
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