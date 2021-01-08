#include "RayTraceRenderer.h"
#include "RayTraceVertexBuffer.h"
#include "RayTraceIndicesBuffer.h"
#include <fstream>
#include <chrono>
RayTraceRenderer::RayTraceRenderer()
{
	
}

RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine)
{
	camera.type = Camera2::CameraType::lookat;
	camera.setPerspective(60.0f, (float)engine.SwapChain.GetSwapChainResolution().width / (float)engine.SwapChain.GetSwapChainResolution().height, 0.1f, 512.0f);
	camera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.setTranslation(glm::vec3(0.0f, 0.0f, -2.5f));

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

	RTTexture = RayTraceTexture(engine);

	uniformBuffer = VulkanUniformBuffer(engine, sizeof(RTUniformData));

	InitializeBottomLevelAccelerationStructure(engine);
	InitializeTopLevelAccelerationStructure(engine);
	InitializeRayTracingPipeline(engine);
	InitializeRayTracingShaderBindingTable(engine);
	InitializeRayTracingDescriptorSet(engine);
	InitializeRayTracingCommandBuffer(engine);
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
	VkBuffer indicesBuffer = RayTraceIndicesBuffer(engine, indices).GetIndiceBuffer();
	VkBuffer transformBuffer = TransformBuffer(engine, transformMatrix).GetTransformBuffer();

	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress = engine.BufferToDeviceAddress(vertexBuffer);
	VkDeviceOrHostAddressConstKHR IndicesBufferDeviceAddress = engine.BufferToDeviceAddress(indicesBuffer);
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

	createAccelerationStructureBuffer(engine, BottomLevelAccelerationBuffer, AccelerationBuildInfo);

	VkAccelerationStructureCreateInfoKHR AccelerationStructureInfo = {};
	AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	AccelerationStructureInfo.buffer = BottomLevelAccelerationBuffer.buffer;
	AccelerationStructureInfo.size = AccelerationBuildInfo.accelerationStructureSize;
	AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	VkResult result = vkCreateAccelerationStructureKHR(engine.Device, &AccelerationStructureInfo, nullptr, &BottomLevelAccelerationStructure);

	RayTracingScratchBuffer ScratchBuffer = createScratchBuffer(engine, AccelerationBuildInfo.accelerationStructureSize);
	VkDeviceOrHostAddressConstKHR ScratchBufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.handle);

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

	VkCommandBuffer TempCMDBuffer;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = engine.GetRenderCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &TempCMDBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(TempCMDBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	vkCmdBuildAccelerationStructuresKHR(TempCMDBuffer, 1, &AccelerationBuildGeometryInfo, AcclerationBuildRangeList.data());
	vkEndCommandBuffer(TempCMDBuffer);

	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &TempCMDBuffer;
	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;

	VkFence fence;
	vkCreateFence(engine.Device, &fenceCreateInfo, nullptr, &fence);
	vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, fence);
	vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, 100000000000);
	vkDestroyFence(engine.Device, fence, nullptr);
	vkFreeCommandBuffers(engine.Device, engine.GetRenderCommandPool(), 1, &TempCMDBuffer);
	

	VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo{};
	AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	AccelerationDeviceAddressInfo.accelerationStructure = BottomLevelAccelerationStructure;
	BottomLevelAccelerationDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &AccelerationDeviceAddressInfo);

	//ScratchBuffer.DestoryBuffer(engine);

	//VkDebugUtilsObjectNameInfoEXT DebugName = {};
	//DebugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	//DebugName.objectType = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR;
	//DebugName.objectHandle = BottomLevelAccelerationDeviceAddress;
	//DebugName.pObjectName = "BSRayTrace";
	//DebugName.pNext = VK_NULL_HANDLE;
 //   result = engine.vkSetDebugUtilsObjectNameEXT(engine.Device, &DebugName);
}

