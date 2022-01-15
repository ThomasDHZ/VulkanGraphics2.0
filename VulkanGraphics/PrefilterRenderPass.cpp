#include "PrefilterRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"


PrefilterRenderPass::PrefilterRenderPass() : BaseRenderPass()
{
}

PrefilterRenderPass::PrefilterRenderPass(std::shared_ptr<Texture> cubeMapTexture, uint32_t cubeMapSize) : BaseRenderPass()
{
    RenderPassResolution = glm::ivec2(cubeMapSize, cubeMapSize);
    CubeMapMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(RenderPassResolution.x, RenderPassResolution.y)))) + 1;

    DrawToCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(glm::ivec2(RenderPassResolution.x), VK_SAMPLE_COUNT_1_BIT));
    RenderedCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(glm::ivec2(RenderPassResolution.x), VK_SAMPLE_COUNT_1_BIT, CubeMapMipLevels));

    CreateRenderPass();
    CreateRendererFramebuffers();
    prefilterPipeline = std::make_shared<PrefilterPipeline>(PrefilterPipeline(RenderPass, cubeMapTexture, RenderPassResolution.x));
    SetUpCommandBuffers();

    Draw();
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
        frameBufferCreateInfo.width = RenderPassResolution.x;
        frameBufferCreateInfo.height = RenderPassResolution.y;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(EnginePtr::GetEnginePtr()->Device, &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void PrefilterRenderPass::RebuildSwapChain(std::shared_ptr<Texture> cubeMapTexture, uint32_t cubeMapSize)
{
    firstRun = true;
    RenderPassResolution = glm::ivec2(cubeMapSize, cubeMapSize);
    DrawToCubeMap->RecreateRendererTexture(RenderPassResolution);
    RenderedCubeMap->RecreateRendererTexture(RenderPassResolution);
   // auto a = TextureManagerPtr::GetTextureManagerPtr()->LoadCubeMap(RenderedCubeMap);

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
    prefilterPipeline->UpdateGraphicsPipeLine(RenderPass, cubeMapTexture, RenderPassResolution.x);
    SetUpCommandBuffers();

    Draw();
}

void PrefilterRenderPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    {

        if (firstRun)
        {
            DrawToCubeMap->UpdateCubeImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            RenderedCubeMap->UpdateCubeImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);
        }
        firstRun = false;
    }

    if (vkBeginCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    RenderedCubeMap->UpdateCubeImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0);

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {0.0f, 0.0f, 0.0f, 1.0f} };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->ImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent.width = RenderPassResolution.x;
    renderPassInfo.renderArea.extent.height = RenderPassResolution.y;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    PrefilterSkybox prefiliter;

    VkRect2D rect2D{};
    rect2D.extent.width = RenderPassResolution.x;
    rect2D.extent.height = RenderPassResolution.y;
    rect2D.offset.x = 0.0f;
    rect2D.offset.y = 0.0f;

    for (int x = 0; x < CubeMapMipLevels; x++)
    {
        VkViewport viewport{};
        viewport.width = static_cast<float>(RenderPassResolution.x * std::pow(0.5f, x));
        viewport.height = static_cast<float>(RenderPassResolution.y * std::pow(0.5f, x));
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        prefiliter.SkyboxSize = CubeMapMipLevels;
        prefiliter.roughness = (float)RenderPassResolution.x / (float)(CubeMapMipLevels - 1);

        vkCmdSetViewport(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &viewport);
        vkCmdSetScissor(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &rect2D);
        vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], prefilterPipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PrefilterSkybox), &prefiliter);
        vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, prefilterPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, prefilterPipeline->ShaderPipelineLayout, 0, 1, &prefilterPipeline->DescriptorSet, 0, nullptr);
        vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        static_cast<Skybox*>(MeshManagerPtr::GetMeshManagerPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
        vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

        DrawToCubeMap->UpdateCubeImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VkImageCopy copyRegion = {};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.layerCount = 6;
        copyRegion.srcOffset = { 0, 0, 0 };

        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.baseArrayLayer = 0;
        copyRegion.dstSubresource.mipLevel = x;
        copyRegion.dstSubresource.layerCount = 6;
        copyRegion.dstOffset = { 0, 0, 0 };

        copyRegion.extent.width = (uint32_t)viewport.width;
        copyRegion.extent.height = (uint32_t)viewport.height;
        copyRegion.extent.depth = 1;
        vkCmdCopyImage(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], DrawToCubeMap->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, RenderedCubeMap->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        DrawToCubeMap->UpdateCubeImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    RenderedCubeMap->UpdateCubeImageLayout(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void PrefilterRenderPass::Destroy()
{
    DrawToCubeMap->Delete();
    RenderedCubeMap->Delete();
    prefilterPipeline->Destroy();
    BaseRenderPass::Destroy();
}