#pragma once
#include <array>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_impl_glfw.h"
#include <vector>

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

	void CreateRenderPass(VkDevice device);
	void CreateRendererFramebuffers(VkDevice device, std::vector<VkImageView> view, VkExtent2D rect);

public:
	InterfaceRenderPass();
	InterfaceRenderPass(VkDevice device, VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkQueue GraphicsQueue, GLFWwindow* window, std::vector<VkImageView> view, VkExtent2D rect);
	~InterfaceRenderPass();

	std::vector<VkCommandBuffer> ImGuiCommandBuffers;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	void Draw(VkDevice device, int frame, VkExtent2D rect);
	void UpdateSwapChain(VkDevice& device, std::vector<VkImageView>& view, VkExtent2D& rect);
	void Destroy(VkDevice device);

	VkRenderPass GetRenderPass() { return RenderPass; }
};

