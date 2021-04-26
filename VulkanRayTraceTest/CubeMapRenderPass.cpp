#include "CubeMapRenderPass.h"
#include "GraphicsPipeline.h"

CubeMapRenderPass::CubeMapRenderPass()
{
}

CubeMapRenderPass::CubeMapRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneDataptr, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer)
{
    sceneData = SceneDataUniformBuffer(engine, sceneDataptr->UniformDataInfo);

    RenderedTexture = std::make_shared<RenderedColorTexture>(engine);
    CopyTexture = std::make_shared<RenderedColorTexture>(engine);

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    CubeMapTexturePipeline = std::make_shared<CubeMapRenderingPipeline>(CubeMapRenderingPipeline(engine, assetManager, SkyUniformBuffer, RenderPass));
    skyBoxRenderingPipeline = std::make_shared<SkyBoxRenderingPipeline>(SkyBoxRenderingPipeline(engine, assetManager, SkyUniformBuffer, RenderPass));
    SetUpCommandBuffers(engine);
}

CubeMapRenderPass::~CubeMapRenderPass()
{
}

void CubeMapRenderPass::CreateRenderPass(VulkanEngine& engine)
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

    if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void CubeMapRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
{
    SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(RenderedTexture->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
        frameBufferCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(engine.Device, &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void CubeMapRenderPass::SetUpCommandBuffers(VulkanEngine& engine)
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

void CubeMapRenderPass::Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, Skybox& skybox)
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

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    VkViewport viewport{};
    viewport.width = 512.0f;
    viewport.height = 512.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D rect2D{};
    rect2D.extent.width = 512.0f;
    rect2D.extent.height = 512.0f;
    rect2D.offset.x = 0.0f;
    rect2D.offset.y = 0.0f;

    std::vector<glm::mat4> SkyboxViews =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    ConstSkyBoxView skyboxView;
    skyboxView.view = SkyboxViews[1];
    skyboxView.proj = glm::perspective(glm::radians(-90.0f), 1.0f, 0.1f, 10.0f);
    skyboxView.proj[1][1] *= -1;

    vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(CommandBuffer, 0, 1, &rect2D);
    vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, CubeMapTexturePipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, CubeMapTexturePipeline->ShaderPipelineLayout, 0, 1, &CubeMapTexturePipeline->DescriptorSets, 0, nullptr);
    vkCmdPushConstants(CommandBuffer, CubeMapTexturePipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstSkyBoxView), &skyboxView);
    skybox.Draw(CommandBuffer, renderPassInfo, RendererID);
    vkCmdEndRenderPass(CommandBuffer);

    //VkImageSubresourceRange subresourceRange{};
    //subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //subresourceRange.baseMipLevel = 0;
    //subresourceRange.levelCount = 1;
    //subresourceRange.layerCount = 1;

    //VkImageMemoryBarrier MemoryBarrior{};
    //MemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //MemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //MemoryBarrior.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    //MemoryBarrior.image = RenderedTexture->Image;
    //MemoryBarrior.subresourceRange = subresourceRange;
    //MemoryBarrior.srcAccessMask = 0;
    //MemoryBarrior.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &MemoryBarrior);

    //VkImageMemoryBarrier barrier2 = {};
    //barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //barrier2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //barrier2.image = CopyTexture->Image;
    //barrier2.subresourceRange = subresourceRange;
    //barrier2.srcAccessMask = 0;
    //barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    //vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

    //VkImageCopy copyRegion{};
    //copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
    //copyRegion.srcOffset = { 0, 0, 0 };
    //copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
    //copyRegion.dstOffset = { 0, 0, 0 };
    //copyRegion.extent = { engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height, 1 };
    //vkCmdCopyImage(CommandBuffer, engine.SwapChain.SwapChainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, CopyTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    //VkImageMemoryBarrier barrier3 = {};
    //barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //barrier3.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //barrier3.image = CopyTexture->Image;
    //barrier3.subresourceRange = subresourceRange;
    //barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //barrier3.dstAccessMask = 0;
    //vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

    //VkImageMemoryBarrier ReturnMemoryBarrior{};
    //ReturnMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //ReturnMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    //ReturnMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //ReturnMemoryBarrior.image = RenderedTexture->Image;
    //ReturnMemoryBarrior.subresourceRange = subresourceRange;
    //ReturnMemoryBarrior.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //ReturnMemoryBarrior.dstAccessMask = 0;
    //vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &ReturnMemoryBarrior);


    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CubeMapRenderPass::Update(VulkanEngine& engine, AssetManager& assetManager, SceneDataUniformBuffer& copysceneDataptr, std::shared_ptr<PerspectiveCamera> camera)
{
    copysceneDataptr.UniformDataInfo.sLight.direction = camera->GetFront();
    copysceneDataptr.UniformDataInfo.viewInverse = glm::inverse(camera->GetViewMatrix());
    copysceneDataptr.UniformDataInfo.projInverse = glm::inverse(camera->GetProjectionMatrix());
    copysceneDataptr.UniformDataInfo.projInverse[1][1] *= -1;
    copysceneDataptr.UniformDataInfo.view = camera->GetViewMatrix();
    copysceneDataptr.UniformDataInfo.proj = camera->GetProjectionMatrix();
    copysceneDataptr.UniformDataInfo.proj[1][1] *= -1;
    copysceneDataptr.UniformDataInfo.viewPos = glm::vec4(camera->GetPosition(), 0.0f);

    sceneData.Update(engine, copysceneDataptr.UniformDataInfo);
}

void CubeMapRenderPass::UpdateSwapChain(VulkanEngine& engine)
{
    RenderedTexture->RecreateRendererTexture(engine);

    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    CubeMapTexturePipeline->UpdateGraphicsPipeLine(engine, RenderPass);
    SetUpCommandBuffers(engine);
}

void CubeMapRenderPass::Destroy(VulkanEngine& engine)
{
    RenderedTexture->Delete(engine);

    sceneData.Destroy(engine);
    CubeMapTexturePipeline->Destroy(engine);

    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}