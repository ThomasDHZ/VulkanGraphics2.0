#include "IrradianceRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"


IrradianceRenderPass::IrradianceRenderPass() : BaseRenderPass()
{
}

IrradianceRenderPass::IrradianceRenderPass(std::shared_ptr<VulkanEngine> engine) : BaseRenderPass()
{
    CubeMapSize = 512.0f * 4;
    ColorTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(glm::ivec2(CubeMapSize), EnginePtr::GetEnginePtr()->MaxSampleCount));
    RenderedTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(glm::ivec2(CubeMapSize), VK_SAMPLE_COUNT_1_BIT));
    RenderedCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(glm::ivec2(CubeMapSize)));

    CreateRenderPass();
    CreateRendererFramebuffers();
    irradiancePipeline = std::make_shared<IrradiancePipeline>(IrradiancePipeline(RenderPass));
    SetUpCommandBuffers();
    RenderedCubeMap->UpdateCubeImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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

IrradianceRenderPass::~IrradianceRenderPass()
{
}

void IrradianceRenderPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription AlebdoAttachment = {};
    AlebdoAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    AlebdoAttachment.samples = EnginePtr::GetEnginePtr()->MaxSampleCount;
    AlebdoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AlebdoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    AlebdoAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AlebdoAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AlebdoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    AlebdoAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    AttachmentDescriptionList.emplace_back(AlebdoAttachment);

    VkAttachmentDescription MultiSampledAttachment = {};
    MultiSampledAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    MultiSampledAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    MultiSampledAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    MultiSampledAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    MultiSampledAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    MultiSampledAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    MultiSampledAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    MultiSampledAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(MultiSampledAttachment);

    std::vector<VkAttachmentReference> ColorRefsList;
    ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

    std::vector<VkAttachmentReference> MultiSampleReferenceList;
    MultiSampleReferenceList.emplace_back(VkAttachmentReference{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });


    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
    subpassDescription.pColorAttachments = ColorRefsList.data();
    subpassDescription.pResolveAttachments = MultiSampleReferenceList.data();

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

void IrradianceRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(ColorTexture->View);
        AttachmentList.emplace_back(RenderedTexture->View);

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

void IrradianceRenderPass::SetUpCommandBuffers()
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

void IrradianceRenderPass::RebuildSwapChain()
{
    ColorTexture->RecreateRendererTexture();
    RenderedTexture->RecreateRendererTexture();
    irradiancePipeline->Destroy();

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    irradiancePipeline->UpdateGraphicsPipeLine(RenderPass);
    SetUpCommandBuffers();
}

void IrradianceRenderPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->ImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent.width = CubeMapSize;
    renderPassInfo.renderArea.extent.height = CubeMapSize;

    std::array<VkClearValue, 5> clearValues{};
    clearValues[0].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {0.0f, 1.0f, 0.0f, 1.0f} };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    std::vector<glm::mat4> SkyboxViews =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
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
    SkyBoxBarrierStart.image = RenderedCubeMap->Image;
    SkyBoxBarrierStart.subresourceRange = SkyBoxSubresourceRange;
    SkyBoxBarrierStart.srcAccessMask = 0;
    SkyBoxBarrierStart.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &SkyBoxBarrierStart);

    for (int x = 0; x < 6; x++)
    {
        ConstSkyBoxView skyboxView;
        skyboxView.view = SkyboxViews[x];
        skyboxView.proj = glm::perspective(glm::radians(-90.0f), 1.0f, 0.1f, 10.0f);
        skyboxView.proj[1][1] *= -1;


        vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, irradiancePipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, irradiancePipeline->ShaderPipelineLayout, 0, 1, &irradiancePipeline->DescriptorSet, 0, nullptr);
        vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], irradiancePipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstSkyBoxView), &skyboxView);
        vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        static_cast<Skybox*>(MeshManagerPtr::GetMeshManagerPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
        vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

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
        vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &MemoryBarrior);

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
        vkCmdCopyImage(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], RenderedTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, RenderedCubeMap->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        VkImageMemoryBarrier ReturnMemoryBarrior{};
        ReturnMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ReturnMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        ReturnMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ReturnMemoryBarrior.image = RenderedTexture->Image;
        ReturnMemoryBarrior.subresourceRange = ImageSubresourceRange;
        ReturnMemoryBarrior.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        ReturnMemoryBarrior.dstAccessMask = 0;
        vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &ReturnMemoryBarrior);
    }

    VkImageMemoryBarrier SkyBoxBarrierEnd = {};
    SkyBoxBarrierEnd.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    SkyBoxBarrierEnd.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    SkyBoxBarrierEnd.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    SkyBoxBarrierEnd.image = RenderedCubeMap->Image;
    SkyBoxBarrierEnd.subresourceRange = SkyBoxSubresourceRange;
    SkyBoxBarrierEnd.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    SkyBoxBarrierEnd.dstAccessMask = 0;
    vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &SkyBoxBarrierEnd);

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void IrradianceRenderPass::Destroy()
{
    ColorTexture->Delete();
    RenderedTexture->Delete();
    irradiancePipeline->Destroy();

    BaseRenderPass::Destroy();
}