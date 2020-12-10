#include "TextureRenderPass.h"

TextureRenderPass::TextureRenderPass()
{
}

TextureRenderPass::TextureRenderPass(VulkanEngine& engine)
{
	textureRenderer = TextureRenderer(engine);
}

TextureRenderPass::~TextureRenderPass()
{
}

void TextureRenderPass::Draw(VulkanEngine& engine)
{
	RenderTexturePass(engine);
}

void TextureRenderPass::UpdateSwapChain(VulkanEngine& engine)
{
	textureRenderer.UpdateSwapChain(engine);
}

void TextureRenderPass::Destory(VulkanEngine& engine)
{
	textureRenderer.Destroy(engine);
}

void TextureRenderPass::RenderTexturePass(VulkanEngine& engine)
{
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = textureRenderer.GetRenderPass();
	renderPassInfo.framebuffer = textureRenderer.SwapChainFramebuffers[engine.DrawFrame];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	//vkCmdBeginRenderPass(engine.RenderCommandBuffer[engine.DrawFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	// textureRenderer.D
	//vkCmdEndRenderPass(engine.RenderCommandBuffer[engine.DrawFrame]);
}