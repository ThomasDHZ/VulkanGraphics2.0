#include "BaseRenderPass.h"

BaseRenderPass::BaseRenderPass()
{
}

BaseRenderPass::BaseRenderPass(glm::ivec2 renderPassResolution)
{
    RenderPassResolution = renderPassResolution;
}

BaseRenderPass::~BaseRenderPass()
{
}

void BaseRenderPass::Destroy()
{
    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}
