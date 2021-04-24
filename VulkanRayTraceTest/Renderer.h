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

class Renderer
{
private:
	int imageview = 0;
	std::shared_ptr<PerspectiveCamera> camera;
	std::shared_ptr<PerspectiveCamera> camera2;
	Keyboard keyboard;
	Mouse mouse;

	VkCommandBuffer RasterCommandBuffer;

	//FrameBufferRenderPass frameBufferRenderPass;
	InterfaceRenderPass interfaceRenderPass;
	ForwardRenderPass forwardRenderPass;
	RayTraceRenderPass RayRenderer;
	//DeferredRenderPass gBufferRenderPass;
	//TextureRenderPass textureRenderPass;

	size_t currentFrame = 0;

	bool RayTraceSwitch = true;
	bool UpdateRenderer = false;
	bool framebufferResized = false;

	std::shared_ptr<SceneDataUniformBuffer> SceneData;
	std::vector<Model> RenderModel;

	void SetUpCommandBuffers(VulkanEngine& engine);

	Skybox skybox;
	std::vector<VkCommandBuffer> CommandBufferSubmitList;

public:
	AssetManager assetManager;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window);
	~Renderer();

	void UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window);
	void Destroy(VulkanEngine& engine);
};