#include "ComputeAnimationPipeline.h"
#include "Vertex.h"
#include "GraphicsPipeline.h"

ComputeAnimationPipeline::ComputeAnimationPipeline()
{
}

ComputeAnimationPipeline::ComputeAnimationPipeline(std::shared_ptr<Mesh> meshptr)
{
	mesh = meshptr;

	SetUpDescriptorBindings();
	CreateShaderPipeLine();

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = VulkanPtr::GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(VulkanPtr::GetDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

ComputeAnimationPipeline::~ComputeAnimationPipeline()
{
}
	
void ComputeAnimationPipeline::SetUpDescriptorBindings()
{
	VertexBufferCopy = std::make_shared<VulkanBuffer>(mesh->VertexBuffer);

	VkDescriptorBufferInfo VertexBufferInfo = AddBufferDescriptor(mesh->VertexBuffer);
	VkDescriptorBufferInfo BoneWeightBufferInfo = AddBufferDescriptor(mesh->BoneWeightBuffer);
	VkDescriptorBufferInfo MeshDataBufferInfo = AddBufferDescriptor(mesh->MeshProperties.VulkanBufferData);
	VkDescriptorBufferInfo BoneTransformBufferInfo = AddBufferDescriptor(mesh->BoneTransformBuffer);
	VkDescriptorBufferInfo TransformDataBufferInfo = AddBufferDescriptor(mesh->TransformBuffer);

	AddStorageBufferDescriptorSetBinding(0, VertexBufferInfo, VK_SHADER_STAGE_COMPUTE_BIT);
	AddStorageBufferDescriptorSetBinding(1, BoneWeightBufferInfo, VK_SHADER_STAGE_COMPUTE_BIT);
	AddStorageBufferDescriptorSetBinding(2, MeshDataBufferInfo, VK_SHADER_STAGE_COMPUTE_BIT);
	AddStorageBufferDescriptorSetBinding(3, BoneTransformBufferInfo, VK_SHADER_STAGE_COMPUTE_BIT);
	AddStorageBufferDescriptorSetBinding(4, TransformDataBufferInfo, VK_SHADER_STAGE_COMPUTE_BIT);
	SubmitDescriptorSet();
}

void ComputeAnimationPipeline::CreateShaderPipeLine()
{
	auto ComputeShaderCode = CreateShader("Shaders/animate.spv", VK_SHADER_STAGE_COMPUTE_BIT);

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(ConstMeshInfo);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	vkCreatePipelineLayout(VulkanPtr::GetDevice(), &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout);

	VkPipelineCacheCreateInfo pipelineCacheInfo{};
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(VulkanPtr::GetDevice(), &pipelineCacheInfo, nullptr, &PipelineCache);

	VkComputePipelineCreateInfo ComputePipelineInfo{};
	ComputePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	ComputePipelineInfo.layout = ShaderPipelineLayout;
	ComputePipelineInfo.flags = 0;
	ComputePipelineInfo.stage = ComputeShaderCode;

	if (vkCreateComputePipelines(VulkanPtr::GetDevice(), PipelineCache, 1, &ComputePipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}

	vkDestroyShaderModule(VulkanPtr::GetDevice(), ComputeShaderCode.module, nullptr);
}

void ComputeAnimationPipeline::Compute()
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
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &BufferMemoryBarrier, 0, nullptr);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ShaderPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ShaderPipelineLayout, 0, 1, &DescriptorSet, 0, 0);
	vkCmdPushConstants(commandBuffer, ShaderPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ConstMeshInfo), &meshInfo);
	vkCmdDispatch(commandBuffer, VertexBufferCopy->BufferSize, 1, 1);

	BufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	BufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	BufferMemoryBarrier.buffer = VertexBufferCopy->Buffer;
	BufferMemoryBarrier.size = VK_WHOLE_SIZE;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &BufferMemoryBarrier, 0, nullptr);

	mesh->VertexBuffer.CopyBufferToMemory(&mesh->VertexList[0], sizeof(Vertex) * mesh->VertexList.size());
	vkEndCommandBuffer(commandBuffer);
}