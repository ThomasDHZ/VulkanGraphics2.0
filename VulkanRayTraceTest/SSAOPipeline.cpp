#include "SSAOPipeline.h"
#include "Vertex.h"

SSAOPipeline::SSAOPipeline() : GraphicsPipeline()
{
}

SSAOPipeline::SSAOPipeline(const VkRenderPass& renderPass, SSAOTextureList& textures) : GraphicsPipeline()
{
    SetUpDescriptorPool(textures);
    SetUpDescriptorLayout(textures);
    SetUpShaderPipeLine(renderPass);
    SetUpDescriptorSets(textures);
}

SSAOPipeline::~SSAOPipeline()
{
}

void SSAOPipeline::SetUpDescriptorPool(SSAOTextureList& textures)
{
    uint32_t size = textures.KernalSampleBufferList.size();
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, size));
    DescriptorPool = EnginePtr::GetEnginePtr()->CreateDescriptorPool(DescriptorPoolList);
}

void SSAOPipeline::SetUpDescriptorLayout(SSAOTextureList& textures)
{
    uint32_t size = textures.KernalSampleBufferList.size();
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, size });
    DescriptorSetLayout = EnginePtr::GetEnginePtr()->CreateDescriptorSetLayout(LayoutBindingInfo);
}
std::vector<VkDescriptorBufferInfo> SSAOPipeline::GetKernallBufferListDescriptor(std::vector<std::shared_ptr<VulkanBuffer>> SamplePoints)
{
    std::vector<VkDescriptorBufferInfo> SamplePointsBufferList{};
    for (int x = 0; x < SamplePoints.size(); x++)
    {
        VkDescriptorBufferInfo MaterialBufferInfo = {};
        MaterialBufferInfo.buffer = SamplePoints[x]->GetBuffer();
        MaterialBufferInfo.offset = 0;
        MaterialBufferInfo.range = VK_WHOLE_SIZE;
        SamplePointsBufferList.emplace_back(MaterialBufferInfo);
    }

    return SamplePointsBufferList;
}
void SSAOPipeline::SetUpDescriptorSets(SSAOTextureList& textures)
{
    DescriptorSets = EnginePtr::GetEnginePtr()->CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

    VkDescriptorImageInfo GPositionTextureBufferInfo = EnginePtr::GetEnginePtr()->AddTextureDescriptor(textures.GPositionTexture->View, textures.GPositionTexture->Sampler);
    VkDescriptorImageInfo GNormalTextureBufferInfo = EnginePtr::GetEnginePtr()->AddTextureDescriptor(textures.GNormalTexture->View, textures.GNormalTexture->Sampler);
    VkDescriptorImageInfo NormalMapTextureBufferInfo = EnginePtr::GetEnginePtr()->AddTextureDescriptor(textures.NormalMapTexture->View, textures.NormalMapTexture->Sampler);
    VkDescriptorImageInfo NoiseTextureBufferInfo = EnginePtr::GetEnginePtr()->AddTextureDescriptor(textures.NoiseTexture->View, textures.NoiseTexture->Sampler);
    std::vector<VkDescriptorBufferInfo> KernalBufferList = GetKernallBufferListDescriptor(textures.KernalSampleBufferList);

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(0, DescriptorSets, GPositionTextureBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(1, DescriptorSets, GNormalTextureBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(2, DescriptorSets, NormalMapTextureBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(3, DescriptorSets, NoiseTextureBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(4, DescriptorSets, KernalBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    vkUpdateDescriptorSets(VulkanPtr::GetDevice(), static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void SSAOPipeline::SetUpShaderPipeLine(const VkRenderPass& renderPass)
{
    std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
    PipelineShaderStageList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/SSAOShaderVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    PipelineShaderStageList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/SSAOShaderFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;

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
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo ColorBlending = {};
    ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = 1;
    ColorBlending.pAttachments = &colorBlendAttachment;
    ColorBlending.blendConstants[0] = 0.0f;
    ColorBlending.blendConstants[1] = 0.0f;
    ColorBlending.blendConstants[2] = 0.0f;
    ColorBlending.blendConstants[3] = 0.0f;

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SSAOProperties);

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

    VkGraphicsPipelineCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = static_cast<uint32_t>(PipelineShaderStageList.size());
    PipelineInfo.pStages = PipelineShaderStageList.data();
    PipelineInfo.pVertexInputState = &vertexInputInfo;
    PipelineInfo.pInputAssemblyState = &inputAssembly;
    PipelineInfo.pViewportState = &viewportState;
    PipelineInfo.pRasterizationState = &rasterizer;
    PipelineInfo.pMultisampleState = &multisampling;
    PipelineInfo.pDepthStencilState = &depthStencil;
    PipelineInfo.pColorBlendState = &ColorBlending;
    PipelineInfo.layout = ShaderPipelineLayout;
    PipelineInfo.renderPass = renderPass;
    PipelineInfo.subpass = 0;
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(VulkanPtr::GetDevice(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create FrameBuffer Pipeline.");
    }

    for (auto& shader : PipelineShaderStageList)
    {
        vkDestroyShaderModule(VulkanPtr::GetDevice(), shader.module, nullptr);
    }
}

void SSAOPipeline::UpdateGraphicsPipeLine(const VkRenderPass& renderPass, SSAOTextureList& textures)
{
    GraphicsPipeline::UpdateGraphicsPipeLine();
    SetUpDescriptorPool(textures);
    SetUpDescriptorLayout(textures);
    SetUpShaderPipeLine(renderPass);
    SetUpDescriptorSets(textures);
}
