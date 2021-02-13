#pragma once
#include "VulkanEngine.h"
#include "FrameBufferRenderPass.h"
#include "InterfaceRenderPass.h"
#include "MainRenderPass.h"
#include "TextureManager.h"
#include "RayTraceModel.h"
#include "RayTraceRenderer.h"

class Renderer
{
private:
	std::vector<VkCommandBuffer> commandBuffers;

	

	size_t currentFrame = 0;

	bool RayTraceSwitch = false;
	bool framebufferResized = false;

public:
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine, TextureManager& textureManager);
	void SetUpDescriptorSets(VulkanEngine& engine, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList, VulkanBuffer& MaterialBuffer, std::shared_ptr<SceneDataStruct>& SceneData);

	FrameBufferRenderPass frameBufferRenderPass;
	InterfaceRenderPass interfaceRenderPass;
	MainRenderPass RenderPass;
	RayTraceRenderer RayRenderer;

	std::shared_ptr<RenderedRayTracedColorTexture> storageImage;
	std::shared_ptr<RenderedRayTracedColorTexture> shadowStorageImage;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList);
	~Renderer();
	void Draw(VulkanEngine& engine);
};

