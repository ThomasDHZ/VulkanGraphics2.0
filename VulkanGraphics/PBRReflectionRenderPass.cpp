#include "PBRReflectionRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"

PBRReflectionRenderPass::PBRReflectionRenderPass() : BaseRenderPass()
{
}

PBRReflectionRenderPass::PBRReflectionRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList) : BaseRenderPass()
{
    RenderPassResolution = glm::ivec2(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height);

    ColorTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, EnginePtr::GetEnginePtr()->MaxSampleCount));
    RenderedTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));
    BloomTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, EnginePtr::GetEnginePtr()->MaxSampleCount));
    RenderedBloomTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));

    CreateRenderPass();
    CreateRendererFramebuffers();
    pbrPipeline = std::make_shared<PBRReflectionPipeline>(PBRReflectionPipeline(RenderPass, IrradianceMap, IrradianceMap, BRDFMap, ShadowMapTextureList));
    skyboxPipeline = std::make_shared<MultiViewSkyBoxPipeline>(RenderPass, TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], 1028);
    SetUpCommandBuffers();
}

PBRReflectionRenderPass::~PBRReflectionRenderPass()
{
}

void PBRReflectionRenderPass::CreateRenderPass()
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

    VkAttachmentDescription BloomAttachment = {};
    BloomAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    BloomAttachment.samples = EnginePtr::GetEnginePtr()->MaxSampleCount;
    BloomAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    BloomAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    BloomAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    BloomAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    BloomAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    AttachmentDescriptionList.emplace_back(BloomAttachment);

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

    VkAttachmentDescription BloomMultiSampledTexture = {};
    BloomMultiSampledTexture.format = VK_FORMAT_R8G8B8A8_UNORM;
    BloomMultiSampledTexture.samples = VK_SAMPLE_COUNT_1_BIT;
    BloomMultiSampledTexture.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomMultiSampledTexture.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    BloomMultiSampledTexture.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomMultiSampledTexture.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    BloomMultiSampledTexture.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    BloomMultiSampledTexture.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(BloomMultiSampledTexture);

    std::vector<VkAttachmentReference> ColorRefsList;
    ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

    std::vector<VkAttachmentReference> MultiSampleReferenceList;
    MultiSampleReferenceList.emplace_back(VkAttachmentReference{ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    MultiSampleReferenceList.emplace_back(VkAttachmentReference{ 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

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

void PBRReflectionRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(ColorTexture->View);
        AttachmentList.emplace_back(BloomTexture->View);
        AttachmentList.emplace_back(RenderedTexture->View);
        AttachmentList.emplace_back(RenderedBloomTexture->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
        frameBufferCreateInfo.height = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(EnginePtr::GetEnginePtr()->Device, &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void PBRReflectionRenderPass::SetUpCommandBuffers()
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

void PBRReflectionRenderPass::RebuildSwapChain(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList)
{
    RenderPassResolution = glm::ivec2(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height);

    ColorTexture->RecreateRendererTexture(RenderPassResolution);
    RenderedTexture->RecreateRendererTexture(RenderPassResolution);
    BloomTexture->RecreateRendererTexture(RenderPassResolution);
    RenderedBloomTexture->RecreateRendererTexture(RenderPassResolution);

    pbrPipeline->Destroy();
    skyboxPipeline->Destroy();

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    pbrPipeline->UpdateGraphicsPipeLine(RenderPass, IrradianceMap, PrefilerMap, BRDFMap, ShadowMapTextureList);
    skyboxPipeline->UpdateGraphicsPipeLine(RenderPass, TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], 1028);
    SetUpCommandBuffers();
}

void PBRReflectionRenderPass::Draw()
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
    renderPassInfo.renderArea.extent = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution();

    std::array<VkClearValue, 4> clearValues{};
    clearValues[0].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {1.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[2].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[3].color = { {1.0f, 1.0f, 0.0f, 1.0f} };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline->ShaderPipelineLayout, 0, 1, &skyboxPipeline->DescriptorSet, 0, nullptr);
    static_cast<Skybox*>(MeshManagerPtr::GetMeshManagerPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

    //vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pbrPipeline->ShaderPipeline);
    //vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pbrPipeline->ShaderPipelineLayout, 0, 1, &pbrPipeline->DescriptorSet, 0, nullptr);
    //AssetManagerPtr::GetAssetPtr()->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], pbrPipeline->ShaderPipelineLayout, CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera);

    vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void PBRReflectionRenderPass::Destroy()
{
    ColorTexture->Delete();
    RenderedTexture->Delete();
    BloomTexture->Delete();
    RenderedBloomTexture->Delete();

    pbrPipeline->Destroy();
    skyboxPipeline->Destroy();

    BaseRenderPass::Destroy();
}