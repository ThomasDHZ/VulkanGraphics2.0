#include "ReflectionCubeMapRenderPass.h"
#include "GraphicsPipeline.h"
#include "Skybox.h"

ReflectionCubeMapRenderPass::ReflectionCubeMapRenderPass() : BaseRenderPass()
{
}

ReflectionCubeMapRenderPass::ReflectionCubeMapRenderPass(uint32_t cubeMapSize) : BaseRenderPass()
{
    cubeSampler = std::make_shared<CubeSampler>(CubeSampler(EnginePtr::GetEnginePtr()));

    RenderPassResolution = glm::ivec2(cubeMapSize, cubeMapSize);
    RenderedCubeMap = std::make_shared<RenderedCubeMapTexture>(RenderedCubeMapTexture(RenderPassResolution, VK_SAMPLE_COUNT_1_BIT));

    CreateRenderPass();
    CreateRendererFramebuffers();
    depthCubeMapPipeline = std::make_shared<DepthCubeMapPipeline>(DepthCubeMapPipeline(RenderPass, RenderPassResolution.x, cubeSampler));
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

void ReflectionCubeMapRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(RenderedCubeMap->View);

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

void ReflectionCubeMapRenderPass::SetUpCommandBuffers()
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

void ReflectionCubeMapRenderPass::RebuildSwapChain(uint32_t cubeMapSize)
{
    RenderPassResolution = glm::ivec2(cubeMapSize, cubeMapSize);
    RenderedCubeMap->RecreateRendererTexture(RenderPassResolution);
    depthCubeMapPipeline->Destroy();

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    depthCubeMapPipeline->UpdateGraphicsPipeLine(RenderPass, RenderPassResolution.x, cubeSampler);
    SetUpCommandBuffers();
}

void ReflectionCubeMapRenderPass::Update()
{
    const glm::vec3 LightPos = LightManagerPtr::GetLightManagerPtr()->PointLightList[0]->LightBuffer.UniformDataInfo.position;
    float near_plane = 1.0f;
    float far_plane = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)RenderedCubeMap->Width / (float)RenderedCubeMap->Height, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    cubeSampler->UniformDataInfo.LightSpaceMatrix[0] = shadowProj * glm::lookAt(LightPos, LightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[1] = shadowProj * glm::lookAt(LightPos, LightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[2] = shadowProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[3] = shadowProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[4] = shadowProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    cubeSampler->UniformDataInfo.LightSpaceMatrix[5] = shadowProj * glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
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

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->ImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent.width = RenderPassResolution.x;
    renderPassInfo.renderArea.extent.height = RenderPassResolution.y;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthCubeMapPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthCubeMapPipeline->ShaderPipelineLayout, 0, 1, &depthCubeMapPipeline->DescriptorSet, 0, nullptr);
    vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    for (auto& mesh : MeshManagerPtr::GetMeshManagerPtr()->MeshList)
    {
        if (mesh->DrawFlags == MeshDrawFlags::Mesh_Draw_All)
        {
            if (mesh->ShowMesh)
            {
                glm::mat4 view = LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[0]->lightViewCamera->GetViewMatrix();
                glm::mat4 proj = LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[0]->lightViewCamera->GetProjectionMatrix();
                proj[1][1] *= -1;

                LightSceneInfo lightSceneInfo;
                lightSceneInfo.MeshIndex = mesh->MeshBufferIndex;
                lightSceneInfo.lightSpaceMatrix = proj * view;

                VkDeviceSize offsets[] = { 0 };

                vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], depthCubeMapPipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(LightSceneInfo), &lightSceneInfo);
                vkCmdBindVertexBuffers(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], 0, 1, &mesh->VertexBuffer.Buffer, offsets);
                if (mesh->IndexCount == 0)
                {
                    vkCmdDraw(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], mesh->VertexCount, 1, 0, 0);
                }
                else
                {
                    vkCmdBindIndexBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], mesh->IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], mesh->IndexCount, 1, 0, 0, 0);
                }
            }
        }
    }
    vkCmdEndRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void ReflectionCubeMapRenderPass::Destroy()
{
    RenderedCubeMap->Delete();
    depthCubeMapPipeline->Destroy();
    BaseRenderPass::Destroy();
}