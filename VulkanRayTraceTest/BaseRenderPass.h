#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "Skybox.h"

class BaseRenderPass
{
private:
protected:
	//virtual void CreateRenderPass(VulkanEngine& engine)= 0;
	//virtual void CreateRendererFramebuffers(VulkanEngine& engine) = 0;
	//virtual void SetUpCommandBuffers(VulkanEngine& engine) = 0;
public:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	BaseRenderPass();
	~BaseRenderPass();

	//virtual void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData) = 0;
	//virtual void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer, Skybox& skybox) = 0;
	//virtual void Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, VkCommandBuffer commandBuffer) = 0;
	virtual void Destroy(VulkanEngine& engine);
};

