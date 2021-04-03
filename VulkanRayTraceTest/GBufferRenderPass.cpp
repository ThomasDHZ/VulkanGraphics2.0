#include "GBufferRenderPass.h"

void GBufferRenderPass::CreateRenderPass(VulkanEngine& engine)
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
        throw std::runtime_error("failed to create GBuffer RenderPass!");
    }
}

void GBufferRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
{
    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkImageView> AttachmentList;
        AttachmentList.emplace_back(GPositionTexture->View);
        AttachmentList.emplace_back(GAlbedoTexture->View);
        AttachmentList.emplace_back(GNormalTexture->View);
        AttachmentList.emplace_back(GBloomTexture->View);
        AttachmentList.emplace_back(DepthTexture->View);

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

void GBufferRenderPass::CreatePipeline(VulkanEngine& engine)
{
   /* std::vector<char> vertShaderCode;
    std::vector<char> fragShaderCode;
    if (PipelineBitFlags & RenderDrawFlags::RenderNormally)
    {
        vertShaderCode = ReadShaderFile("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/GBufferShaderVert.spv");
        fragShaderCode = ReadShaderFile("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/GBufferShaderFrag.spv");
    }
    else if (PipelineBitFlags & RenderDrawFlags::RenderAnimated)
    {
        vertShaderCode = ReadShaderFile("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/AnimatedGBufferShaderVert.spv");
        fragShaderCode = ReadShaderFile("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/GBufferShaderFrag.spv");
    }

    VkShaderModule vertShaderModule = CreateShaderModule(renderer, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(renderer, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)renderer.SwapChain.GetSwapChainResolution().width;
    viewport.height = (float)renderer.SwapChain.GetSwapChainResolution().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = renderer.SwapChain.GetSwapChainResolution();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    std::array<VkPipelineColorBlendAttachmentState, 4> ColorAttachment = {};
    ColorAttachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorAttachment[0].blendEnable = VK_TRUE;
    ColorAttachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorAttachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorAttachment[0].colorBlendOp = VK_BLEND_OP_ADD;
    ColorAttachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorAttachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorAttachment[0].alphaBlendOp = VK_BLEND_OP_SUBTRACT;

    ColorAttachment[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorAttachment[1].blendEnable = VK_TRUE;
    ColorAttachment[1].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorAttachment[1].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorAttachment[1].colorBlendOp = VK_BLEND_OP_ADD;
    ColorAttachment[1].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorAttachment[1].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorAttachment[1].alphaBlendOp = VK_BLEND_OP_SUBTRACT;

    ColorAttachment[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorAttachment[2].blendEnable = VK_TRUE;
    ColorAttachment[2].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorAttachment[2].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorAttachment[2].colorBlendOp = VK_BLEND_OP_ADD;
    ColorAttachment[2].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorAttachment[2].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorAttachment[2].alphaBlendOp = VK_BLEND_OP_SUBTRACT;

    ColorAttachment[3].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorAttachment[3].blendEnable = VK_TRUE;
    ColorAttachment[3].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorAttachment[3].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorAttachment[3].colorBlendOp = VK_BLEND_OP_ADD;
    ColorAttachment[3].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorAttachment[3].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorAttachment[3].alphaBlendOp = VK_BLEND_OP_SUBTRACT;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = static_cast<uint32_t>(ColorAttachment.size());
    colorBlending.pAttachments = ColorAttachment.data();
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &ShaderPipelineDescriptorLayout;

    if (vkCreatePipelineLayout(renderer.Device, &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = ShaderPipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(renderer.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(renderer.Device, fragShaderModule, nullptr);
    vkDestroyShaderModule(renderer.Device, vertShaderModule, nullptr);*/
}

//#include "GBufferRenderPass.h"
//
//#include <stdexcept>
//#include <array>
//#include "Vertex.h"
//#include "ImGui/imgui_impl_vulkan.h"
//#include "BaseMesh.h"
//
//GBufferRenderPass::GBufferRenderPass()
//{
//}
//
//GBufferRenderPass::GBufferRenderPass(VulkanEngine& engine)
//{
//    GPositionTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
//    GNormalTexture = std::make_shared<RenderedGBufferNormalTexture>(engine);
//    GAlbedoTexture = std::make_shared<RenderedGBufferAlbedoTexture>(engine);
//    BloomTexture = std::make_shared<RenderedColorTexture>(engine);
//    DepthTexture = std::make_shared<RenderedDepthTexture>(engine);
//    SSAOTexture = std::make_shared<RenderedSSAOTexture>(engine);
//    SSAOBlurTexture = std::make_shared<RenderedSSAOTexture>(engine);
//
//    CreateRenderPass(engine);
//    CreateRendererFramebuffers(engine);
//
//    gBufferPipeline = std::make_shared<GBufferPipeline>(engine, RenderPass, RenderDrawFlags::RenderNormally);
//    AnimatedGBufferPipeline = std::make_shared<GBufferPipeline>(engine, RenderPass, RenderDrawFlags::RenderAnimated);
//    ssaoPipeline = std::make_shared<SSAOPipeline>(engine, SSAORenderPass);
//    ssaoBlurPipeline = std::make_shared<SSAOBlurPipeline>(engine, SSAOBlurRenderPass);
//}
//
//GBufferRenderPass::~GBufferRenderPass()
//{
//}
//
//void GBufferRenderPass::CreateRenderPass(VulkanEngine& engine)
//{
//    //GBuffer
//    {
//        std::vector<VkAttachmentDescription> AttachmentDescriptionList;
//
//        VkAttachmentDescription PositionAttachment = {};
//        PositionAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
//        PositionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        PositionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        PositionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//        PositionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        PositionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        PositionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        PositionAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(PositionAttachment);
//
//        VkAttachmentDescription NormalAttachment = {};
//        NormalAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
//        NormalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        NormalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        NormalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//        NormalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        NormalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        NormalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        NormalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(NormalAttachment);
//
//        VkAttachmentDescription AlebdoAttachment = {};
//        AlebdoAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
//        AlebdoAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        AlebdoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        AlebdoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//        AlebdoAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        AlebdoAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        AlebdoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        AlebdoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(AlebdoAttachment);
//
//        VkAttachmentDescription BloomAttachment = {};
//        BloomAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
//        BloomAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        BloomAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        BloomAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//        BloomAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        BloomAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        BloomAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        BloomAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(BloomAttachment);
//
//        VkAttachmentDescription DepthAttachment = {};
//        DepthAttachment.format = VK_FORMAT_D32_SFLOAT;
//        DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(DepthAttachment);
//
//        std::vector<VkAttachmentReference> ColorRefsList;
//        ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
//        ColorRefsList.emplace_back(VkAttachmentReference{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
//        ColorRefsList.emplace_back(VkAttachmentReference{ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
//        ColorRefsList.emplace_back(VkAttachmentReference{ 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
//        VkAttachmentReference depthReference = { 4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
//
//        VkSubpassDescription subpassDescription = {};
//        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
//        subpassDescription.pColorAttachments = ColorRefsList.data();
//        subpassDescription.pDepthStencilAttachment = &depthReference;
//
//        std::vector<VkSubpassDependency> DependencyList;
//
//        VkSubpassDependency FirstDependency = {};
//        FirstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//        FirstDependency.dstSubpass = 0;
//        FirstDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        FirstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        FirstDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        FirstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        FirstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//        DependencyList.emplace_back(FirstDependency);
//
//        VkSubpassDependency SecondDependency = {};
//        SecondDependency.srcSubpass = 0;
//        SecondDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
//        SecondDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        SecondDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        SecondDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        SecondDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        SecondDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//        DependencyList.emplace_back(SecondDependency);
//
//        VkRenderPassCreateInfo renderPassInfo = {};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//        renderPassInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptionList.size());
//        renderPassInfo.pAttachments = AttachmentDescriptionList.data();
//        renderPassInfo.subpassCount = 1;
//        renderPassInfo.pSubpasses = &subpassDescription;
//        renderPassInfo.dependencyCount = static_cast<uint32_t>(DependencyList.size());
//        renderPassInfo.pDependencies = DependencyList.data();
//
//        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &RenderPass))
//        {
//            throw std::runtime_error("failed to create RenderPass!");
//        }
//    }
//
//    //SSAO
//    {
//        std::vector<VkAttachmentDescription> AttachmentDescriptionList;
//        VkAttachmentDescription SSAOAttachment = {};
//        SSAOAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
//        SSAOAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        SSAOAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        SSAOAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//        SSAOAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        SSAOAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        SSAOAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        SSAOAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(SSAOAttachment);
//
//        std::vector<VkAttachmentReference> ColorRefsList;
//        ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
//
//        VkSubpassDescription subpassDescription = {};
//        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
//        subpassDescription.pColorAttachments = ColorRefsList.data();
//
//        std::vector<VkSubpassDependency> DependencyList;
//
//        VkSubpassDependency FirstDependency = {};
//        FirstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//        FirstDependency.dstSubpass = 0;
//        FirstDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        FirstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        FirstDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        FirstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        FirstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//        DependencyList.emplace_back(FirstDependency);
//
//        VkSubpassDependency SecondDependency = {};
//        SecondDependency.srcSubpass = 0;
//        SecondDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
//        SecondDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        SecondDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        SecondDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        SecondDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        SecondDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//        DependencyList.emplace_back(SecondDependency);
//
//        VkRenderPassCreateInfo renderPassInfo = {};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//        renderPassInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptionList.size());
//        renderPassInfo.pAttachments = AttachmentDescriptionList.data();
//        renderPassInfo.subpassCount = 1;
//        renderPassInfo.pSubpasses = &subpassDescription;
//        renderPassInfo.dependencyCount = static_cast<uint32_t>(DependencyList.size());
//        renderPassInfo.pDependencies = DependencyList.data();
//
//        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &SSAORenderPass))
//        {
//            throw std::runtime_error("failed to create RenderPass!");
//        }
//    }
//
//    //SSAOBlur
//    {
//        std::vector<VkAttachmentDescription> AttachmentDescriptionList;
//        VkAttachmentDescription SSAOAttachment = {};
//        SSAOAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
//        SSAOAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//        SSAOAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//        SSAOAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//        SSAOAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//        SSAOAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//        SSAOAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        SSAOAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        AttachmentDescriptionList.emplace_back(SSAOAttachment);
//
//        std::vector<VkAttachmentReference> ColorRefsList;
//        ColorRefsList.emplace_back(VkAttachmentReference{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
//
//        VkSubpassDescription subpassDescription = {};
//        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(ColorRefsList.size());
//        subpassDescription.pColorAttachments = ColorRefsList.data();
//
//        std::vector<VkSubpassDependency> DependencyList;
//
//        VkSubpassDependency FirstDependency = {};
//        FirstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//        FirstDependency.dstSubpass = 0;
//        FirstDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        FirstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        FirstDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        FirstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        FirstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//        DependencyList.emplace_back(FirstDependency);
//
//        VkSubpassDependency SecondDependency = {};
//        SecondDependency.srcSubpass = 0;
//        SecondDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
//        SecondDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//        SecondDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//        SecondDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        SecondDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        SecondDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//        DependencyList.emplace_back(SecondDependency);
//
//        VkRenderPassCreateInfo renderPassInfo = {};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//        renderPassInfo.attachmentCount = static_cast<uint32_t>(AttachmentDescriptionList.size());
//        renderPassInfo.pAttachments = AttachmentDescriptionList.data();
//        renderPassInfo.subpassCount = 1;
//        renderPassInfo.pSubpasses = &subpassDescription;
//        renderPassInfo.dependencyCount = static_cast<uint32_t>(DependencyList.size());
//        renderPassInfo.pDependencies = DependencyList.data();
//
//        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &SSAOBlurRenderPass))
//        {
//            throw std::runtime_error("failed to create RenderPass!");
//        }
//    }
//}
//
//void GBufferRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
//{
//    SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
//    SSAOSwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
//    SSAOBlurSwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
//    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
//    {
//        //GBuffer
//        {
//            std::vector<VkImageView> AttachmentList;
//            AttachmentList.emplace_back(GPositionTexture->View);
//            AttachmentList.emplace_back(GNormalTexture->View);
//            AttachmentList.emplace_back(GAlbedoTexture->View);
//            AttachmentList.emplace_back(BloomTexture->View);
//            AttachmentList.emplace_back(DepthTexture->View);
//
//            VkFramebufferCreateInfo fbufCreateInfo = {};
//            fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//            fbufCreateInfo.renderPass = RenderPass;
//            fbufCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
//            fbufCreateInfo.pAttachments = AttachmentList.data();
//            fbufCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
//            fbufCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
//            fbufCreateInfo.layers = 1;
//
//            if (vkCreateFramebuffer(engine.Device, &fbufCreateInfo, nullptr, &SwapChainFramebuffers[i]))
//            {
//                throw std::runtime_error("failed to create vkCreateImageView!");
//            }
//        }
//
//        //SSAO
//        {
//            VkFramebufferCreateInfo SSAOfbufCreateInfo = {};
//            SSAOfbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//            SSAOfbufCreateInfo.renderPass = SSAORenderPass;
//            SSAOfbufCreateInfo.attachmentCount = 1;
//            SSAOfbufCreateInfo.pAttachments = &SSAOTexture->View;
//            SSAOfbufCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
//            SSAOfbufCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
//            SSAOfbufCreateInfo.layers = 1;
//
//            if (vkCreateFramebuffer(engine.Device, &SSAOfbufCreateInfo, nullptr, &SSAOSwapChainFramebuffers[i]))
//            {
//                throw std::runtime_error("failed to create SSAO vkCreateImageView!");
//            }
//        }
//
//        //SSAOBlur
//        {
//            VkFramebufferCreateInfo SSAOfbufCreateInfo = {};
//            SSAOfbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//            SSAOfbufCreateInfo.renderPass = SSAOBlurRenderPass;
//            SSAOfbufCreateInfo.attachmentCount = 1;
//            SSAOfbufCreateInfo.pAttachments = &SSAOBlurTexture->View;
//            SSAOfbufCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
//            SSAOfbufCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
//            SSAOfbufCreateInfo.layers = 1;
//
//            if (vkCreateFramebuffer(engine.Device, &SSAOfbufCreateInfo, nullptr, &SSAOBlurSwapChainFramebuffers[i]))
//            {
//                throw std::runtime_error("failed to create SSAO vkCreateImageView!");
//            }
//        }
//    }
//}
//
//void GBufferRenderPass::UpdateSwapChain(VulkanEngine& engine)
//{
//    GPositionTexture->RecreateRendererTexture(engine);
//    GNormalTexture->RecreateRendererTexture(engine);
//    GAlbedoTexture->RecreateRendererTexture(engine);
//    BloomTexture->RecreateRendererTexture(engine);
//    DepthTexture->RecreateRendererTexture(engine);
//    SSAOTexture->RecreateRendererTexture(engine);
//    SSAOBlurTexture->RecreateRendererTexture(engine);
//
//    gBufferPipeline->UpdateGraphicsPipeLine(engine, RenderPass, RenderDrawFlags::RenderNormally);
//    AnimatedGBufferPipeline->UpdateGraphicsPipeLine(engine, RenderPass, RenderDrawFlags::RenderAnimated);
//    ssaoPipeline->UpdateGraphicsPipeLine(engine, SSAORenderPass);
//    ssaoBlurPipeline->UpdateGraphicsPipeLine(engine, SSAOBlurRenderPass);
//
//    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
//    RenderPass = VK_NULL_HANDLE;
//
//    vkDestroyRenderPass(engine.Device, SSAORenderPass, nullptr);
//    SSAORenderPass = VK_NULL_HANDLE;
//
//    vkDestroyRenderPass(engine.Device, SSAOBlurRenderPass, nullptr);
//    SSAOBlurRenderPass = VK_NULL_HANDLE;
//
//    for (int x = 0; x < engine.GetSwapChainImageCount(); x++)
//    {
//        vkDestroyFramebuffer(engine.Device, SwapChainFramebuffers[x], nullptr);
//        SwapChainFramebuffers[x] = VK_NULL_HANDLE;
//
//        vkDestroyFramebuffer(engine.Device, SSAOSwapChainFramebuffers[x], nullptr);
//        SSAOSwapChainFramebuffers[x] = VK_NULL_HANDLE;
//
//        vkDestroyFramebuffer(engine.Device, SSAOBlurSwapChainFramebuffers[x], nullptr);
//        SSAOBlurSwapChainFramebuffers[x] = VK_NULL_HANDLE;
//    }
//
//    CreateRenderPass(engine);
//    CreateRendererFramebuffers(engine);
//}
//
//void GBufferRenderPass::Destroy(VulkanEngine& engine)
//{
//    GPositionTexture->Delete(engine);
//    GNormalTexture->Delete(engine);
//    GAlbedoTexture->Delete(engine);
//    BloomTexture->Delete(engine);
//    DepthTexture->Delete(engine);
//    SSAOTexture->Delete(engine);
//    SSAOBlurTexture->Delete(engine);
//
//    gBufferPipeline->Destroy(engine);
//    AnimatedGBufferPipeline->Destroy(engine);
//    ssaoPipeline->Destroy(engine);
//    ssaoBlurPipeline->Destroy(engine);
//
//    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
//    RenderPass = VK_NULL_HANDLE;
//
//    vkDestroyRenderPass(engine.Device, SSAORenderPass, nullptr);
//    SSAORenderPass = VK_NULL_HANDLE;
//
//    vkDestroyRenderPass(engine.Device, SSAOBlurRenderPass, nullptr);
//    SSAOBlurRenderPass = VK_NULL_HANDLE;
//
//    for (int x = 0; x < engine.GetSwapChainImageCount(); x++)
//    {
//        vkDestroyFramebuffer(engine.Device, SwapChainFramebuffers[x], nullptr);
//        SwapChainFramebuffers[x] = VK_NULL_HANDLE;
//
//        vkDestroyFramebuffer(engine.Device, SSAOSwapChainFramebuffers[x], nullptr);
//        SSAOSwapChainFramebuffers[x] = VK_NULL_HANDLE;
//
//        vkDestroyFramebuffer(engine.Device, SSAOBlurSwapChainFramebuffers[x], nullptr);
//        SSAOBlurSwapChainFramebuffers[x] = VK_NULL_HANDLE;
//    }
//}