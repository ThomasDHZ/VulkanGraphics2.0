#include "GBufferRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"

GBufferRenderPass::GBufferRenderPass()
{
}

GBufferRenderPass::GBufferRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{

    GPositionTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
    GAlbedoTexture = std::make_shared<RenderedGBufferAlbedoTexture>(engine);
    GNormalTexture = std::make_shared<RenderedGBufferNormalTexture>(engine);
    //GTangentTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
    //GBiTangentTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
    GBloomTexture = std::make_shared<RenderedColorTexture>(engine);
    NormalMapTexture = std::make_shared<RenderedColorTexture>(engine);
    SpecularMapTexture = std::make_shared<RenderedColorTexture>(engine);
    DepthTexture = std::make_shared<RenderedDepthTexture>(engine);

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    gBufferPipeline = std::make_shared<GBufferPipeline>(GBufferPipeline(engine, assetManager, RenderPass));
    SetUpCommandBuffers(engine);
}

GBufferRenderPass::~GBufferRenderPass()
{
}

void GBufferRenderPass::CreateRenderPass(std::shared_ptr<VulkanEngine> engine)
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription PositionAttachment = {};
    PositionAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    PositionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    PositionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    PositionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    PositionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    PositionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    PositionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    PositionAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(PositionAttachment);

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

    VkAttachmentDescription NormalAttachment = {};
    NormalAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    NormalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    NormalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    NormalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    NormalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    NormalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    NormalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    NormalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(NormalAttachment);

    //VkAttachmentDescription TangentAttachment = {};
    //TangentAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    //TangentAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //TangentAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //TangentAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //TangentAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //TangentAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //TangentAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //TangentAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //AttachmentDescriptionList.emplace_back(TangentAttachment);

    //VkAttachmentDescription BiTangentAttachment = {};
    //BiTangentAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    //BiTangentAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //BiTangentAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //BiTangentAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //BiTangentAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //BiTangentAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //BiTangentAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //BiTangentAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //AttachmentDescriptionList.emplace_back(BiTangentAttachment);

    VkAttachmentDescription BloomAttachment = {};
    BloomAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    BloomAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    BloomAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    BloomAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    BloomAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    BloomAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    BloomAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(BloomAttachment);

    VkAttachmentDescription NormalMapAttachment = {};
    NormalMapAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    NormalMapAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    NormalMapAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    NormalMapAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    NormalMapAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    NormalMapAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    NormalMapAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    NormalMapAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(NormalMapAttachment);

    VkAttachmentDescription SpecularMapAttachment = {};
    SpecularMapAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    SpecularMapAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    SpecularMapAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    SpecularMapAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    SpecularMapAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    SpecularMapAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    SpecularMapAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    SpecularMapAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(SpecularMapAttachment);

    VkAttachmentDescription DepthAttachment = {};
    DepthAttachment.format = VK_FORMAT_D32_SFLOAT;
    DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(DepthAttachment);

    std::vector<VkAttachmentReference> ColorRefsList;
    ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 4, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 5, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    //ColorRefsList.emplace_back(VkAttachmentReference{ 6, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    //ColorRefsList.emplace_back(VkAttachmentReference{ 7, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    VkAttachmentReference depthReference = { 6, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
    subpassDescription.pColorAttachments = ColorRefsList.data();
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

    if (vkCreateRenderPass(engine->Device, &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void GBufferRenderPass::CreateRendererFramebuffers(std::shared_ptr<VulkanEngine> engine)
{
    SwapChainFramebuffers.resize(engine->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < engine->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(GPositionTexture->View);
        AttachmentList.emplace_back(GAlbedoTexture->View);
        AttachmentList.emplace_back(GNormalTexture->View);
        //AttachmentList.emplace_back(GTangentTexture->View);
        //AttachmentList.emplace_back(GBiTangentTexture->View);
        AttachmentList.emplace_back(GBloomTexture->View);
        AttachmentList.emplace_back(NormalMapTexture->View);
        AttachmentList.emplace_back(SpecularMapTexture->View);
        AttachmentList.emplace_back(DepthTexture->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = engine->SwapChain.GetSwapChainResolution().width;
        frameBufferCreateInfo.height = engine->SwapChain.GetSwapChainResolution().height;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(engine->Device, &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void GBufferRenderPass::SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine->CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(engine->Device, &allocInfo, &CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void GBufferRenderPass::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
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
    renderPassInfo.renderArea.extent = engine->SwapChain.SwapChainResolution;

    std::array<VkClearValue, 7> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[3].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[4].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[5].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    //clearValues[6].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    //clearValues[7].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[6].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gBufferPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gBufferPipeline->ShaderPipelineLayout, 0, 1, &gBufferPipeline->DescriptorSets, 0, nullptr);
    assetManager->Draw(CommandBuffer, renderPassInfo, gBufferPipeline->ShaderPipelineLayout, RendererID, assetManager->cameraManager->ActiveCamera);
    vkCmdEndRenderPass(CommandBuffer);
    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void GBufferRenderPass::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{
    GPositionTexture->RecreateRendererTexture(engine);
    GNormalTexture->RecreateRendererTexture(engine);
    GAlbedoTexture->RecreateRendererTexture(engine);
    //GTangentTexture->RecreateRendererTexture(engine);
    //GBiTangentTexture->RecreateRendererTexture(engine);
    GBloomTexture->RecreateRendererTexture(engine);
    NormalMapTexture->RecreateRendererTexture(engine);
    SpecularMapTexture->RecreateRendererTexture(engine);
    DepthTexture->RecreateRendererTexture(engine);

    vkDestroyRenderPass(engine->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    gBufferPipeline->UpdateGraphicsPipeLine(engine, assetManager, RenderPass);
    SetUpCommandBuffers(engine);
}

void GBufferRenderPass::Destroy(std::shared_ptr<VulkanEngine> engine)
{

    GPositionTexture->Delete();
    GNormalTexture->Delete();
    GAlbedoTexture->Delete();
    //GTangentTexture->Delete();
    //GBiTangentTexture->Delete();
    GBloomTexture->Delete();
    NormalMapTexture->Delete();
    SpecularMapTexture->Delete();
    DepthTexture->Delete();

    gBufferPipeline->Destroy(engine);
    vkDestroyRenderPass(engine->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}