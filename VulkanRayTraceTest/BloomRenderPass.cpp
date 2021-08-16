#include "BloomRenderPass.h"
#include "GraphicsPipeline.h"
#include "SSAOBlurRenderPass.h"

BloomRenderPass::BloomRenderPass() : BaseRenderPass()
{
}

BloomRenderPass::BloomRenderPass(glm::ivec2 renderPassResolution, std::shared_ptr<Texture> InputBloomTexture) : BaseRenderPass(renderPassResolution)
{
    BloomTexture = std::make_shared<RenderedColorTexture>(renderPassResolution);

    CreateRenderPass();
    CreateRendererFramebuffers();
    BloomPipelinePass1 = std::make_shared<BloomPipeline>(BloomPipeline(RenderPass, InputBloomTexture));
    BloomPipelinePass2 = std::make_shared<BloomPipeline>(BloomPipeline(RenderPass, BloomTexture));
    SetUpCommandBuffers();
}

BloomRenderPass::~BloomRenderPass()
{
}

void BloomRenderPass::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> AttachmentDescriptionList;

    VkAttachmentDescription SSAOAttachment = {};
    SSAOAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    SSAOAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    SSAOAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    SSAOAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    SSAOAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    SSAOAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    SSAOAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    SSAOAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    AttachmentDescriptionList.emplace_back(SSAOAttachment);

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

    if (vkCreateRenderPass(VulkanPtr::GetDevice(), &renderPassInfo, nullptr, &RenderPass))
    {
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void BloomRenderPass::CreateRendererFramebuffers()
{
    SwapChainFramebuffers.resize(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    for (size_t i = 0; i < EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(BloomTexture->View);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = RenderPass;
        frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
        frameBufferCreateInfo.pAttachments = AttachmentList.data();
        frameBufferCreateInfo.width = EnginePtr::GetEnginePtr()->ScreenResoulation.x;
        frameBufferCreateInfo.height = EnginePtr::GetEnginePtr()->ScreenResoulation.y;
        frameBufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(VulkanPtr::GetDevice(), &frameBufferCreateInfo, nullptr, &SwapChainFramebuffers[i]))
        {
            throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
        }
    }
}

void BloomRenderPass::SetUpCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = VulkanPtr::GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(VulkanPtr::GetDevice(), &allocInfo, &CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void BloomRenderPass::Draw()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(CommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRect2D rect{};
    rect.extent.width = RenderPassResolution.x;
    rect.extent.height = RenderPassResolution.y;
    rect.offset.x = 0;
    rect.offset.y = 0;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass;
    renderPassInfo.framebuffer = SwapChainFramebuffers[EnginePtr::GetEnginePtr()->DrawFrame];
    renderPassInfo.renderArea.offset = rect.offset;
    renderPassInfo.renderArea.extent = rect.extent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    ConstBloomProperites bloomProperites = {};
    bloomProperites.BloomPass = 0;

    vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdPushConstants(CommandBuffer, BloomPipelinePass1->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstBloomProperites), &bloomProperites);
    vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BloomPipelinePass1->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BloomPipelinePass1->ShaderPipelineLayout, 0, 1, &BloomPipelinePass1->DescriptorSets, 0, nullptr);
    vkCmdDraw(CommandBuffer, 6, 1, 0, 0);

    bloomProperites.BloomPass = 1;
    vkCmdPushConstants(CommandBuffer, BloomPipelinePass2->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstBloomProperites), &bloomProperites);
    vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BloomPipelinePass1->ShaderPipeline);
    vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BloomPipelinePass1->ShaderPipelineLayout, 0, 1, &BloomPipelinePass1->DescriptorSets, 0, nullptr);
    vkCmdDraw(CommandBuffer, 6, 1, 0, 0);
    vkCmdEndRenderPass(CommandBuffer);

    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void BloomRenderPass::RebuildSwapChain(std::shared_ptr<Texture> InputBloomTexture)
{
    BloomTexture->RecreateRendererTexture();

    BloomPipelinePass1->Destroy();
    BloomPipelinePass2->Destroy();

    vkDestroyRenderPass(VulkanPtr::GetDevice(), RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(VulkanPtr::GetDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass();
    CreateRendererFramebuffers();
    BloomPipelinePass1->UpdateGraphicsPipeLine(RenderPass, InputBloomTexture);
    BloomPipelinePass2->UpdateGraphicsPipeLine(RenderPass, BloomTexture);
    SetUpCommandBuffers();
}

void BloomRenderPass::Destroy()
{
    BloomTexture->Delete();
    BloomPipelinePass1->Destroy();
    BloomPipelinePass2->Destroy();
    BaseRenderPass::Destroy();
}
