#pragma once
#include "FrameBufferMesh.h"
#include "RenderToTextureRenderPass.h"

class BloomRenderPass
{
private:
	RenderToTextureRenderPass BloomRenderer;
	RenderToTextureRenderPass BloomRenderer2;
	FrameBufferMesh BloomFrameBuffer;
	FrameBufferMesh BloomFrameBuffer2;

public:
	BloomRenderPass();
	BloomRenderPass(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, std::shared_ptr<Texture> FrameBufferImage);
	~BloomRenderPass();

	void Update(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex);
	void UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Texture> FrameBufferImage);
	void Destory(VulkanEngine& engine);

	std::shared_ptr<Texture> OutputBloomImage() { return BloomRenderer2.ColorTexture; }
	VkDescriptorSet GetIMGuiImagePass1() { return BloomRenderer.ColorTexture->ImGuiDescriptorSet; }
	VkDescriptorSet GetIMGuiImagePass2() { return BloomRenderer2.ColorTexture->ImGuiDescriptorSet; }
};

