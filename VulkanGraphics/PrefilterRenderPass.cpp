#include "PrefilterRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"


PrefilterRenderPass::PrefilterRenderPass() : BaseRenderPass()
{
}

PrefilterRenderPass::PrefilterRenderPass(std::shared_ptr<VulkanEngine> engine) : BaseRenderPass()
{
    CubeMapSize = 512.0f;
    CubeMapMipLevels = 10;
    //float TempCubeMapSize;
    //while (CubeMapSize)
    //{
    //    CubeMapSize /= 2;
    //    CubeMapMipLevels++;
    //}

    DrawToCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(glm::ivec2(CubeMapSize)));
    RenderedCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(glm::ivec2(CubeMapSize), CubeMapMipLevels));

    CreateRenderPass();
    CreateRendererFramebuffers();
    prefilterPipeline = std::make_shared<PrefilterPipeline>(PrefilterPipeline(RenderPass, CubeMapSize));
    SetUpCommandBuffers();
    RenderedCubeMap->UpdateCubeImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    Draw();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex];
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(VulkanPtr::GetDevice(), &fenceCreateInfo, nullptr, &fence);
    vkQueueSubmit(VulkanPtr::GetGraphicsQueue(), 1, &submitInfo, fence);
    vkWaitForFences(VulkanPtr::GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(VulkanPtr::GetDevice(), fence, nullptr);
}

PrefilterRenderPass::~PrefilterRenderPass()
{
}

void PrefilterRenderPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription CubeMapAttachment = {};
    CubeMapAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    CubeMapAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    CubeMapAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    CubeMapAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    CubeMapAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    CubeMapAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    CubeMapAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    CubeMapAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(CubeMapAttachment);

    std::vector<VkAttachmentReference> ColorRefsList;
    ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
    subpassDescription.pColorAttachments = ColorRefsList.data();

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


    const uint32_t viewMask = 0b00111111;
    const uint32_t correlationMask = 0b00111111;

    VkRenderPassMultiviewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
    createInfo.subpassCount = 1;
    createInfo.pViewMasks = &viewMask;
    createInfo.correlationMaskCount = 1;
    createInfo.pCorrelationMasks = &correlationMask;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptionList.size());
    renderPassInfo.pAttachments = AttachmentDescriptionList.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(DependencyList.size());
    renderPassInfo.pDependencies = DependencyList.data();
    renderPassInfo.pNext = &createInfo;

    if (vkCreateRenderPass(EnginePtr::GetEnginePtr()->Device, &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void PrefilterRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(DrawToCubeMap->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = CubeMapSize;
        frameBufferCreateInfo.height = CubeMapSize;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(EnginePtr::GetEnginePtr()->Device, &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void PrefilterRenderPass::SetUpCommandBuffers()
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

void PrefilterRenderPass::RebuildSwapChain()
{
    prefilterPipeline->Destroy();

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    prefilterPipeline->UpdateGraphicsPipeLine(RenderPass, CubeMapSize);
    SetUpCommandBuffers();
}

void PrefilterRenderPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {0.0f, 0.0f, 0.0f, 1.0f} };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->ImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent.width = CubeMapSize;
    renderPassInfo.renderArea.extent.height = CubeMapSize;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    PrefilterSkybox prefiliter;

    VkImageSubresourceRange SkyBoxSubresourceRange{};
    SkyBoxSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    SkyBoxSubresourceRange.baseMipLevel = 0;
    SkyBoxSubresourceRange.levelCount = 1;
    SkyBoxSubresourceRange.layerCount = 6;

    VkRect2D rect2D{};
    rect2D.extent.width = CubeMapSize;
    rect2D.extent.height = CubeMapSize;
    rect2D.offset.x = 0.0f;
    rect2D.offset.y = 0.0f;

    for (int x = 0; x < CubeMapMipLevels; x++)
    {
        VkViewport viewport{};
        viewport.width = CubeMapSize;
        viewport.height = CubeMapSize;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        prefiliter.roughness = (float)CubeMapSize / (float)(CubeMapMipLevels - 1);

        vkCmdSetViewport(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &viewport);
        vkCmdSetScissor(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &rect2D);
        vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], prefilterPipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PrefilterSkybox), &prefiliter);
        vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, prefilterPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, prefilterPipeline->ShaderPipelineLayout, 0, 1, &prefilterPipeline->DescriptorSet, 0, nullptr);
        vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        static_cast<Skybox*>(MeshManagerPtr::GetMeshManagerPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
        vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

        VkImageSubresourceRange ImageSubresourceRange{};
        ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageSubresourceRange.baseMipLevel = 0;
        ImageSubresourceRange.levelCount = 1;
        ImageSubresourceRange.layerCount = 6;

        VkImageMemoryBarrier MemoryBarrior{};
        MemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        MemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        MemoryBarrior.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        MemoryBarrior.image = DrawToCubeMap->Image;
        MemoryBarrior.subresourceRange = ImageSubresourceRange;
        MemoryBarrior.srcAccessMask = 0;
        MemoryBarrior.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &MemoryBarrior);

        VkImageCopy copyRegion = {};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.layerCount = 6;
        copyRegion.srcOffset = { 0, 0, 0 };

        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.baseArrayLayer = 0;
        copyRegion.dstSubresource.mipLevel = 0;
        copyRegion.dstSubresource.layerCount = 6;
        copyRegion.dstOffset = { 0, 0, 0 };

        copyRegion.extent.width = (uint32_t)DrawToCubeMap->Width;
        copyRegion.extent.height = (uint32_t)DrawToCubeMap->Height;
        copyRegion.extent.depth = 1;
        vkCmdCopyImage(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], DrawToCubeMap->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, RenderedCubeMap->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        VkImageMemoryBarrier ReturnMemoryBarrior{};
        ReturnMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ReturnMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        ReturnMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ReturnMemoryBarrior.image = DrawToCubeMap->Image;
        ReturnMemoryBarrior.subresourceRange = ImageSubresourceRange;
        ReturnMemoryBarrior.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        ReturnMemoryBarrior.dstAccessMask = 0;
        vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &ReturnMemoryBarrior);
}

    RenderedCubeMap->UpdateCubeImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void PrefilterRenderPass::Destroy()
{
    prefilterPipeline->Destroy();

    BaseRenderPass::Destroy();
}