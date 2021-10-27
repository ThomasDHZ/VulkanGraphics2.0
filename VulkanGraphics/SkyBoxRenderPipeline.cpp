#include "SkyBoxRenderPipeline.h"
#include "Vertex.h"

SkyBoxRenderPipeline::SkyBoxRenderPipeline() : GraphicsPipeline()
{
}

SkyBoxRenderPipeline::SkyBoxRenderPipeline(const VkRenderPass& renderPass, VkSampleCountFlagBits SampleCount) : GraphicsPipeline()
{
    SetUpDescriptorPool();
    SetUpDescriptorLayout();
    SetUpShaderPipeLine(renderPass, SampleCount);
    SetUpDescriptorSets();
}

SkyBoxRenderPipeline::~SkyBoxRenderPipeline()
{
}

void SkyBoxRenderPipeline::SetUpDescriptorPool()
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {}; 
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManagerPtr::GetAssetPtr()->GetCubeMapBufferDescriptorCount()));
    DescriptorPool = EnginePtr::GetEnginePtr()->CreateDescriptorPool(DescriptorPoolList);
}

void SkyBoxRenderPipeline::SetUpDescriptorLayout()
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_MISS_BIT_KHR, AssetManagerPtr::GetAssetPtr()->GetCubeMapBufferDescriptorCount() });
    DescriptorSetLayout = EnginePtr::GetEnginePtr()->CreateDescriptorSetLayout(LayoutBindingInfo);
}

void SkyBoxRenderPipeline::SetUpDescriptorSets()
{
    DescriptorSet = EnginePtr::GetEnginePtr()->CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

    std::vector<VkDescriptorImageInfo> TextureBufferInfo = AssetManagerPtr::GetAssetPtr()->textureManager->GetCubeMapTextureBufferListDescriptor();

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(0, DescriptorSet, TextureBufferInfo));
    vkUpdateDescriptorSets(EnginePtr::GetEnginePtr()->Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void SkyBoxRenderPipeline::SetUpShaderPipeLine(const VkRenderPass& renderPass, VkSampleCountFlagBits SampleCount)
{
    std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
    PipelineShaderStageList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/SkyBoxRenderShaderVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    PipelineShaderStageList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/SkyBoxRenderShaderFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

    VkPipelineVertexInputStateCreateInfo SkyBoxvertexInputInfo = {};
    SkyBoxvertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto SkyBoxbindingDescription = Vertex::GetBindingDescription();
    auto SkyBoxattributeDescriptions = Vertex::GetAttributeDescriptions();

    SkyBoxvertexInputInfo.vertexBindingDescriptionCount = 1;
    SkyBoxvertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(SkyBoxattributeDescriptions.size());
    SkyBoxvertexInputInfo.pVertexBindingDescriptions = &SkyBoxbindingDescription;
    SkyBoxvertexInputInfo.pVertexAttributeDescriptions = SkyBoxattributeDescriptions.data();

    VkPipelineDepthStencilStateCreateInfo SkyBoxdepthStencil = {};
    SkyBoxdepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    SkyBoxdepthStencil.depthTestEnable = VK_TRUE;
    SkyBoxdepthStencil.depthWriteEnable = VK_TRUE;
    SkyBoxdepthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    SkyBoxdepthStencil.depthBoundsTestEnable = VK_FALSE;
    SkyBoxdepthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
    viewport.height = (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution();

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.rasterizationSamples = SampleCount;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    int ColorAttachmentCount = 2;
    std::vector<VkPipelineColorBlendAttachmentState> ColorAttachmentList(ColorAttachmentCount, colorBlendAttachment);

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = static_cast<uint32_t>(ColorAttachmentList.size());
    colorBlending.pAttachments = ColorAttachmentList.data();
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ConstSkyBoxView);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(VulkanPtr::GetDevice(), &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create FrameBuffer Pipeline Layout.");
    }

    VkGraphicsPipelineCreateInfo SkyBoxpipelineInfo = {};
    SkyBoxpipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    SkyBoxpipelineInfo.stageCount = static_cast<uint32_t>(PipelineShaderStageList.size());
    SkyBoxpipelineInfo.pStages = PipelineShaderStageList.data();
    SkyBoxpipelineInfo.pVertexInputState = &SkyBoxvertexInputInfo;
    SkyBoxpipelineInfo.pInputAssemblyState = &inputAssembly;
    SkyBoxpipelineInfo.pViewportState = &viewportState;
    SkyBoxpipelineInfo.pRasterizationState = &rasterizer;
    SkyBoxpipelineInfo.pMultisampleState = &multisampling;
    SkyBoxpipelineInfo.pDepthStencilState = &SkyBoxdepthStencil;
    SkyBoxpipelineInfo.pColorBlendState = &colorBlending;
    SkyBoxpipelineInfo.layout = ShaderPipelineLayout;
    SkyBoxpipelineInfo.renderPass = renderPass;
    SkyBoxpipelineInfo.subpass = 0;
    SkyBoxpipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(VulkanPtr::GetDevice(), VK_NULL_HANDLE, 1, &SkyBoxpipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create FrameBuffer Pipeline.");
    }

    for (auto& shader : PipelineShaderStageList)
    {
        vkDestroyShaderModule(VulkanPtr::GetDevice(), shader.module, nullptr);
    }
}

void SkyBoxRenderPipeline::UpdateGraphicsPipeLine(const VkRenderPass& renderPass, VkSampleCountFlagBits SampleCount)
{
    GraphicsPipeline::UpdateGraphicsPipeLine();
    SetUpDescriptorPool();
    SetUpDescriptorLayout();
    SetUpShaderPipeLine(renderPass, SampleCount);
    SetUpDescriptorSets();
}