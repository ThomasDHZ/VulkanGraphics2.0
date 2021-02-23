#pragma once
#include "VulkanEngine.h"
#include "FrameBufferRenderPass.h"
#include "InterfaceRenderPass.h"
#include "MainRenderPass.h"
#include "TextureManager.h"
#include "Model.h"
#include "RayTraceRenderer.h"
#include "ModelRenderManager.h"

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

	size_t currentFrame = 0;

	bool RayTraceSwitch = true;
	bool UpdateRenderer = false;
	bool framebufferResized = false;

	std::shared_ptr<SceneDataStruct> SceneData;
	std::vector<Model> RenderModel;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine);
	void SetUpCommandBuffers(VulkanEngine& engine);

	VkWriteDescriptorSetAccelerationStructureKHR AddAcclerationStructureBinding(VulkanEngine& engine, VkAccelerationStructureKHR& handle);
	VkDescriptorPoolSize AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType);
	VkDescriptorImageInfo AddRayTraceReturnImageDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout, StorageImage& texture);
	std::vector<VkDescriptorImageInfo> AddTextureDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout);
	VkDescriptorImageInfo AddTextureDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture);
	VkDescriptorBufferInfo AddStorageDescriptor(VulkanEngine& engine, VulkanBuffer buffer);
	std::vector<VkDescriptorBufferInfo> AddStorageDescriptor(VulkanEngine& engine, std::vector<VulkanBuffer> BufferList);
	VkDescriptorBufferInfo AddBufferDescriptor(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize);

	VkWriteDescriptorSet AddAccelerationBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure);
	VkWriteDescriptorSet AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
	VkWriteDescriptorSet AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList);
	VkWriteDescriptorSet AddStorageImageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& BufferInfo);
	VkWriteDescriptorSet AddDescriptorSetBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
	VkWriteDescriptorSet AddDescriptorSetTexture(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
	VkWriteDescriptorSet AddDescriptorSetTexture(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo);


public:
	ModelRenderManager modelRenderManager;

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