#pragma once
#include "BaseRenderer.h"
#include "ForwardRenderPass.h"
#include "CubeMapRenderPass.h"
#include "PrefilterRenderPass.h"

class PBRRenderer : public BaseRenderer
{
private:
	static constexpr RendererID rendererID = RendererID::PBR_Raster_Renderer;
	CubeMapRenderPass cubeMapRenderer;
	PrefilterRenderPass prefilterRenderPass;
public:
	PBRRenderer();
	PBRRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr);
	~PBRRenderer();


	ForwardRenderPass forwardRenderPass;
	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex, Skybox skybox);
	void Destroy(VulkanEngine& engine);

	 std::vector<VkCommandBuffer> AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList);
};

