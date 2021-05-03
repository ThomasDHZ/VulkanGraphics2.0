#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "CubeMapRenderPass.h"
#include "PrefilterRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	ForwardRenderPass forwardRenderPass;
	CubeMapRenderPass cubeMapRenderer;
	PrefilterRenderPass prefilterRenderPass;
public:
	PBRRenderer();
	PBRRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~PBRRenderer();

	void UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window);
	void Destroy(VulkanEngine& engine);;
};

