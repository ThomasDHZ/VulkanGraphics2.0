#include "DepthPassRendererPass.h"
#include "GraphicsPipeline.h"

DepthPassRendererPass::DepthPassRendererPass() : BaseRenderPass()
{
}

DepthPassRendererPass::DepthPassRendererPass(uint32_t depthTextureSize) : BaseRenderPass()
{
    RenderPassResolution = glm::ivec2(depthTextureSize, depthTextureSize);
    DepthToTexture = std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(RenderPassResolution));
    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->DirectionalLightList.size(); x++)
    {
        DepthTextureList.emplace_back(std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(RenderPassResolution)));
        DepthTextureList[x]->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    DepthToTexture->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    CreateRenderPass();
    CreateRendererFramebuffers();
    depthPipeline = std::make_shared<DepthPassPipeline>(DepthPassPipeline(RenderPass));
    SetUpCommandBuffers();
}

DepthPassRendererPass::~DepthPassRendererPass()
{
}

void DepthPassRendererPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription DepthAttachment = {};
    DepthAttachment.format = VK_FORMAT_D32_SFLOAT;
    DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(DepthAttachment);

    VkAttachmentReference depthReference = { 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    std::vector<VkSubpassDependency> DependencyList;

    VkSubpassDependency FirstDependency = {};
    FirstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    FirstDependency.dstSubpass = 0;
    FirstDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    FirstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    FirstDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    FirstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    FirstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    DependencyList.emplace_back(FirstDependency);

    VkSubpassDependency SecondDependency = {};
    SecondDependency.srcSubpass = 0;
    SecondDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
    SecondDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    SecondDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    SecondDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    SecondDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    SecondDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    DependencyList.emplace_back(SecondDependency);

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptionList.size());
    renderPassInfo.pAttachments = AttachmentDescriptionList.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(DependencyList.size());
    renderPassInfo.pDependencies = DependencyList.data();

    if (vkCreateRenderPass(EnginePtr::GetEnginePtr()->Device, &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void DepthPassRendererPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(DepthToTexture->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = RenderPassResolution.x;
        frameBufferCreateInfo.height = RenderPassResolution.y;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(EnginePtr::GetEnginePtr()->Device, &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void DepthPassRendererPass::SetUpCommandBuffers()
{
    CommandBuffer.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());
    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = EnginePtr::GetEnginePtr()->CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(EnginePtr::GetEnginePtr()->Device, &allocInfo, &CommandBuffer[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
}

void DepthPassRendererPass::RebuildSwapChain(uint32_t depthTextureSize)
{
    RenderPassResolution = glm::ivec2(depthTextureSize, depthTextureSize); 
    DepthToTexture->RecreateRendererTexture(RenderPassResolution);

    DepthTextureList.clear();
    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->DirectionalLightList.size(); x++)
    {
        DepthTextureList.emplace_back(std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(RenderPassResolution)));
        DepthTextureList[x]->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    depthPipeline->Destroy();

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    depthPipeline->UpdateGraphicsPipeLine(RenderPass);
    SetUpCommandBuffers();
}

void DepthPassRendererPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->ImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent.width = RenderPassResolution.x;
    renderPassInfo.renderArea.extent.height = RenderPassResolution.y;

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    VkRect2D rect2D{};
    rect2D.extent.width = RenderPassResolution.x;
    rect2D.extent.height = RenderPassResolution.y;
    rect2D.offset.x = 0.0f;
    rect2D.offset.y = 0.0f;

    VkViewport viewport{};
    viewport.width = (float)RenderPassResolution.x;
    viewport.height = (float)RenderPassResolution.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkImageSubresourceRange ImageSubresourceRange{};
    ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    ImageSubresourceRange.baseMipLevel = 0;
    ImageSubresourceRange.levelCount = 1;
    ImageSubresourceRange.layerCount = 1;

    for (int x = 0; x < DepthTextureList.size(); x++)
    {
        vkCmdSetViewport(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &viewport);
        vkCmdSetScissor(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &rect2D);
        vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthPipeline->ShaderPipelineLayout, 0, 1, &depthPipeline->DescriptorSet, 0, nullptr);
        AssetManagerPtr::GetAssetPtr()->Draw(RendererType, CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], depthPipeline->ShaderPipelineLayout, LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->lightViewCamera);
        vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

        DepthTextureList[x]->UpdateDepthImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        DepthToTexture->UpdateDepthImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        DepthToTexture->CopyDepthTexture(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], DepthTextureList[x]);
        DepthToTexture->UpdateDepthImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        DepthTextureList[x]->UpdateDepthImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void DepthPassRendererPass::Destroy()
{
    DepthToTexture->Delete();
    for (int x = 0; x < DepthTextureList.size(); x++)
    {
        DepthTextureList[x]->Delete();
    }
    depthPipeline->Destroy();
    BaseRenderPass::Destroy();
}