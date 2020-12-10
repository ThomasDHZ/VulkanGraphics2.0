#pragma once
#include "TextureRenderer.h"

class TextureRenderPass
{
private:
	TextureRenderer textureRenderer;

	void RenderTexturePass(VulkanEngine& engine);

public:
	TextureRenderPass();
	TextureRenderPass(VulkanEngine& engine);
	~TextureRenderPass();

	void Draw(VulkanEngine& engine);
	void UpdateSwapChain(VulkanEngine& engine);
	void Destory(VulkanEngine& engine);

	std::shared_ptr<Texture> GetRenderedTextureImage() { return textureRenderer.ColorTexture; }
	VkDescriptorSet GetIMGuiImage() { return textureRenderer.ColorTexture->ImGuiDescriptorSet; }
};