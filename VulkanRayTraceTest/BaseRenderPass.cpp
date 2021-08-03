#include "BaseRenderPass.h"

BaseRenderPass::BaseRenderPass()
{
}

BaseRenderPass::~BaseRenderPass()
{
}

void BaseRenderPass::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    vkDestroyRenderPass(engine->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}
