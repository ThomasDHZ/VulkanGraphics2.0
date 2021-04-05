#pragma once
#include "VulkanEngine.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedGBufferAlbedoTexture.h"
#include "RenderedGBufferNormalTexture.h"
#include "RenderedGBufferPositionTexture.h"
#include "AssetManager.h"

class GBufferRenderPass
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
	void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void SetUpShaderPipeLine(VulkanEngine& renderer);
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
public:
	GBufferRenderPass();
	GBufferRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	~GBufferRenderPass();

	VkRenderPass RenderPass;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedGBufferPositionTexture> GPositionTexture;
	std::shared_ptr<RenderedGBufferAlbedoTexture> GAlbedoTexture;
	std::shared_ptr<RenderedGBufferNormalTexture> GNormalTexture;
	std::shared_ptr<RenderedColorTexture> GBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;

	void UpdateSwapChain(VulkanEngine& engine, VkDescriptorSetLayout& DescriptorLayout);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};

//class GBufferPipeline : public GraphicsPipeline
//{
//private:
//	void CreateDescriptorSetLayout(VulkanEngine& renderer);
//	void CreateShaderPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
//public:
//	GBufferPipeline();
//	GBufferPipeline(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
//	~GBufferPipeline();
//
//	void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass, int PipelineBitFlags);
//};
//
