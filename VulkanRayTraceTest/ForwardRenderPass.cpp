#include "ForwardRenderPass.h"
#include "GraphicsPipeline.h"

ForwardRenderPass::ForwardRenderPass() : BaseRenderPass()
{
    DepthTexture = std::make_shared<RenderedDepthTexture>(EnginePtr::GetEnginePtr());

    CreateRenderPass();
    CreateRendererFramebuffers();
    DebugLightPipeline = std::make_shared<DebugLightRenderingPipeline>(DebugLightRenderingPipeline(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass));
    forwardRenderingPipeline = std::make_shared<ForwardRenderingPipeline>(ForwardRenderingPipeline(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass));
    pbrRenderingPipeline = std::make_shared<PBRPipeline>(PBRPipeline(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass));
    skyBoxRenderingPipeline = std::make_shared<SkyBoxRenderingPipeline>(SkyBoxRenderingPipeline(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass));

    SetUpCommandBuffers();
}

ForwardRenderPass::~ForwardRenderPass()
{
}


void ForwardRenderPass::CreateRenderPass()
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

    if (vkCreateRenderPass(EnginePtr::GetEnginePtr()->Device, &renderPassInfo, nullptr, &RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void ForwardRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++) {
        std::array<VkImageView, 2> attachments = {
            EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageViews()[i],
            DepthTexture->GetTextureView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
        framebufferInfo.height = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(EnginePtr::GetEnginePtr()->Device, &framebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void ForwardRenderPass::SetUpCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = EnginePtr::GetEnginePtr()->CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(EnginePtr::GetEnginePtr()->Device, &allocInfo, &CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void ForwardRenderPass::RebuildSwapChain()
{
    DepthTexture->RecreateRendererTexture();
    DebugLightPipeline->Destroy(EnginePtr::GetEnginePtr());
    forwardRenderingPipeline->Destroy(EnginePtr::GetEnginePtr());
    pbrRenderingPipeline->Destroy(EnginePtr::GetEnginePtr());
    skyBoxRenderingPipeline->Destroy(EnginePtr::GetEnginePtr());

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    DebugLightPipeline->UpdateGraphicsPipeLine(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass);
    forwardRenderingPipeline->UpdateGraphicsPipeLine(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass);
    pbrRenderingPipeline->UpdateGraphicsPipeLine(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass);
    skyBoxRenderingPipeline->UpdateGraphicsPipeLine(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass);
    SetUpCommandBuffers();
}

void ForwardRenderPass::Draw(RendererID rendererID)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(CommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->DrawFrame];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = EnginePtr::GetEnginePtr()->SwapChain.SwapChainResolution;

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
        AssetManagerPtr::GetAssetPtr()->Draw(CommandBuffer, renderPassInfo, pbrRenderingPipeline->ShaderPipelineLayout, rendererPassID, AssetManagerPtr::GetAssetPtr()->cameraManager->ActiveCamera);

        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipelineLayout, 0, 1, &skyBoxRenderingPipeline->DescriptorSets, 0, nullptr);
        static_cast<Skybox*>(AssetManagerPtr::GetAssetPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer);
    }
    else if (rendererID == RendererID::BlinnPhong_Raster_Renderer)
    {
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, forwardRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, forwardRenderingPipeline->ShaderPipelineLayout, 0, 1, &forwardRenderingPipeline->DescriptorSets, 0, nullptr);
        AssetManagerPtr::GetAssetPtr()->Draw(CommandBuffer, renderPassInfo, forwardRenderingPipeline->ShaderPipelineLayout, rendererPassID, AssetManagerPtr::GetAssetPtr()->cameraManager->ActiveCamera);

        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxRenderingPipeline->ShaderPipelineLayout, 0, 1, &skyBoxRenderingPipeline->DescriptorSets, 0, nullptr);
        static_cast<Skybox*>(AssetManagerPtr::GetAssetPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer);
    }

    vkCmdEndRenderPass(CommandBuffer);
    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}

void ForwardRenderPass::Destroy()
{
    DepthTexture->Delete();
    DebugLightPipeline->Destroy(EnginePtr::GetEnginePtr());
    forwardRenderingPipeline->Destroy(EnginePtr::GetEnginePtr());
    pbrRenderingPipeline->Destroy(EnginePtr::GetEnginePtr());
    skyBoxRenderingPipeline->Destroy(EnginePtr::GetEnginePtr());
    BaseRenderPass::Destroy();
}

