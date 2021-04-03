#pragma once
#include "VulkanEngine.h"
#include "ForwardRenderingPipeline.h"
#include "RenderedDepthTexture.h"
#include "RenderedColorTexture.h"
class GBufferRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void CreatePipeline(VulkanEngine& engine);
public:
	GBufferRenderPass();
	GBufferRenderPass(VulkanEngine& engine);
	~GBufferRenderPass();

	VkRenderPass RenderPass;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	std::shared_ptr<RenderedColorTexture> GPositionTexture;
	std::shared_ptr<RenderedColorTexture> GAlbedoTexture;
	std::shared_ptr<RenderedColorTexture> GNormalTexture;
	std::shared_ptr<RenderedColorTexture> GBloomTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;
	std::shared_ptr<ForwardRenderingPipeline> forwardRendereringPipeline;

	void StartPipeline(VulkanEngine& engine, VkDescriptorSetLayout& DescriptorLayout);
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
