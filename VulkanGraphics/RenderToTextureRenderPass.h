#pragma once
#include "VulkanEngine.h"
#include "BloomPipeline.h"
#include "BloomPipeline2nd.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "Object2D.h"
#include "FrameBufferMesh.h"
#include "DeferredFrameBufferMesh.h"

class RenderToTextureRenderPass
{
private:
	VkRenderPass RenderPass;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);

public:
	RenderToTextureRenderPass();
	RenderToTextureRenderPass(VulkanEngine& engine);
	~RenderToTextureRenderPass();

	std::vector<VkFramebuffer> SwapChainFramebuffers;

	std::shared_ptr<BloomPipeline> bloomPipeline;
	std::shared_ptr<BloomPipeline2nd> bloomPipeline2nd;

	std::shared_ptr<RenderedColorTexture> ColorTexture;
	std::shared_ptr<RenderedDepthTexture> DepthTexture;

	void Render(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex, FrameBufferMesh& TextureRender);
	void Render2(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex, FrameBufferMesh& TextureRender);
	void UpdateSwapChain(VulkanEngine& engine);
	void Destroy(VulkanEngine& engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};

