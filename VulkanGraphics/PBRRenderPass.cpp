#include "PBRRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"


PBRRenderPass::PBRRenderPass() : BaseRenderPass()
{
}

PBRRenderPass::PBRRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList) : BaseRenderPass()
{
    RenderPassResolution = glm::ivec2(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height);

    ColorTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(RenderPassResolution, EnginePtr::GetEnginePtr()->MaxSampleCount));
    RenderedTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));
    BloomTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(RenderPassResolution, EnginePtr::GetEnginePtr()->MaxSampleCount));
    RenderedBloomTexture = std::make_shared<RenderedColorTexture>(RenderedColorTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));
    DepthTexture = std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(RenderPassResolution, EnginePtr::GetEnginePtr()->MaxSampleCount));

    CreateRenderPass();
    CreateRendererFramebuffers();
    pbrPipeline = std::make_shared<PBRPipeline>(PBRPipeline(RenderPass, IrradianceMap, IrradianceMap, BRDFMap, ShadowMapTextureList, RenderedCubeMap, SpotLightShadowMapTextureList));
    skyboxPipeline = std::make_shared<SkyBoxRenderPipeline>(RenderPass, EnginePtr::GetEnginePtr()->MaxSampleCount);
    SetUpCommandBuffers();
}

PBRRenderPass::~PBRRenderPass()
{
}

void PBRRenderPass::CreateRenderPass()
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

    VkAttachmentDescription DepthAttachment = {};
    DepthAttachment.format = VK_FORMAT_D32_SFLOAT;
    DepthAttachment.samples = EnginePtr::GetEnginePtr()->MaxSampleCount;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    AttachmentDescriptionList.emplace_back(DepthAttachment);

    std::vector<VkAttachmentReference> ColorRefsList;
    ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

    std::vector<VkAttachmentReference> MultiSampleReferenceList;
    MultiSampleReferenceList.emplace_back(VkAttachmentReference{ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    MultiSampleReferenceList.emplace_back(VkAttachmentReference{ 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

    VkAttachmentReference depthReference = { 4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
    subpassDescription.pColorAttachments = ColorRefsList.data();
    subpassDescription.pDepthStencilAttachment = &depthReference;
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

void PBRRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(ColorTexture->View);
        AttachmentList.emplace_back(BloomTexture->View);
        AttachmentList.emplace_back(RenderedTexture->View);
        AttachmentList.emplace_back(RenderedBloomTexture->View);
        AttachmentList.emplace_back(DepthTexture->View);

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

void PBRRenderPass::RebuildSwapChain(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::vector<std::shared_ptr<RenderedDepthTexture>>& RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList)
{
    RenderPassResolution = glm::ivec2(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height);

    ColorTexture->RecreateRendererTexture(RenderPassResolution);
    RenderedTexture->RecreateRendererTexture(RenderPassResolution);
    BloomTexture->RecreateRendererTexture(RenderPassResolution);
    RenderedBloomTexture->RecreateRendererTexture(RenderPassResolution);
    DepthTexture->RecreateRendererTexture(RenderPassResolution);

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
    pbrPipeline->UpdateGraphicsPipeLine(RenderPass, IrradianceMap, PrefilerMap, BRDFMap, ShadowMapTextureList, RenderedCubeMap, SpotLightShadowMapTextureList);
    skyboxPipeline->UpdateGraphicsPipeLine(RenderPass, EnginePtr::GetEnginePtr()->MaxSampleCount);
    SetUpCommandBuffers();
}

void PBRRenderPass::Draw()
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

    std::array<VkClearValue, 5> clearValues{};
    clearValues[0].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {1.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[2].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[3].color = { {1.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[4].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    ConstSkyBoxView cubeMapInfo;
    cubeMapInfo.view = glm::mat4(glm::mat3(CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera->GetViewMatrix()));
    cubeMapInfo.proj = glm::perspective(glm::radians(CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera->GetZoom()), EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width / (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f);
    cubeMapInfo.proj[1][1] *= -1;

    vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], skyboxPipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstSkyBoxView), &cubeMapInfo);
    vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline->ShaderPipelineLayout, 0, 1, &skyboxPipeline->DescriptorSet, 0, nullptr);
    static_cast<Skybox*>(MeshManagerPtr::GetMeshManagerPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

    vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pbrPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pbrPipeline->ShaderPipelineLayout, 0, 1, &pbrPipeline->DescriptorSet, 0, nullptr);
    AssetManagerPtr::GetAssetPtr()->Draw(RendererType, CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], pbrPipeline->ShaderPipelineLayout, CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera);

    vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void PBRRenderPass::Destroy()
{
    ColorTexture->Delete();
    RenderedTexture->Delete();
    BloomTexture->Delete();
    RenderedBloomTexture->Delete();
    DepthTexture->Delete();

    pbrPipeline->Destroy();
    skyboxPipeline->Destroy();

    BaseRenderPass::Destroy();
}