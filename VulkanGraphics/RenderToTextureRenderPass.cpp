#include "RenderToTextureRenderPass.h"
#include "ImGui/imgui_impl_vulkan.h"

RenderToTextureRenderPass::RenderToTextureRenderPass()
{
}

RenderToTextureRenderPass::RenderToTextureRenderPass(VulkanEngine& engine)
{
    CreateRenderPass(engine);
    ColorTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(engine));
    DepthTexture = std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(engine));
    CreateRendererFramebuffers(engine);

    bloomPipeline = std::make_shared<BloomPipeline>(engine, RenderPass);
    bloomPipeline2nd = std::make_shared<BloomPipeline2nd>(engine, RenderPass);

    ImGui_ImplVulkan_AddTexture(ColorTexture->ImGuiDescriptorSet, ColorTexture->GetTextureSampler(), ColorTexture->GetTextureView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

RenderToTextureRenderPass::~RenderToTextureRenderPass()
{
}

void RenderToTextureRenderPass::CreateRenderPass(VulkanEngine& engine)
{
    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};

    attchmentDescriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attchmentDescriptions[1].format = VK_FORMAT_D32_SFLOAT;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create vkCreateImageView!");
    }
}

void RenderToTextureRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
{
    SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageViews().size());
    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageViews().size(); i++) {
        std::array<VkImageView, 2> attachments = {
            ColorTexture->GetTextureView(),
            DepthTexture->GetTextureView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = engine.SwapChain.GetSwapChainResolution().width;
        framebufferInfo.height = engine.SwapChain.GetSwapChainResolution().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(engine.Device, &framebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void RenderToTextureRenderPass::Render(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex, FrameBufferMesh& TextureRender)
{
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[SwapBufferImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    TextureRender.Draw(commandBuffers[SwapBufferImageIndex], bloomPipeline, SwapBufferImageIndex);
    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
}

void RenderToTextureRenderPass::Render2(VulkanEngine& engine, std::vector<VkCommandBuffer>& commandBuffers, int SwapBufferImageIndex, FrameBufferMesh& TextureRender)
{
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[SwapBufferImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    TextureRender.Draw(commandBuffers[SwapBufferImageIndex], bloomPipeline2nd, SwapBufferImageIndex);
    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
}


void RenderToTextureRenderPass::UpdateSwapChain(VulkanEngine& engine)
{
    ColorTexture->RecreateRendererTexture(engine);
    DepthTexture->RecreateRendererTexture(engine);

    bloomPipeline->UpdateGraphicsPipeLine(engine, RenderPass);
    bloomPipeline2nd->UpdateGraphicsPipeLine(engine, RenderPass);

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
    CreateRendererFramebuffers(engine);
}

void RenderToTextureRenderPass::Destroy(VulkanEngine& engine)
{
    bloomPipeline->Destroy(engine);
    bloomPipeline2nd->Destroy(engine);

    ColorTexture->Delete(engine);
    DepthTexture->Delete(engine);

    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}