void RayTraceRenderer::InitializeTopLevelAccelerationStructure(VulkanEngine& engine)
{
	VkTransformMatrixKHR transformMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	VkAccelerationStructureInstanceKHR AccelerationInstance = {};
	AccelerationInstance.transform = transformMatrix;
	AccelerationInstance.instanceCustomIndex = 0;
	AccelerationInstance.mask = 0xFF;
	AccelerationInstance.instanceShaderBindingTableRecordOffset = 0;
	AccelerationInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
	AccelerationInstance.accelerationStructureReference = BottomLevelAccelerationBuffer.deviceAddress;

	VulkanBuffer instancesBuffer;
	instancesBuffer.CreateBuffer(engine, sizeof(VkAccelerationStructureInstanceKHR), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &AccelerationInstance);


	VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{};
	instanceDataDeviceAddress.deviceAddress = engine.BufferToDeviceAddress(instancesBuffer.Buffer).deviceAddress;

	VkAccelerationStructureGeometryKHR AccelerationGeometry = {};
	AccelerationGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	AccelerationGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	AccelerationGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	AccelerationGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	AccelerationGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	AccelerationGeometry.geometry.instances.data = instanceDataDeviceAddress;

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometry = {};
	AccelerationStructureBuildGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationStructureBuildGeometry.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	AccelerationStructureBuildGeometry.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	AccelerationStructureBuildGeometry.geometryCount = 1;
	AccelerationStructureBuildGeometry.pGeometries = &AccelerationGeometry;

	uint32_t geoCount = 1;
	VkAccelerationStructureBuildSizesInfoKHR AccelerationBuildInfo = {};
	AccelerationBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometry, &geoCount, &AccelerationBuildInfo);

	createAccelerationStructureBuffer(engine, TopLevelAccelerationBuffer, AccelerationBuildInfo);

	VkAccelerationStructureCreateInfoKHR AccelerationStructureInfo = {};
	AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	AccelerationStructureInfo.buffer = TopLevelAccelerationBuffer.buffer;
	AccelerationStructureInfo.size = AccelerationBuildInfo.accelerationStructureSize;
	AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	VkResult result = vkCreateAccelerationStructureKHR(engine.Device, &AccelerationStructureInfo, nullptr, &TopLevelAccelerationStructure);

	RayTracingScratchBuffer ScratchBuffer = createScratchBuffer(engine, AccelerationBuildInfo.accelerationStructureSize);
	VkDeviceOrHostAddressConstKHR ScratchBufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.handle);

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
	AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	AccelerationBuildGeometryInfo.dstAccelerationStructure = TopLevelAccelerationStructure;
	AccelerationBuildGeometryInfo.geometryCount = 1;
	AccelerationBuildGeometryInfo.pGeometries = &AccelerationGeometry;
	AccelerationBuildGeometryInfo.scratchData.deviceAddress = ScratchBufferDeviceAddress.deviceAddress;

	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> AcclerationBuildRangeList;
	VkAccelerationStructureBuildRangeInfoKHR AcclerationBuildRange = {};
	AcclerationBuildRange.primitiveCount = geoCount;
	AcclerationBuildRange.primitiveOffset = 0;
	AcclerationBuildRange.firstVertex = 0;
	AcclerationBuildRange.transformOffset = 0;
	AcclerationBuildRangeList.emplace_back(&AcclerationBuildRange);

	VkCommandBuffer TempCMDBuffer;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = engine.GetRenderCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &TempCMDBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(TempCMDBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	vkCmdBuildAccelerationStructuresKHR(TempCMDBuffer, 1, &AccelerationBuildGeometryInfo, AcclerationBuildRangeList.data());
	vkEndCommandBuffer(TempCMDBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &TempCMDBuffer;
	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;

	VkFence fence;
	vkCreateFence(engine.Device, &fenceCreateInfo, nullptr, &fence);
	vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, fence);
	vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, 100000000000);
	vkDestroyFence(engine.Device, fence, nullptr);
	vkFreeCommandBuffers(engine.Device, engine.GetRenderCommandPool(), 1, &TempCMDBuffer);
	
	VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo{};
	AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	AccelerationDeviceAddressInfo.accelerationStructure = TopLevelAccelerationStructure;
	TopLevelAccelerationDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &AccelerationDeviceAddressInfo);

	//ScratchBuffer.(engine);
}

