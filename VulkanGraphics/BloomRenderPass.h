#pragma once
#include "TextureRenderer.h"
#include "Mesh2D.h"
#include "RenderedColorTexture.h"
class BloomRenderPass
{
private:
	Mesh2D mesh;
	TextureRenderer textureRenderer;

public:
	BloomRenderPass();
	BloomRenderPass(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<GraphicsPipeline> Shader);
	~BloomRenderPass();

	void Render(VulkanEngine& engine);
	std::shared_ptr<RenderedColorTexture> GetColorTexture() { return textureRenderer.ColorTexture; };
};