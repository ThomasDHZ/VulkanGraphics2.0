#include "BaseRenderPass.h"

BaseRenderPass::BaseRenderPass()
{
}

BaseRenderPass::~BaseRenderPass()
{
}

void BaseRenderPass::Destroy()
{
    vkDestroyRenderPass(GlobalPtr::enginePtr->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(GlobalPtr::enginePtr->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}
