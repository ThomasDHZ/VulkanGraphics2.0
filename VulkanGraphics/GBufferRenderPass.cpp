#include "GBufferRenderPass.h"

#include <stdexcept>
#include <array>
#include "Vertex.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "BaseMesh.h"

GBufferRenderPass::GBufferRenderPass()
{
}

GBufferRenderPass::GBufferRenderPass(VulkanEngine& engine)
{
    GPositionTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
    GNormalTexture = std::make_shared<RenderedGBufferNormalTexture>(engine);
    GAlbedoTexture = std::make_shared<RenderedGBufferAlbedoTexture>(engine);
    BloomTexture = std::make_shared<RenderedColorTexture>(engine);
    DepthTexture = std::make_shared<RenderedDepthTexture>(engine);
    SSAOTexture = std::make_shared<RenderedSSAOTexture>(engine);
    SSAOBlurTexture = std::make_shared<RenderedSSAOTexture>(engine);

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);

    gBufferPipeline = std::make_shared<GBufferPipeline>(engine, RenderPass, RenderDrawFlags::RenderNormally);
    AnimatedGBufferPipeline = std::make_shared<GBufferPipeline>(engine, RenderPass, RenderDrawFlags::RenderAnimated);
    ssaoPipeline = std::make_shared<SSAOPipeline>(engine, SSAORenderPass);
    ssaoBlurPipeline = std::make_shared<SSAOBlurPipeline>(engine, SSAOBlurRenderPass);
}

GBufferRenderPass::~GBufferRenderPass()
{
}

void GBufferRenderPass::CreateRenderPass(VulkanEngine& engine)
{
    //GBuffer
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

        VkAttachmentDescription DepthAttachment = {};
        DepthAttachment.format = VK_FORMAT_D32_SFLOAT;
        DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
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
        VkAttachmentReference depthReference = { 4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

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

        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &RenderPass))
        {
            throw std::runtime_error("failed to create RenderPass!");
        }
    }

    //SSAO
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

        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &SSAORenderPass))
        {
            throw std::runtime_error("failed to create RenderPass!");
        }
    }
   
    //SSAOBlur
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

        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &SSAOBlurRenderPass))
        {
            throw std::runtime_error("failed to create RenderPass!");
        }
    }
}

void GBufferRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
{
    SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
    SSAOSwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
    SSAOBlurSwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    {
        //GBuffer
        {
            std::vector<VkImageView> AttachmentList;
            AttachmentList.emplace_back(GPositionTexture->View);
            AttachmentList.emplace_back(GNormalTexture->View);
            AttachmentList.emplace_back(GAlbedoTexture->View);
            AttachmentList.emplace_back(BloomTexture->View);
            AttachmentList.emplace_back(DepthTexture->View);

            VkFramebufferCreateInfo fbufCreateInfo = {};
            fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbufCreateInfo.renderPass = RenderPass;
            fbufCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
            fbufCreateInfo.pAttachments = AttachmentList.data();
            fbufCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
            fbufCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
            fbufCreateInfo.layers = 1;

            if (vkCreateFramebuffer(engine.Device, &fbufCreateInfo, nullptr, &SwapChainFramebuffers[i]))
            {
                throw std::runtime_error("failed to create vkCreateImageView!");
            }
        }

        //SSAO
        {
            VkFramebufferCreateInfo SSAOfbufCreateInfo = {};
            SSAOfbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            SSAOfbufCreateInfo.renderPass = SSAORenderPass;
            SSAOfbufCreateInfo.attachmentCount = 1;
            SSAOfbufCreateInfo.pAttachments = &SSAOTexture->View;
            SSAOfbufCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
            SSAOfbufCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
            SSAOfbufCreateInfo.layers = 1;

            if (vkCreateFramebuffer(engine.Device, &SSAOfbufCreateInfo, nullptr, &SSAOSwapChainFramebuffers[i]))
            {
                throw std::runtime_error("failed to create SSAO vkCreateImageView!");
            }
        }

        //SSAOBlur
        {
            VkFramebufferCreateInfo SSAOfbufCreateInfo = {};
            SSAOfbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            SSAOfbufCreateInfo.renderPass = SSAOBlurRenderPass;
            SSAOfbufCreateInfo.attachmentCount = 1;
            SSAOfbufCreateInfo.pAttachments = &SSAOBlurTexture->View;
            SSAOfbufCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
            SSAOfbufCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
            SSAOfbufCreateInfo.layers = 1;

            if (vkCreateFramebuffer(engine.Device, &SSAOfbufCreateInfo, nullptr, &SSAOBlurSwapChainFramebuffers[i]))
            {
                throw std::runtime_error("failed to create SSAO vkCreateImageView!");
            }
        }
    }
}

void GBufferRenderPass::UpdateSwapChain(VulkanEngine& engine)
{
    GPositionTexture->RecreateRendererTexture(engine);
    GNormalTexture->RecreateRendererTexture(engine);
    GAlbedoTexture->RecreateRendererTexture(engine);
    BloomTexture->RecreateRendererTexture(engine);
    DepthTexture->RecreateRendererTexture(engine);
    SSAOTexture->RecreateRendererTexture(engine);
    SSAOBlurTexture->RecreateRendererTexture(engine);

    gBufferPipeline->UpdateGraphicsPipeLine(engine, RenderPass, RenderDrawFlags::RenderNormally);
    AnimatedGBufferPipeline->UpdateGraphicsPipeLine(engine, RenderPass, RenderDrawFlags::RenderAnimated);
    ssaoPipeline->UpdateGraphicsPipeLine(engine, SSAORenderPass);
    ssaoBlurPipeline->UpdateGraphicsPipeLine(engine, SSAOBlurRenderPass);

    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
}

void GBufferRenderPass::Destroy(VulkanEngine& engine)
{
    GPositionTexture->Delete(engine);
    GNormalTexture->Delete(engine);
    GAlbedoTexture->Delete(engine);
    BloomTexture->Delete(engine);
    DepthTexture->Delete(engine);
    SSAOTexture->Delete(engine);
    SSAOBlurTexture->Delete(engine);

    gBufferPipeline->Destroy(engine);
    AnimatedGBufferPipeline->Destroy(engine);
    ssaoPipeline->Destroy(engine);
    ssaoBlurPipeline->Destroy(engine);

    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}