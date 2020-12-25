#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline()
{

}


GraphicsPipeline::~GraphicsPipeline()
{
}

std::vector<char> GraphicsPipeline::ReadShaderFile(const std::string& filename)
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

VkShaderModule GraphicsPipeline::CreateShaderModule(VkDevice device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void GraphicsPipeline::CreateDescriptorSetLayout(VkDevice device, std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo)
{

	std::vector<VkDescriptorSetLayoutBinding> LayoutBindingList = {};

	for (auto Binding : LayoutBindingInfo)
	{
		VkDescriptorSetLayoutBinding LayoutBinding = {};
		LayoutBinding.binding = Binding.Binding;
		LayoutBinding.descriptorCount = 1;
		LayoutBinding.descriptorType = Binding.DescriptorType;
		LayoutBinding.pImmutableSamplers = nullptr;
		LayoutBinding.stageFlags = Binding.StageFlags;

		LayoutBindingList.emplace_back(LayoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(LayoutBindingList.size());
	layoutInfo.pBindings = LayoutBindingList.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &ShaderPipelineDescriptorLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void GraphicsPipeline::CreatePipeLineLayout(VkDevice device, VkPipelineLayoutCreateInfo PipelineLayoutInfo)
{
	if (vkCreatePipelineLayout(device, &PipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void GraphicsPipeline::CreatePipeLine(VkDevice device, VkGraphicsPipelineCreateInfo PipeLineInfo)
{
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &PipeLineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void GraphicsPipeline::UpdateSwapChain(VkDevice device)
{
	vkDestroyPipeline(device, ShaderPipeline, nullptr);
	vkDestroyPipelineLayout(device, ShaderPipelineLayout, nullptr);

	ShaderPipeline = VK_NULL_HANDLE;
	ShaderPipelineLayout = VK_NULL_HANDLE;
}

void GraphicsPipeline::Destroy(VkDevice device)
{
	vkDestroyPipeline(device, ShaderPipeline, nullptr);
	vkDestroyPipelineLayout(device, ShaderPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, ShaderPipelineDescriptorLayout, nullptr);

	ShaderPipeline = VK_NULL_HANDLE;
	ShaderPipelineLayout = VK_NULL_HANDLE;
	ShaderPipelineDescriptorLayout = VK_NULL_HANDLE;
}
