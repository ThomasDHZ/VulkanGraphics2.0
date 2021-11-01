#include "DepthPassRendererPass.h"
#include "GraphicsPipeline.h"

DepthPassRendererPass::DepthPassRendererPass() : BaseRenderPass()
{
}

DepthPassRendererPass::DepthPassRendererPass(uint32_t depthTextureSize) : BaseRenderPass()
{
    RenderPassResolution = glm::ivec2(depthTextureSize, depthTextureSize);
    DepthToTexture = std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(RenderPassResolution));
   
    for (int x = 0; x < 2; x++)
    {
        DepthTextureList.emplace_back(std::make_shared<RenderedDepthTexture>(RenderedDepthTexture(RenderPassResolution)));
        DepthTextureList[x]->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    DepthToTexture->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    CreateRenderPass();
    CreateRendererFramebuffers();
    depthPipeline = std::make_shared<DepthPassPipeline>(DepthPassPipeline(RenderPass));
    SetUpCommandBuffers(); 
}

DepthPassRendererPass::~DepthPassRendererPass()
{
}

void DepthPassRendererPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription DepthAttachment = {};
    DepthAttachment.format = VK_FORMAT_D32_SFLOAT;
    DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(DepthAttachment);

    VkAttachmentReference depthReference = { 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
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

    if (vkCreateRenderPass(EnginePtr::GetEnginePtr()->Device, &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void DepthPassRendererPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(DepthToTexture->View);

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

void DepthPassRendererPass::SetUpCommandBuffers()
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

void DepthPassRendererPass::RebuildSwapChain(uint32_t depthTextureSize)
{
    RenderPassResolution = glm::ivec2(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height);
    DepthToTexture->RecreateRendererTexture(RenderPassResolution);

    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->DirectionalLightList.size(); x++)
    {
        DepthTextureList[x]->RecreateRendererTexture(RenderPassResolution);
        DepthTextureList[x]->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    depthPipeline->Destroy();

    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    depthPipeline->UpdateGraphicsPipeLine(RenderPass);
    SetUpCommandBuffers();
}

void DepthPassRendererPass::Draw()
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
    renderPassInfo.renderArea.extent.width = RenderPassResolution.x;
    renderPassInfo.renderArea.extent.height = RenderPassResolution.y;

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->DirectionalLightList.size(); x++)
    {
        vkCmdBeginRenderPass(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, depthPipeline->ShaderPipelineLayout, 0, 1, &depthPipeline->DescriptorSet, 0, nullptr);
        for (auto& mesh : MeshManagerPtr::GetMeshManagerPtr()->MeshList)
        {
            if (mesh->DrawFlags == MeshDrawFlags::Mesh_Draw_All)
            {
                if (mesh->ShowMesh)
                {
                    glm::mat4 view = LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->lightViewCamera->GetViewMatrix();
                    glm::mat4 proj = LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->lightViewCamera->GetProjectionMatrix();
                    proj[1][1] *= -1;

                    LightSceneInfo lightSceneInfo;
                    lightSceneInfo.MeshIndex = mesh->MeshBufferIndex;
                    lightSceneInfo.lightSpaceMatrix = proj * view;

                    VkDeviceSize offsets[] = { 0 };

                    vkCmdPushConstants(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], depthPipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(LightSceneInfo), &lightSceneInfo);
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

     /*   DepthTextureList[x]->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageSubresourceRange ImageSubresourceRange{};
        ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        ImageSubresourceRange.baseMipLevel = 0;
        ImageSubresourceRange.levelCount = 1;
        ImageSubresourceRange.layerCount = 1;

        VkImageMemoryBarrier SrcMemoryBarrior{};
        SrcMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        SrcMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        SrcMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        SrcMemoryBarrior.image = DepthToTexture->Image;
        SrcMemoryBarrior.subresourceRange = ImageSubresourceRange;
        SrcMemoryBarrior.srcAccessMask = 0;
        SrcMemoryBarrior.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &SrcMemoryBarrior);


        VkImageCopy copyRegion = {};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        copyRegion.srcSubresource.baseArrayLayer = 0;
        copyRegion.srcSubresource.mipLevel = 0;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.srcOffset = { 0, 0, 0 };

        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        copyRegion.dstSubresource.baseArrayLayer = 0;
        copyRegion.dstSubresource.mipLevel = 0;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.dstOffset = { 0, 0, 0 };

        copyRegion.extent.width = (uint32_t)RenderPassResolution.x;
        copyRegion.extent.height = (uint32_t)RenderPassResolution.y;
        copyRegion.extent.depth = 1;
        vkCmdCopyImage(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], DepthToTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, DepthTextureList[x]->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        VkImageMemoryBarrier ReturnSrcMemoryBarrior{};
        ReturnSrcMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ReturnSrcMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        ReturnSrcMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ReturnSrcMemoryBarrior.image = DepthToTexture->Image;
        ReturnSrcMemoryBarrior.subresourceRange = ImageSubresourceRange;
        ReturnSrcMemoryBarrior.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        ReturnSrcMemoryBarrior.dstAccessMask = 0;
        vkCmdPipelineBarrier(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &ReturnSrcMemoryBarrior);
   
        DepthTextureList[x]->UpdateDepthImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);*/
    }

    if (vkEndCommandBuffer(CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void DepthPassRendererPass::Destroy()
{
    DepthToTexture->Delete();
    for (auto& light : DepthTextureList)
    {
        light->Delete();
    }
    depthPipeline->Destroy();
    BaseRenderPass::Destroy();
}