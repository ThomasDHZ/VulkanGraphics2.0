#include "CubeMapRenderPass.h"
#include "GraphicsPipeline.h"

CubeMapRenderPass::CubeMapRenderPass()
{
}

CubeMapRenderPass::CubeMapRenderPass(uint32_t cubeMapSize)
{
    CubeMapSize = cubeMapSize;

    RenderedTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(EnginePtr::GetEnginePtr(), glm::vec2(CubeMapSize)));
    BlurredSkyBoxTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(EnginePtr::GetEnginePtr(), glm::vec2(CubeMapSize, CubeMapSize)));

    CreateRenderPass();
    CreateRendererFramebuffers();
    CubeMapTexturePipeline = std::make_shared<CubeMapRenderingPipeline>(CubeMapRenderingPipeline(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass));
    SetUpCommandBuffers();
    BlurredSkyBoxTexture->UpdateCubeImageLayout(EnginePtr::GetEnginePtr(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    Draw();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffer;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(VulkanPtr::GetDevice(), &fenceCreateInfo, nullptr, &fence);
   vkQueueSubmit(VulkanPtr::GetGraphicsQueue(), 1, &submitInfo, fence);
   vkWaitForFences(VulkanPtr::GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
   vkDestroyFence(VulkanPtr::GetDevice(), fence, nullptr);
}

CubeMapRenderPass::~CubeMapRenderPass()
{
}

void CubeMapRenderPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription AlebdoAttachment = {};
    AlebdoAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    AlebdoAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    AlebdoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AlebdoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    AlebdoAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AlebdoAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AlebdoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    AlebdoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(AlebdoAttachment);

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
    FirstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    FirstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    DependencyList.emplace_back(FirstDependency);

    VkSubpassDependency SecondDependency = {};
    SecondDependency.srcSubpass = 0;
    SecondDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
    SecondDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    SecondDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    SecondDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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

    if (vkCreateRenderPass(VulkanPtr::GetDevice(), &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void CubeMapRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(RenderedTexture->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = CubeMapSize;
        frameBufferCreateInfo.height = CubeMapSize;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(VulkanPtr::GetDevice(), &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void CubeMapRenderPass::SetUpCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = VulkanPtr::GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(VulkanPtr::GetDevice(), &allocInfo, &CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CubeMapRenderPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(CommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkExtent2D rectextent;
    rectextent.width = CubeMapSize;
    rectextent.height = CubeMapSize;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->DrawFrame];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = rectextent;

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    VkViewport viewport{};
    viewport.width = CubeMapSize;
    viewport.height = CubeMapSize;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D rect2D{};
    rect2D.extent.width = CubeMapSize;
    rect2D.extent.height = CubeMapSize;
    rect2D.offset.x = 0.0f;
    rect2D.offset.y = 0.0f;

    std::vector<glm::mat4> SkyboxViews =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    VkImageSubresourceRange SkyBoxSubresourceRange{};
    SkyBoxSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    SkyBoxSubresourceRange.baseMipLevel = 0;
    SkyBoxSubresourceRange.levelCount = 1;
    SkyBoxSubresourceRange.layerCount = 6;

    VkImageMemoryBarrier SkyBoxBarrierStart = {};
    SkyBoxBarrierStart.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    SkyBoxBarrierStart.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    SkyBoxBarrierStart.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    SkyBoxBarrierStart.image = BlurredSkyBoxTexture->Image;
    SkyBoxBarrierStart.subresourceRange = SkyBoxSubresourceRange;
    SkyBoxBarrierStart.srcAccessMask = 0;
    SkyBoxBarrierStart.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &SkyBoxBarrierStart);

    for (int x = 0; x < 6; x++)
    {
        ConstSkyBoxView skyboxView;
        skyboxView.view = SkyboxViews[x];
        skyboxView.proj = glm::perspective(glm::radians(-90.0f), 1.0f, 0.1f, 10.0f);
        skyboxView.proj[1][1] *= -1;

        vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(CommandBuffer, 0, 1, &rect2D);
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, CubeMapTexturePipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, CubeMapTexturePipeline->ShaderPipelineLayout, 0, 1, &CubeMapTexturePipeline->DescriptorSets, 0, nullptr);
        vkCmdPushConstants(CommandBuffer, CubeMapTexturePipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstSkyBoxView), &skyboxView);
        vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        static_cast<Skybox*>(AssetManagerPtr::GetAssetPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer);
        vkCmdEndRenderPass(CommandBuffer);

        VkImageSubresourceRange ImageSubresourceRange{};
        ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageSubresourceRange.baseMipLevel = 0;
        ImageSubresourceRange.levelCount = 1;
        ImageSubresourceRange.layerCount = 1;

        VkImageMemoryBarrier MemoryBarrior{};
        MemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        MemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        MemoryBarrior.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        MemoryBarrior.image = RenderedTexture->Image;
        MemoryBarrior.subresourceRange = ImageSubresourceRange;
        MemoryBarrior.srcAccessMask = 0;
        MemoryBarrior.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &MemoryBarrior);

        VkImageCopy copyRegion = {};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.srcOffset = { 0, 0, 0 };

        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.baseArrayLayer = x;
        copyRegion.dstSubresource.mipLevel = 0;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.dstOffset = { 0, 0, 0 };

        copyRegion.extent.width = (uint32_t)RenderedTexture->Width;
        copyRegion.extent.height = (uint32_t)RenderedTexture->Height;
        copyRegion.extent.depth = 1;
        vkCmdCopyImage(CommandBuffer, RenderedTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, BlurredSkyBoxTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        VkImageMemoryBarrier ReturnMemoryBarrior{};
        ReturnMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ReturnMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        ReturnMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ReturnMemoryBarrior.image = RenderedTexture->Image;
        ReturnMemoryBarrior.subresourceRange = ImageSubresourceRange;
        ReturnMemoryBarrior.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        ReturnMemoryBarrior.dstAccessMask = 0;
        vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &ReturnMemoryBarrior);
   }

    VkImageMemoryBarrier SkyBoxBarrierEnd = {};
    SkyBoxBarrierEnd.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    SkyBoxBarrierEnd.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    SkyBoxBarrierEnd.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    SkyBoxBarrierEnd.image = BlurredSkyBoxTexture->Image;
    SkyBoxBarrierEnd.subresourceRange = SkyBoxSubresourceRange;
    SkyBoxBarrierEnd.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    SkyBoxBarrierEnd.dstAccessMask = 0;
    vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &SkyBoxBarrierEnd);

    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CubeMapRenderPass::RebuildSwapChain()
{
    RenderedTexture->RecreateRendererTexture(EnginePtr::GetEnginePtr(), glm::vec2(CubeMapSize));

    CubeMapTexturePipeline->Destroy(EnginePtr::GetEnginePtr());

    vkDestroyRenderPass(VulkanPtr::GetDevice(), RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(VulkanPtr::GetDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    CubeMapTexturePipeline = std::make_shared<CubeMapRenderingPipeline>(CubeMapRenderingPipeline(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), RenderPass));
    SetUpCommandBuffers();
    BlurredSkyBoxTexture->UpdateCubeImageLayout(EnginePtr::GetEnginePtr(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    Draw();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffer;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(VulkanPtr::GetDevice(), &fenceCreateInfo, nullptr, &fence);
    vkQueueSubmit(VulkanPtr::GetGraphicsQueue(), 1, &submitInfo, fence);
    vkWaitForFences(VulkanPtr::GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(VulkanPtr::GetDevice(), fence, nullptr);
}

void CubeMapRenderPass::Destroy()
{
    RenderedTexture->Delete(EnginePtr::GetEnginePtr());
    BlurredSkyBoxTexture->Delete(EnginePtr::GetEnginePtr());

    CubeMapTexturePipeline->Destroy(EnginePtr::GetEnginePtr());

    vkDestroyRenderPass(VulkanPtr::GetDevice(), RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(VulkanPtr::GetDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}