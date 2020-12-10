#include "BloomRenderPass.h"

BloomRenderPass::BloomRenderPass()
{
}

BloomRenderPass::BloomRenderPass(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Texture> FrameBufferImage)
{
	BloomRenderer = RenderToTextureRenderPass(engine);
	BloomRenderer2 = RenderToTextureRenderPass(engine);

	BloomFrameBuffer = FrameBufferMesh(engine, textureManager, FrameBufferImage, BloomRenderer.bloomPipeline->ShaderPipelineDescriptorLayout);
	BloomFrameBuffer2 = FrameBufferMesh(engine, textureManager, FrameBufferImage, BloomRenderer.bloomPipeline->ShaderPipelineDescriptorLayout);
}

BloomRenderPass::~BloomRenderPass()
{
}

void BloomRenderPass::Update(VulkanEngine& engine)
{
	BloomFrameBuffer.Update(engine);
	BloomFrameBuffer2.Update(engine);
}

void BloomRenderPass::Draw(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex)
{
	BloomRenderer.Render(engine, commandBuffers, SwapBufferImageIndex, BloomFrameBuffer);
	BloomRenderer2.Render(engine, commandBuffers, SwapBufferImageIndex, BloomFrameBuffer2);
}

void BloomRenderPass::UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Texture> FrameBufferImage)
{
	BloomRenderer.UpdateSwapChain(engine);
	BloomRenderer2.UpdateSwapChain(engine);

	//BloomFrameBuffer.ScreenResizeUpdate(engine, textureManager, FrameBufferImage, FrameBufferImage, 5, BloomRenderer.bloomPipeline);
	//BloomFrameBuffer2.ScreenResizeUpdate(engine, textureManager, BloomRenderer.ColorTexture, BloomRenderer.ColorTexture, 6, BloomRenderer.bloomPipeline2nd);
}

void BloomRenderPass::Destory(VulkanEngine& engine)
{
	BloomRenderer.Destroy(engine);
	BloomRenderer2.Destroy(engine);

	BloomFrameBuffer.Destory(engine);
	BloomFrameBuffer2.Destory(engine);
}