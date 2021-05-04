#pragma once
#include "VulkanEngine.h"
#include "FrameBufferRenderPass.h"
#include "InterfaceRenderPass.h"
#include "ForwardRenderPass.h"
#include "TextureManager.h"
#include "Model.h"
#include "RayTraceRenderPass.h"
#include "OrthographicCamera.h"
#include "AnimatorCompute.h"
#include "AssetManager.h"
#include "Skybox.h"
#include "DeferredRenderPass.h"
#include "TextureRenderPass.h"
#include "WaterSurfaceMesh.h"
#include "CubeMapRenderPass.h"
#include "PrefilterRenderPass.h"
#include "PBRRenderer.h"
#include "BlinnPhongRasterRenderer.h"

class Renderer
{
private:
	int imageview = 0;

	BlinnPhongRasterRenderer blinnPhongRenderer;
	//PBRRenderer pbrRenderer;
	RayTraceRenderPass RayRenderer;
	InterfaceRenderPass interfaceRenderPass;

	size_t currentFrame = 0;

	bool RayTraceSwitch = true;
	bool UpdateRenderer = false;
	bool framebufferResized = false;

	Skybox skybox;
	std::vector<VkCommandBuffer> CommandBufferSubmitList;

public:
	std::shared_ptr<AssetManager> assetManager;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPTR);
	~Renderer();

	void RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window);
	void Destroy(VulkanEngine& engine);
};