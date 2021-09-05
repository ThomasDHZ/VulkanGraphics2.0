#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan_core.h>

enum RendererType
{
	RT_ForwardRenderer,
	RT_SceneRenderer,
	RT_TextureRenderer,
	RT_ShadowRenderer
};

struct DescriptorSetLayoutBindingInfo
{
	uint32_t Binding;
	VkDescriptorType DescriptorType;
	VkShaderStageFlags StageFlags;
};

class GraphicsPipeline
{
protected:
	std::vector<char> ReadShaderFile(const std::string& filename);
	VkShaderModule CreateShaderModule(VkDevice renderer, const std::vector<char>& code);

	void CreateDescriptorSetLayout(VkDevice renderer, std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo);
	void CreatePipeLineLayout(VkDevice renderer, VkPipelineLayoutCreateInfo PipelineLayoutInfo);
	void CreatePipeLine(VkDevice renderer, VkGraphicsPipelineCreateInfo PipeLineInfo);

public:
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;
	VkDescriptorSetLayout ShaderPipelineDescriptorLayout;

	GraphicsPipeline();
	~GraphicsPipeline();

	void UpdateSwapChain(VkDevice renderer);
	void Destroy(VkDevice renderer);
};
