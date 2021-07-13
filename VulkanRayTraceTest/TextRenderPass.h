#pragma once
#include "FrameBufferPipeline.h"
#include "TextRenderPipeline.h"

class TextRenderPass
{
private:
	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

public:
	TextRenderPass();
	TextRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<TextTexture> FontTexture);
	~TextRenderPass();

	static constexpr RenderPassID rendererPassID = RenderPassID::GUI_Renderer;

	std::shared_ptr<RenderedColorTexture> RenderedTexture;
	std::shared_ptr<TextRenderPipeline> TextPipeline;

	VkRenderPass RenderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

	void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<TextTexture> FontTexture);
	void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, RendererID rendererID);
	void Destroy(VulkanEngine& engine);
};