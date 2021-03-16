#pragma once
#include "VulkanEngine.h"
#include "FrameBufferRenderPass.h"
#include "InterfaceRenderPass.h"
#include "MainRenderPass.h"
#include "TextureManager.h"
#include "Model.h"
#include "RayTraceRenderer.h"
#include "ModelRenderManager.h"
#include "ComputeHelper.h"
#include "OrthographicCamera.h"


class Renderer
{
private:
	std::shared_ptr<PerspectiveCamera> camera;
	Keyboard keyboard;
	Mouse mouse;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;

	//FrameBufferRenderPass frameBufferRenderPass;
	InterfaceRenderPass interfaceRenderPass;
	MainRenderPass RenderPass;
	RayTraceRenderer RayRenderer;
	ComputeHelper AnimationRenderer;

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

public:
	ModelRenderManager modelRenderManager;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window);
	~Renderer();

	std::vector<Vertex> CalcVertex();
	void UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window);
	void Destroy(VulkanEngine& engine);
};