#pragma once
#include "VulkanEngine.h"
#include "FrameBufferRenderPass.h"
#include "InterfaceRenderPass.h"
#include "ForwardRenderPass.h"
#include "TextureManager.h"
#include "Model.h"
#include "RayTraceRenderer.h"
#include "OrthographicCamera.h"
#include "AnimatorCompute.h"
#include "AssetManager.h"
#include "Skybox.h"
#include "GBufferRenderPass.h"


class Renderer
{
private:
	bool AddTextureFlag = false;
	bool RemoveTextureFlag = false;
	bool AddMaterialFlag = false;
	bool RemoveMaterialFlag = false;

	std::shared_ptr<PerspectiveCamera> camera;
	Keyboard keyboard;
	Mouse mouse;

	VkCommandBuffer RasterCommandBuffer;

	FrameBufferRenderPass frameBufferRenderPass;
	InterfaceRenderPass interfaceRenderPass;
	ForwardRenderPass RenderPass;
	GBufferRenderPass gBufferRenderPass;
	RayTraceRenderer RayRenderer;

	//AnimatorCompute AnimationRenderer;

	size_t currentFrame = 0;

	bool RayTraceSwitch = true;
	bool UpdateRenderer = false;
	bool framebufferResized = false;

	std::shared_ptr<SceneDataUniformBuffer> SceneData;
	std::vector<Model> RenderModel;

	void SetUpCommandBuffers(VulkanEngine& engine);

	//Skybox skybox;

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