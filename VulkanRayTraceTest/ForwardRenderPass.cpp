#include "ForwardRenderPass.h"
#include "GraphicsPipeline.h"

ForwardRenderPass::ForwardRenderPass() : BaseRenderPass()
{
}

ForwardRenderPass::ForwardRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager) : BaseRenderPass()
{
    DepthTexture = std::make_shared<RenderedDepthTexture>(engine);

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    DebugLightPipeline = std::make_shared<DebugLightRenderingPipeline>(DebugLightRenderingPipeline(engine, assetManager, RenderPass));
    forwardRenderingPipeline = std::make_shared<ForwardRenderingPipeline>(ForwardRenderingPipeline(engine, assetManager, RenderPass));
    pbrRenderingPipeline = std::make_shared<PBRPipeline>(PBRPipeline(engine, assetManager, RenderPass));
    skyBoxRenderingPipeline = std::make_shared<SkyBoxRenderingPipeline>(SkyBoxRenderingPipeline(engine, assetManager, RenderPass));

    SetUpCommandBuffers(engine);
}

ForwardRenderPass::~ForwardRenderPass()
{
}


void ForwardRenderPass::CreateRenderPass(VulkanEngine& engine)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void ForwardRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
{
    SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++) {
        std::array<VkImageView, 2> attachments = {
            engine.SwapChain.GetSwapChainImageViews()[i],
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

void ForwardRenderPass::SetUpCommandBuffers(VulkanEngine& engine)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void ForwardRenderPass::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager)
{
    DepthTexture->RecreateRendererTexture(engine);
    DebugLightPipeline->Destroy(engine);
    forwardRenderingPipeline->Destroy(engine);
    pbrRenderingPipeline->Destroy(engine);
    skyBoxRenderingPipeline->Destroy(engine);

    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    DebugLightPipeline->UpdateGraphicsPipeLine(engine, assetManager, RenderPass);
    forwardRenderingPipeline->UpdateGraphicsPipeLine(engine, assetManager, RenderPass);
    pbrRenderingPipeline->UpdateGraphicsPipeLine(engine, assetManager, RenderPass);
    skyBoxRenderingPipeline->UpdateGraphicsPipeLine(engine, assetManager, RenderPass);
    SetUpCommandBuffers(engine);
}

void ForwardRenderPass::Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, RendererID rendererID)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(CommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.SwapChainResolution;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    if (rendererID == RendererID::PBR_Raster_Renderer)
    {
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pbrRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pbrRenderingPipeline->ShaderPipelineLayout, 0, 1, &pbrRenderingPipeline->DescriptorSets, 0, nullptr);
        assetManager->Draw(CommandBuffer, renderPassInfo, pbrRenderingPipeline->ShaderPipelineLayout, rendererPassID, assetManager->cameraManager.ActiveCamera);

        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipelineLayout, 0, 1, &skyBoxRenderingPipeline->DescriptorSets, 0, nullptr);
        static_cast<Skybox*>(assetManager->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer, renderPassInfo, rendererPassID);
    }
    else if (rendererID == RendererID::BlinnPhong_Raster_Renderer)
    {
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, forwardRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, forwardRenderingPipeline->ShaderPipelineLayout, 0, 1, &forwardRenderingPipeline->DescriptorSets, 0, nullptr);
        assetManager->Draw(CommandBuffer, renderPassInfo, forwardRenderingPipeline->ShaderPipelineLayout, rendererPassID, assetManager->cameraManager.ActiveCamera);

        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipelineLayout, 0, 1, &skyBoxRenderingPipeline->DescriptorSets, 0, nullptr);
        static_cast<Skybox*>(assetManager->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer, renderPassInfo, rendererPassID);
    }

    vkCmdEndRenderPass(CommandBuffer);
    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}

void ForwardRenderPass::Destroy(VulkanEngine& engine)
{
    DepthTexture->Delete(engine);
    DebugLightPipeline->Destroy(engine);
    forwardRenderingPipeline->Destroy(engine);
    pbrRenderingPipeline->Destroy(engine);
    skyBoxRenderingPipeline->Destroy(engine);
    BaseRenderPass::Destroy(engine);
}