void RayTraceRenderer::InitializeRayTracingPipeline(VulkanEngine& engine)
{
	std::vector<VkDescriptorSetLayoutBinding> RTDescriptorSetBindings;
	VkDescriptorSetLayoutBinding AccelerationStructureBinding = {};
	AccelerationStructureBinding.binding = 0;
	AccelerationStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	AccelerationStructureBinding.descriptorCount = 1;
	AccelerationStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	RTDescriptorSetBindings.emplace_back(AccelerationStructureBinding);

	VkDescriptorSetLayoutBinding ReturnImageStructureBinding = {};
	ReturnImageStructureBinding.binding = 1;
	ReturnImageStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	ReturnImageStructureBinding.descriptorCount = 1;
	ReturnImageStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	RTDescriptorSetBindings.emplace_back(ReturnImageStructureBinding);

	VkDescriptorSetLayoutBinding UniformBufferStructureBinding = {};
	UniformBufferStructureBinding.binding = 2;
	UniformBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UniformBufferStructureBinding.descriptorCount = 1;
	UniformBufferStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	RTDescriptorSetBindings.emplace_back(UniformBufferStructureBinding);

	VkDescriptorSetLayoutCreateInfo RTDescriptorSetLayout = {};
	RTDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	RTDescriptorSetLayout.bindingCount = static_cast<uint32_t>(RTDescriptorSetBindings.size());
	RTDescriptorSetLayout.pBindings = RTDescriptorSetBindings.data();
	vkCreateDescriptorSetLayout(engine.Device, &RTDescriptorSetLayout, nullptr, &RayTraceDescriptorSetLayout);

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.setLayoutCount = 1;
	PipelineLayoutCreateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
	vkCreatePipelineLayout(engine.Device, &PipelineLayoutCreateInfo, nullptr, &RayTracePipelineLayout);

	std::vector<VkPipelineShaderStageCreateInfo> ShaderList;

	ShaderList.emplace_back(LoadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
	VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
	RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
	RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
	RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
	RayTraceShaders.emplace_back(RayGeneratorShaderInfo);
	
	ShaderList.emplace_back(LoadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
	VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
	MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
	MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
	MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
	MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
	RayTraceShaders.emplace_back(MissShaderInfo);

	ShaderList.emplace_back(LoadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
	VkRayTracingShaderGroupCreateInfoKHR ClosestHitShaderInfo = {};
	ClosestHitShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
	ClosestHitShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	ClosestHitShaderInfo.generalShader = VK_SHADER_UNUSED_KHR;
	ClosestHitShaderInfo.closestHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;
	ClosestHitShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
	ClosestHitShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
	RayTraceShaders.emplace_back(ClosestHitShaderInfo);

	VkRayTracingPipelineCreateInfoKHR RayTracingPipeline = {};
	RayTracingPipeline.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	RayTracingPipeline.stageCount = static_cast<uint32_t>(ShaderList.size());
	RayTracingPipeline.pStages = ShaderList.data();
	RayTracingPipeline.groupCount = static_cast<uint32_t>(RayTraceShaders.size());
	RayTracingPipeline.pGroups = RayTraceShaders.data();
	RayTracingPipeline.maxPipelineRayRecursionDepth = 1;
	RayTracingPipeline.layout = RayTracePipelineLayout;
	VkResult result = vkCreateRayTracingPipelinesKHR(engine.Device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &RayTracingPipeline, nullptr, &RayTracePipeline);
}

void RayTraceRenderer::InitializeRayTracingShaderBindingTable(VulkanEngine& engine)
{
	const VkPhysicalDeviceRayTracingPipelinePropertiesKHR PhysicalDeviceRayTracingPipelineProperties = engine.GetRayTracingPipelineProperties(engine.PhysicalDevice);
	const uint32_t HandleSize = PhysicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize;
	const uint32_t AlignedHandleSize = engine.GetShaderGroupAlignment(engine.PhysicalDevice);
	const uint32_t GroupCount = static_cast<uint32_t>(RayTraceShaders.size());
	const uint32_t Sizing = GroupCount * AlignedHandleSize;
	std::vector<uint8_t> shaderHandleStorage(Sizing);

	VkResult result = vkGetRayTracingShaderGroupHandlesKHR(engine.Device, RayTracePipeline, 0, GroupCount, Sizing, shaderHandleStorage.data());
	
	raygenShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data());
	missShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + AlignedHandleSize);
	hitShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + AlignedHandleSize * 2);
}

