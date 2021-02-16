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

	VkDescriptorPool RayTracingDescriptorPool = VK_NULL_HANDLE;
	//VkDescriptorSetLayout RayTraceDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet RayTraceDescriptorSets = VK_NULL_HANDLE;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSets;

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

	void SetUpRayTracingDescriptorPool(VulkanEngine& engine);
	void SetUpRayTraceDescriptorSetLayout(VulkanEngine& engine);
	void SetUpRayTraceDescriptorSet(VulkanEngine& engine);

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

	VkDescriptorPoolSize AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType);
	std::vector<VkDescriptorImageInfo> AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout);
	VkDescriptorBufferInfo AddBufferDescriptorInfo(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize);
	VkWriteDescriptorSet AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
	VkWriteDescriptorSet AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
	VkWriteDescriptorSet AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
	VkWriteDescriptorSet AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo);

public:
	TextureManager textureManager;
	std::vector<RayTraceModel> ModelList;

	Renderer();
	Renderer(VulkanEngine& engine, VulkanWindow& window);
	~Renderer();

	void AddModel(VulkanEngine& engine, VulkanWindow& window, const std::string& FilePath);
	void UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window);
	void Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage);
	void GUIUpdate(VulkanEngine& engine);
	void Draw(VulkanEngine& engine, VulkanWindow& window);
	void Destroy(VulkanEngine& engine);
};