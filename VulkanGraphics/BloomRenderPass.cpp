#include "BloomRenderPass.h"

BloomRenderPass::BloomRenderPass()
{
}

BloomRenderPass::BloomRenderPass(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<GraphicsPipeline> Shader)
{
	/*const std::vector<Vertex> TextureVertices =
	{
		{{0.0f, 0.0f, -0.1f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{1.0f, 0.0f, -0.1f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{1.0f, 1.0f, -0.1f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{0.0f, 1.0f, -0.1f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
	};

	const std::vector<uint16_t> TextureIndices =
	{
		  0, 1, 2, 2, 3, 0
	};

	MeshTextures RendererTextures = {};
	RendererTextures.RendererDiffuseMap = FrameBufferImage;

	mesh = Mesh2D(renderer, textureManager, TextureVertices, TextureIndices, RendererTextures, Shader);
	RenderMessage = mesh.DrawMessage;
	textureRenderer = TextureRenderer(renderer);*/
}

BloomRenderPass::~BloomRenderPass()
{
}

void BloomRenderPass::Render(VulkanEngine& engine)
{
	/*std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = textureRenderer.RenderPass;
	renderPassInfo.framebuffer = textureRenderer.SwapChainFramebuffers[renderer.DrawFrame];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = renderer.SwapChain.GetSwapChainResolution();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(renderer.RenderCommandBuffer[renderer.DrawFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	textureRenderer.Draw(*renderer.GetVulkanRendererBase(), RenderMessage);
	vkCmdEndRenderPass(renderer.RenderCommandBuffer[renderer.DrawFrame]);*/
}