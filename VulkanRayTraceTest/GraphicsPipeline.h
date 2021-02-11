#pragma once
#include "VulkanEngine.h"

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

struct MeshInfo
{
	int MeshID;
	int MaterialID;
};


class GraphicsPipeline
{
protected:
	std::vector<char> ReadShaderFile(const std::string& filename);
	VkShaderModule CreateShaderModule(VulkanEngine& renderer, const std::vector<char>& code);

	void CreatePipeLineLayout(VulkanEngine& renderer, VkPipelineLayoutCreateInfo PipelineLayoutInfo);
	void CreatePipeLine(VulkanEngine& renderer, VkGraphicsPipelineCreateInfo PipeLineInfo);

public:
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;

	GraphicsPipeline();
	GraphicsPipeline(VulkanEngine& renderer);
	~GraphicsPipeline();

	void UpdateSwapChain(VulkanEngine& renderer);
	void Destroy(VulkanEngine& renderer);
};