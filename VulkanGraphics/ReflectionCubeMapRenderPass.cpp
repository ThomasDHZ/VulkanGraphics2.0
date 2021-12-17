#include "ReflectionCubeMapRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"

ReflectionCubeMapRenderPass::ReflectionCubeMapRenderPass() : BaseRenderPass()
{
}

ReflectionCubeMapRenderPass::ReflectionCubeMapRenderPass(uint32_t cubeMapSize, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList) : BaseRenderPass()
{
    cubeSampler = std::make_shared<CubeSampler>(CubeSampler(EnginePtr::GetEnginePtr()));

    RenderPassResolution = glm::ivec2(cubeMapSize, cubeMapSize);
    RenderedCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));
    RenderedCubeBloom = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));
    CreateRenderPass();
    CreateRendererFramebuffers();
    depthCubeMapPipeline = std::make_shared<ReflectionCubeMapPipeline>(ReflectionCubeMapPipeline(RenderPass, RenderPassResolution.x, ShadowMapTextureList, cubeSampler));
    skyboxPipeline = std::make_shared<SkyBoxRenderPipeline>(RenderPass, VK_SAMPLE_COUNT_1_BIT);
    SetUpCommandBuffers();
}

ReflectionCubeMapRenderPass::~ReflectionCubeMapRenderPass()
{
}

void ReflectionCubeMapRenderPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription ColorAttachment = {};
    ColorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(ColorAttachment);

    VkAttachmentDescription BloomAttachment = {};
    BloomAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    BloomAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    BloomAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    BloomAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    BloomAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    BloomAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    BloomAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(BloomAttachment);

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

void ReflectionCubeMapRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(RenderedCubeMap->View);
        AttachmentList.emplace_back(RenderedCubeBloom->View);

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

void ReflectionCubeMapRenderPass::RebuildSwapChain(uint32_t cubeMapSize, std::vector<std::shared_ptr<RenderedDepthTexture>>& ShadowMapTextureList)
{
    RenderPassResolution = glm::ivec2(cubeMapSize, cubeMapSize);
    RenderedCubeMap->RecreateRendererTexture(RenderPassResolution);
    RenderedCubeBloom->RecreateRendererTexture(RenderPassResolution);
    depthCubeMapPipeline->Destroy();
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
    depthCubeMapPipeline->UpdateGraphicsPipeLine(RenderPass, RenderPassResolution.x, ShadowMapTextureList, cubeSampler);
    skyboxPipeline->UpdateGraphicsPipeLine(RenderPass, VK_SAMPLE_COUNT_1_BIT);
    SetUpCommandBuffers();
}

void ReflectionCubeMapRenderPass::Update()
{
    const auto LightCamera = LightManagerPtr::GetLightManagerPtr()->PointLightList[0]->lightViewCamera;
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    for (int x = 0; x < 6; x++)
    {
        switch (x)
        {
        case 0: 
            viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 1:	
            viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 2:
            viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 3:	
            viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 4:	
            viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case 5:	
            viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;
        }

        cubeSampler->UniformDataInfo.LightSpaceMatrix[x] = LightCamera->GetProjectionMatrix() * viewMatrix;
    }
    cubeSampler->Update();
}

void ReflectionCubeMapRenderPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[1].color = { {1.0f, 1.0f, 0.0f, 1.0f} };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->ImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent.width = RenderPassResolution.x;
    renderPassInfo.renderArea.extent.height = RenderPassResolution.y;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    ConstSkyBoxView cubeMapInfo;
    cubeMapInfo.view = glm::mat4(glm::mat3(CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera->GetViewMatrix()));
    cubeMapInfo.proj = glm::perspective(glm::radians(CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera->GetZoom()), EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width / (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f);
    cubeMapInfo.proj[1][1] *= -1;

    vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    //vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], skyboxPipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstSkyBoxView), &cubeMapInfo);
    //vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline->ShaderPipeline);
    //vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline->ShaderPipelineLayout, 0, 1, &skyboxPipeline->DescriptorSet, 0, nullptr);
    //static_cast<Skybox*>(MeshManagerPtr::GetMeshManagerPtr()->GetMeshByType(MeshTypeFlag::Mesh_Type_SkyBox)[0].get())->Draw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

    vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthCubeMapPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthCubeMapPipeline->ShaderPipelineLayout, 0, 1, &depthCubeMapPipeline->DescriptorSet, 0, nullptr);
    AssetManagerPtr::GetAssetPtr()->Draw(RendererType, CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], depthCubeMapPipeline->ShaderPipelineLayout, CameraManagerPtr::GetCameraManagerPtr()->ActiveCamera);
    vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void ReflectionCubeMapRenderPass::Destroy()
{
    cubeSampler->Destroy();
    RenderedCubeMap->Delete();
    RenderedCubeBloom->Delete();
    skyboxPipeline->Destroy();
    depthCubeMapPipeline->Destroy();
    BaseRenderPass::Destroy();
}