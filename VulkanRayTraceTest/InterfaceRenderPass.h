#pragma once
#include "Vulkanengine.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_impl_glfw.h"
#include "GlobalPtr.h"

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
	InterfaceRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window);
	~InterfaceRenderPass();

	RenderPassID RendererID = Interface_Renderer;
	VkCommandBuffer ImGuiCommandBuffers;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	void Draw(int frame);
	void RebuildSwapChain();
	void Destroy();

	VkRenderPass GetRenderPass() { return RenderPass; }
};

