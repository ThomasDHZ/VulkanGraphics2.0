#include "RayTraceRenderer.h"
#include "RayTraceVertexBuffer.h"
#include "RayTraceIndicesBuffer.h"
#include <fstream>

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

	BottomLevelAccelerationBuffer = VulkanBuffer(engine);
	TopLevelAccelerationBuffer = VulkanBuffer(engine);

	InitializeBottomLevelAccelerationStructure(engine);
	InitializeTopLevelAccelerationStructure(engine);
	InitializeRayTracingPipeline(engine);
	InitializeRayTracingShaderBindingTable(engine);
	InitializeRayTracingDescriptorSet(engine);
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


	ScratchBuffer.DestoryBuffer(engine);

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
	AccelerationInstance.accelerationStructureReference = engine.BufferToDeviceAddress(BottomLevelAccelerationBuffer.GetBuffer()).deviceAddress;

	VulkanBuffer TopLevelAccelerationInstanceBuffer = VulkanBuffer(engine);
	TopLevelAccelerationInstanceBuffer.CreateBuffer(engine, sizeof(VkAccelerationStructureInstanceKHR), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkDeviceOrHostAddressConstKHR TopLevelAccelerationInstanceBufferDeviceAddress{};
	TopLevelAccelerationInstanceBufferDeviceAddress.deviceAddress = engine.BufferToDeviceAddress(TopLevelAccelerationInstanceBuffer.GetBuffer()).deviceAddress;

	VkAccelerationStructureGeometryKHR AccelerationGeometry = {};
	AccelerationGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	AccelerationGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	AccelerationGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	AccelerationGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	AccelerationGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
	AccelerationGeometry.geometry.instances.data = TopLevelAccelerationInstanceBufferDeviceAddress;

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

	VkResult result = TopLevelAccelerationBuffer.CreateBuffer(engine, AccelerationBuildInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkAccelerationStructureCreateInfoKHR AccelerationStructureInfo = {};
	AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	AccelerationStructureInfo.buffer = TopLevelAccelerationBuffer.GetBuffer();
	AccelerationStructureInfo.size = TopLevelAccelerationBuffer.GetBufferSize();
	AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	result = vkCreateAccelerationStructureKHR(engine.Device, &AccelerationStructureInfo, nullptr, &TopLevelAccelerationStructure);

	VulkanBuffer ScratchBuffer = VulkanBuffer(engine);
	ScratchBuffer.CreateBuffer(engine, AccelerationBuildInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkDeviceOrHostAddressConstKHR ScratchBufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.GetBuffer());

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

	vkCmdBuildAccelerationStructuresKHR(RayTraceCommandBuffer, 1, &AccelerationBuildGeometryInfo, AcclerationBuildRangeList.data());

	VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo{};
	AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	AccelerationDeviceAddressInfo.accelerationStructure = TopLevelAccelerationStructure;
	TopLevelAccelerationDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &AccelerationDeviceAddressInfo);

	ScratchBuffer.DestoryBuffer(engine);
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
}

void RayTraceRenderer::InitializeRayTracingDescriptorSet(VulkanEngine& engine)
{
}

void RayTraceRenderer::Draw()
{
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