void RayTraceRenderer::InitializeRayTracingDescriptorSet(VulkanEngine& engine)
{
	std::vector<VkDescriptorPoolSize> poolSizes = {
	{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
	};

	VkDescriptorPoolCreateInfo RayTraceDescriptorPoolInfo = {};
	RayTraceDescriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	RayTraceDescriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	RayTraceDescriptorPoolInfo.pPoolSizes = poolSizes.data();
	RayTraceDescriptorPoolInfo.maxSets = 1;
	vkCreateDescriptorPool(engine.Device, &RayTraceDescriptorPoolInfo, nullptr, &RTDescriptorPool);

	VkDescriptorSetAllocateInfo RayTraceDescriptorSetAllocateInfo = {};
	RayTraceDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	RayTraceDescriptorSetAllocateInfo.descriptorPool = RTDescriptorPool;
	RayTraceDescriptorSetAllocateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
	RayTraceDescriptorSetAllocateInfo.descriptorSetCount = 1;
	vkAllocateDescriptorSets(engine.Device, &RayTraceDescriptorSetAllocateInfo, &RTDescriptorSet);

	VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = {};
	AccelerationDescriptorStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	AccelerationDescriptorStructure.accelerationStructureCount = 1;
	AccelerationDescriptorStructure.pAccelerationStructures = &TopLevelAccelerationStructure;

	VkWriteDescriptorSet AccelerationDesciptorSet = {};
	AccelerationDesciptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	AccelerationDesciptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	AccelerationDesciptorSet.dstSet = RTDescriptorSet;
	AccelerationDesciptorSet.dstBinding = 0;
	AccelerationDesciptorSet.descriptorCount = 1;
	AccelerationDesciptorSet.pNext = &AccelerationDescriptorStructure;

	VkDescriptorImageInfo RayTraceImageDescriptor{};
	RayTraceImageDescriptor.imageView = RTTexture.View;
	RayTraceImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet ImageDescriptorSet{};
	ImageDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	ImageDescriptorSet.dstSet = RTDescriptorSet;
	ImageDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	ImageDescriptorSet.dstBinding = 1;
	ImageDescriptorSet.pImageInfo = &RayTraceImageDescriptor;
	ImageDescriptorSet.descriptorCount = 1;

	VkDescriptorBufferInfo RTBufferInfo = {};
	RTBufferInfo.buffer = uniformBuffer.GetUniformBuffer(0);
	RTBufferInfo.offset = 0;
	RTBufferInfo.range = uniformBuffer.GetBufferSize();

	VkWriteDescriptorSet UniformDescriptorSet{};
	UniformDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	UniformDescriptorSet.dstSet = RTDescriptorSet;
	UniformDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UniformDescriptorSet.dstBinding = 2;
	UniformDescriptorSet.pBufferInfo = &RTBufferInfo;
	UniformDescriptorSet.descriptorCount = 1;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
		AccelerationDesciptorSet,
		ImageDescriptorSet,
		UniformDescriptorSet
	};
	vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);
}

