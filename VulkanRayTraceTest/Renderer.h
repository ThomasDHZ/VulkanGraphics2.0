#pragma once
#include "VulkanEngine.h"
#include "FrameBufferRenderPass.h"
#include "InterfaceRenderPass.h"
#include "MainRenderPass.h"
#include "TextureManager.h"
#include "RayTraceModel.h"
#include "RayTraceRenderer.h"
#include "RayTraceModel.h"

class Renderer
{
private:


	std::shared_ptr<PerspectiveCamera> camera;
	Keyboard keyboard;
	Mouse mouse;

	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkCommandBuffer> commandBuffers;

	//FrameBufferRenderPass frameBufferRenderPass;
	InterfaceRenderPass interfaceRenderPass;
	MainRenderPass RenderPass;
	RayTraceRenderer RayRenderer;

	size_t currentFrame = 0;

	bool RayTraceSwitch = false;
	bool framebufferResized = false;

	VulkanBuffer MaterialBuffer;
	std::shared_ptr<SceneDataStruct> SceneData;
	std::vector<RayTraceModel> RenderModel;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine, TextureManager& textureManager);
	void SetUpDescriptorSets(VulkanEngine& engine, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList);
	void SetUpCommandBuffers(VulkanEngine& engine, std::vector<RayTraceModel>& ModelList);

public:
	TextureManager textureManager;
	std::vector<RayTraceModel> ModelList;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList);
	~Renderer();

	void AddModel(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath);
	void UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window, std::vector<RayTraceModel>& ModelList);
	void Destroy(VulkanEngine& engine);
};