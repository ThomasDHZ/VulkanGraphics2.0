#include "PBRReflectionRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"

PBRReflectionRenderPass::PBRReflectionRenderPass() : BaseRenderPass()
{
}

PBRReflectionRenderPass::PBRReflectionRenderPass(glm::ivec2 renderPassResolution, glm::vec3 reflectViewPos, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList) : BaseRenderPass()
{
    reflectionViewCamera = std::make_shared<ReflectionViewCamera>(ReflectionViewCamera("ReflectionCamera", glm::vec2(renderPassResolution), reflectViewPos));
    CameraManagerPtr::GetCameraManagerPtr()->CameraList.emplace_back(reflectionViewCamera);

    cubeSampler = std::make_shared<CubeSampler>(CubeSampler(EnginePtr::GetEnginePtr()));
    RenderPassResolution = renderPassResolution;

    RenderedTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));
    RenderedBloomTexture = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));

    CreateRenderPass();
    CreateRendererFramebuffers();
    pbrPipeline = std::make_shared<PBRReflectionPipeline>(PBRReflectionPipeline(RenderPass, IrradianceMap, IrradianceMap, BRDFMap, ShadowMapTextureList, RenderedCubeMap, cubeSampler, SpotLightShadowMapTextureList));
    skyboxPipeline = std::make_shared<MultiViewSkyboxPipeline>(MultiViewSkyboxPipeline(RenderPass, TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], renderPassResolution.x));
    SetUpCommandBuffers();
}

PBRReflectionRenderPass::~PBRReflectionRenderPass()
{
}

void PBRReflectionRenderPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription SampledAttachment = {};
    SampledAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    SampledAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    SampledAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    SampledAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    SampledAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    SampledAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    SampledAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    SampledAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(SampledAttachment);

    VkAttachmentDescription BloomSampledTexture = {};
    BloomSampledTexture.format = VK_FORMAT_R8G8B8A8_UNORM;
    BloomSampledTexture.samples = VK_SAMPLE_COUNT_1_BIT;
    BloomSampledTexture.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomSampledTexture.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    BloomSampledTexture.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomSampledTexture.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    BloomSampledTexture.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    BloomSampledTexture.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(BloomSampledTexture);

    std::vector<VkAttachmentReference> ColorRefsList;
    ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
    ColorRefsList.emplace_back(VkAttachmentReference{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

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

void PBRReflectionRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(RenderedTexture->View);
        AttachmentList.emplace_back(RenderedBloomTexture->View);

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

void PBRReflectionRenderPass::RebuildSwapChain(glm::ivec2 renderPassResolution, glm::vec3 reflectViewPos, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList)
{
    reflectPos = reflectViewPos;
    RenderPassResolution = renderPassResolution;

    RenderedTexture->RecreateRendererTexture(RenderPassResolution);
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
    pbrPipeline->UpdateGraphicsPipeLine(RenderPass, IrradianceMap, PrefilerMap, BRDFMap, ShadowMapTextureList, RenderedCubeMap, cubeSampler, SpotLightShadowMapTextureList);
    skyboxPipeline->UpdateGraphicsPipeLine(RenderPass, TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[2], renderPassResolution.x);
    SetUpCommandBuffers();
}

void PBRReflectionRenderPass::Draw()
{
    reflectionViewCamera->Position = reflectPos;
    const glm::vec3 LightPos = reflectPos;

    glm::mat4 reflectionProj = glm::ortho(XNearFar.x, XNearFar.y, YNearFar.x, YNearFar.y, ZNearFar.x, ZNearFar.y);
    cubeSampler->UniformDataInfo.LightSpaceMatrix[0] = reflectionProj * glm::lookAt(LightPos, LightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[1] = reflectionProj * glm::lookAt(LightPos, LightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[2] = reflectionProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[3] = reflectionProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[4] = reflectionProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[5] = reflectionProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->Update();

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
    renderPassInfo.renderArea.extent.width = RenderPassResolution.x;
    renderPassInfo.renderArea.extent.height = RenderPassResolution.y;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {1.0f, 1.0f, 0.0f, 1.0f} };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    VkRect2D rect2D{};
    rect2D.extent.width = RenderPassResolution.x;
    rect2D.extent.height = RenderPassResolution.y;
    rect2D.offset.x = 0.0f;
    rect2D.offset.y = 0.0f;

    VkViewport viewport{};
    viewport.width = static_cast<float>(RenderPassResolution.x);
    viewport.height = static_cast<float>(RenderPassResolution.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdSetViewport(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &viewport);
    vkCmdSetScissor(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &rect2D);
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

void PBRReflectionRenderPass::Destroy()
{
    RenderedTexture->Delete();
    RenderedBloomTexture->Delete();

    pbrPipeline->Destroy();
    skyboxPipeline->Destroy();

    BaseRenderPass::Destroy();
}