void RayTraceRenderer::InitializeRayTracingCommandBuffer(VulkanEngine& engine)
{
	RayTraceCommandBuffer.resize(engine.SwapChain.GetSwapChainImageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = engine.GetRenderCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)RayTraceCommandBuffer.size();

	if (vkAllocateCommandBuffers(engine.Device, &allocInfo, RayTraceCommandBuffer.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	const uint32_t AlignedHandleSize = engine.GetShaderGroupAlignment(engine.PhysicalDevice);

	VkStridedDeviceAddressRegionKHR RaygenShader{};
	RaygenShader.deviceAddress = engine.BufferToDeviceAddress(raygenShaderBindingTable.GetBuffer()).deviceAddress;
	RaygenShader.stride = AlignedHandleSize;
	RaygenShader.size = AlignedHandleSize;

	VkStridedDeviceAddressRegionKHR MissShaderEntry{};
	MissShaderEntry.deviceAddress = engine.BufferToDeviceAddress(missShaderBindingTable.GetBuffer()).deviceAddress;
	MissShaderEntry.stride = AlignedHandleSize;
	MissShaderEntry.size = AlignedHandleSize;

	VkStridedDeviceAddressRegionKHR HitShaderEntry{};
	HitShaderEntry.deviceAddress = engine.BufferToDeviceAddress(hitShaderBindingTable.GetBuffer()).deviceAddress;
	HitShaderEntry.stride = AlignedHandleSize;
	HitShaderEntry.size = AlignedHandleSize;

	VkStridedDeviceAddressRegionKHR ShaderIndex{};
	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	for (size_t x = 0; x < RayTraceCommandBuffer.size(); x++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(RayTraceCommandBuffer[x], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		vkCmdBindPipeline(RayTraceCommandBuffer[x], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipeline);
		vkCmdBindDescriptorSets(RayTraceCommandBuffer[x], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipelineLayout, 0, 1, &RTDescriptorSet, 0, 0);
		vkCmdTraceRaysKHR(RayTraceCommandBuffer[x], &RaygenShader, &MissShaderEntry, &HitShaderEntry, &ShaderIndex, engine.SwapChain.GetSwapChainResolution().width, engine.SwapChain.GetSwapChainResolution().height, 1);


			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.image = engine.SwapChain.GetSwapChainImages()[x];
			barrier.subresourceRange = subresourceRange;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			vkCmdPipelineBarrier(RayTraceCommandBuffer[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkImageMemoryBarrier barrier2 = {};
			barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier2.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
			barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier2.image = RTTexture.Image;
			barrier2.subresourceRange = subresourceRange;
			barrier2.srcAccessMask = 0;
			barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(RayTraceCommandBuffer[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

			VkImageCopy copyRegion{};
			copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			copyRegion.srcOffset = { 0, 0, 0 };
			copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			copyRegion.dstOffset = { 0, 0, 0 };
			copyRegion.extent = { engine.GetSwapChainResolution().width, engine.GetSwapChainResolution().height, 1 };
			vkCmdCopyImage(RayTraceCommandBuffer[x], RTTexture.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, engine.SwapChain.SwapChainImages[x], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


			VkImageMemoryBarrier barrier3 = {};
			barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier3.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			barrier3.image = engine.SwapChain.GetSwapChainImages()[x];
			barrier3.subresourceRange = subresourceRange;
			barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier3.dstAccessMask = 0;

			vkCmdPipelineBarrier(RayTraceCommandBuffer[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

			VkImageMemoryBarrier barrier4 = {};
			barrier4.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier4.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier4.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			barrier4.image = RTTexture.Image;
			barrier4.subresourceRange = subresourceRange;
			barrier4.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier4.dstAccessMask = 0;

			vkCmdPipelineBarrier(RayTraceCommandBuffer[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier4);


		if (vkEndCommandBuffer(RayTraceCommandBuffer[x]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void RayTraceRenderer::Update(VulkanEngine& engine)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto  currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	camera.rotate(glm::vec3(time * camera.rotationSpeed, -time * camera.rotationSpeed, 0.0f));

	ubo.projInverse = glm::inverse(camera.matrices.perspective);
	ubo.viewInverse = glm::inverse(camera.matrices.view);
	uniformBuffer.UpdateUniformBuffer(engine, static_cast<void*>(&ubo));
}

std::vector<char> RayTraceRenderer::ReadShaderFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

VkShaderModule RayTraceRenderer::CreateShaderModule(VulkanEngine& engine, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(engine.Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

VkPipelineShaderStageCreateInfo RayTraceRenderer::LoadShader(VulkanEngine& engine, const std::string& filename, VkShaderStageFlagBits StageFlag)
{
	auto ShaderCode = ReadShaderFile(filename);

	VkShaderModule ShaderModule = CreateShaderModule(engine, ShaderCode);

	VkPipelineShaderStageCreateInfo ShaderInfo{};
	ShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderInfo.stage = StageFlag;
	ShaderInfo.module = ShaderModule;
	ShaderInfo.pName = "main";

	return ShaderInfo;
}

void RayTraceRenderer::createAccelerationStructureBuffer(VulkanEngine& engine, AccelerationStructure& accelerationStructure, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = buildSizeInfo.accelerationStructureSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	vkCreateBuffer(engine.Device, &bufferCreateInfo, nullptr, &accelerationStructure.buffer);
	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(engine.Device, accelerationStructure.buffer, &memoryRequirements);
	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &accelerationStructure.memory);
	vkBindBufferMemory(engine.Device, accelerationStructure.buffer, accelerationStructure.memory, 0);
}

RayTracingScratchBuffer RayTraceRenderer::createScratchBuffer(VulkanEngine& engine, VkDeviceSize size)
{
	RayTracingScratchBuffer scratchBuffer{};

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	vkCreateBuffer(engine.Device, &bufferCreateInfo, nullptr, &scratchBuffer.handle);

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(engine.Device, scratchBuffer.handle, &memoryRequirements);

	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &scratchBuffer.memory);
	vkBindBufferMemory(engine.Device, scratchBuffer.handle, scratchBuffer.memory, 0);

	VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
	bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferDeviceAddressInfo.buffer = scratchBuffer.handle;
	scratchBuffer.deviceAddress = vkGetBufferDeviceAddressKHR(engine.Device, &bufferDeviceAddressInfo);

	return scratchBuffer;
}

uint32_t RayTraceRenderer::getMemoryType(VulkanEngine& engine, uint32_t typeBits, VkMemoryPropertyFlags properties)
{
	VkBool32* memTypeFound = nullptr;
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(engine.PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				if (memTypeFound)
				{
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound)
	{
		*memTypeFound = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Could not find a matching memory type");
	}
}