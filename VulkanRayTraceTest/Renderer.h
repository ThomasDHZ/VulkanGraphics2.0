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

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSets;
	VkCommandBuffer RasterCommandBuffer;

	//FrameBufferRenderPass frameBufferRenderPass;
	InterfaceRenderPass interfaceRenderPass;
	ForwardRenderPass RenderPass;
	//RayTraceRenderer RayRenderer;
	//AnimatorCompute AnimationRenderer;

	size_t currentFrame = 0;

	bool RayTraceSwitch = true;
	bool UpdateRenderer = false;
	bool framebufferResized = false;

	std::shared_ptr<SceneDataUniformBuffer> SceneData;
	std::vector<Model> RenderModel;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine);
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