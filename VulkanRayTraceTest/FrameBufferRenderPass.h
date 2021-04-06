#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"

class FrameBufferRenderPass
{
private:
	VkDescriptorSetLayout DescriptorLayout;
	VkDescriptorPool DescriptorPool;
	VkDescriptorSet DescriptorSets;
	VkRenderPass RenderPass;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void CreateDescriptorSetLayout(VulkanEngine& engine, AssetManager& assetManager);
	void CreateShaderPipeLine(VulkanEngine& engine);
	void CreateDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void CreateDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);

	void UpdateGraphicsPipeLine(VulkanEngine& engine);

public:
	FrameBufferRenderPass();
	FrameBufferRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);
	~FrameBufferRenderPass();

	std::vector<VkFramebuffer> SwapChainFramebuffers;

	void Draw(VulkanEngine& engine, VkCommandBuffer commandbuffer, uint32_t index);
	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};