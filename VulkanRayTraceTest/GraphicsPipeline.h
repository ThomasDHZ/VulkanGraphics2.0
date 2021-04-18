#pragma once
#include "VulkanEngine.h"

enum RendererType
{
	RT_ForwardRenderer,
	RT_SceneRenderer,
	RT_TextureRenderer,
	RT_ShadowRenderer
};

struct ConstMeshInfo
{
	alignas(4) uint32_t MeshIndex;
};


class GraphicsPipeline
{
protected:


public:
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;

	GraphicsPipeline();
	GraphicsPipeline(VulkanEngine& renderer);
	~GraphicsPipeline();

	virtual void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
	virtual void Destroy(VulkanEngine& engine);
};