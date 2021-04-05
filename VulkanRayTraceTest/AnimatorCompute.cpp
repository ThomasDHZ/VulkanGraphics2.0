#include "AnimatorCompute.h"

#include "Vertex.h"
#include "GraphicsPipeline.h"

AnimatorCompute::AnimatorCompute()
{
}

AnimatorCompute::AnimatorCompute(VulkanEngine& engine,  std::shared_ptr<Mesh> meshptr)
{
	mesh = meshptr;

	SetUpDescriptorPool(engine);
	SetUpDescriptorLayout(engine);
	SetUpDescriptorSets(engine);
	CreateShaderPipeLine(engine);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = engine.CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

AnimatorCompute::~AnimatorCompute()
{
}

void AnimatorCompute::SetUpDescriptorPool(VulkanEngine& engine)
{
	std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1));
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1));
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1));

	descriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void AnimatorCompute::SetUpDescriptorLayout(VulkanEngine& engine)
{
		std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
		LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
		LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
		LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
		descriptorLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}
	
void AnimatorCompute::SetUpDescriptorSets(VulkanEngine& engine)
{
	VertexBufferCopy = std::make_shared<VulkanBuffer>(mesh->VertexBuffer);

	descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorLayout);

	VkDescriptorBufferInfo VertexBufferInfo = engine.AddBufferDescriptor(mesh->VertexBuffer);
	VkDescriptorBufferInfo MeshDataufferInfo = engine.AddBufferDescriptor(mesh->MeshProperties.VulkanBufferData);
	VkDescriptorBufferInfo TransformDataBufferInfo = engine.AddBufferDescriptor(mesh->TransformBuffer);

	std::vector<VkWriteDescriptorSet> DescriptorList;
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(0, descriptorSets, VertexBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(3, descriptorSets, MeshDataufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(6, descriptorSets, TransformDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void AnimatorCompute::CreateShaderPipeLine(VulkanEngine& engine)
{
	auto ComputeShaderCode = engine.CreateShader("Shader/animate.spv", VK_SHADER_STAGE_COMPUTE_BIT);

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(ConstMeshInfo);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorLayout;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	vkCreatePipelineLayout(engine.Device, &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout);

	VkPipelineCacheCreateInfo pipelineCacheInfo{};
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(engine.Device, &pipelineCacheInfo, nullptr, &PipelineCache);

	VkComputePipelineCreateInfo ComputePipelineInfo{};
	ComputePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	ComputePipelineInfo.layout = ShaderPipelineLayout;
	ComputePipelineInfo.flags = 0;
	ComputePipelineInfo.stage = ComputeShaderCode;

	if (vkCreateComputePipelines(engine.Device, PipelineCache, 1, &ComputePipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}

	vkDestroyShaderModule(engine.Device, ComputeShaderCode.module, nullptr);
}

void AnimatorCompute::Compute(VulkanEngine& engine, uint32_t currentFrame)
{
	ConstMeshInfo meshInfo;
	meshInfo.MeshIndex = 0;

	VkCommandBufferBeginInfo CommandBufferBeginInfo{};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	vkBeginCommandBuffer(commandBuffer, &CommandBufferBeginInfo);

	VkBufferMemoryBarrier BufferMemoryBarrier{};
	BufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.buffer = VertexBufferCopy->Buffer;
	BufferMemoryBarrier.size = VK_WHOLE_SIZE;
	BufferMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	BufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, nullptr,
		1, &BufferMemoryBarrier,
		0, nullptr);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ShaderPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ShaderPipelineLayout, 0, 1, &descriptorSets, 0, 0);
	vkCmdPushConstants(commandBuffer, ShaderPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ConstMeshInfo), &meshInfo);
	vkCmdDispatch(commandBuffer, VertexBufferCopy->BufferSize, 1, 1);

	BufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	BufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	BufferMemoryBarrier.buffer = VertexBufferCopy->Buffer;
	BufferMemoryBarrier.size = VK_WHOLE_SIZE;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		1, &BufferMemoryBarrier,
		0, nullptr);

	mesh->VertexBuffer.CopyBufferToMemory(engine.Device, &mesh->VertexList[0], sizeof(Vertex) * mesh->VertexList.size());
	vkEndCommandBuffer(commandBuffer);
}

void AnimatorCompute::Destroy(VulkanEngine& engine)
{
	vkDestroyPipeline(engine.Device, ShaderPipeline, nullptr);
	vkDestroyPipelineLayout(engine.Device, ShaderPipelineLayout, nullptr);
	vkDestroyPipelineCache(engine.Device, PipelineCache, nullptr);
	vkDestroyDescriptorSetLayout(engine.Device, descriptorLayout, nullptr);
	vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);

	ShaderPipeline = VK_NULL_HANDLE;
	ShaderPipelineLayout = VK_NULL_HANDLE;
	PipelineCache = VK_NULL_HANDLE;
	descriptorLayout = VK_NULL_HANDLE;
	descriptorPool = VK_NULL_HANDLE;
}