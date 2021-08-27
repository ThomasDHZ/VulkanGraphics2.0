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

	void CreateRenderPass();
	void CreateRendererFramebuffers();

public:
	InterfaceRenderPass();
	InterfaceRenderPass(std::shared_ptr<VulkanEngine> engine);
	~InterfaceRenderPass();

	VkCommandBuffer ImGuiCommandBuffers;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	void Draw(int currentFrame, int drawframe);
	void RebuildSwapChain();
	void Destroy();

	VkRenderPass GetRenderPass() { return RenderPass; }
};
