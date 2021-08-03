#pragma once
#include "Vulkanengine.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_impl_glfw.h"

static void check_vk_result(VkResult err)
{
	if (err == 0) return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

class InterfaceRenderPass
{
private:
	VkRenderPass RenderPass;
	VkDescriptorPool ImGuiDescriptorPool;
	VkCommandPool ImGuiCommandPool;

	void CreateRenderPass(std::shared_ptr<VulkanEngine> engine);
	void CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine);

public:
	InterfaceRenderPass();
	InterfaceRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	~InterfaceRenderPass();

	RenderPassID RendererID = Interface_Renderer;
	VkCommandBuffer ImGuiCommandBuffers;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	void Draw(std::shared_ptr<VulkanEngine> engine, int frame);
	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine);
	void Destroy(std::shared_ptr<VulkanEngine> engine);

	VkRenderPass GetRenderPass() { return RenderPass; }
};

