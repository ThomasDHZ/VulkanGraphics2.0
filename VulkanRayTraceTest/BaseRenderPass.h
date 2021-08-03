#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "Skybox.h"

class BaseRenderPass
{
private:
protected:
	//virtual void CreateRenderPass(std::shared_ptr<VulkanEngine> engine)= 0;
	//virtual void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine) = 0;
	//virtual void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine) = 0;
public:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	BaseRenderPass();
	~BaseRenderPass();

	//virtual void UpdateSwapChain(std::shared_ptr<VulkanEngine> engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData) = 0;
	//virtual void Draw(std::shared_ptr<VulkanEngine> engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer, Skybox& skybox) = 0;
	//virtual void Draw(std::shared_ptr<VulkanEngine> engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer) = 0;
	virtual void Destroy(std::shared_ptr<VulkanEngine> engine);
};

