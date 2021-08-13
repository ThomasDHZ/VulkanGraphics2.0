#include "WireFramePipeline.h"
#include "Vertex.h"

WireFramePipeline::WireFramePipeline() : GraphicsPipeline()
{
}

WireFramePipeline::WireFramePipeline(const VkRenderPass& renderPass) : GraphicsPipeline()
{
    SetUpDescriptorPool();
    SetUpDescriptorLayout();
    SetUpShaderPipeLine(renderPass);
    SetUpDescriptorSets();
}

WireFramePipeline::~WireFramePipeline()
{
}

void WireFramePipeline::SetUpDescriptorPool()
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMaterialDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManagerPtr::GetAssetPtr()->GetTextureBufferDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPool = EnginePtr::GetEnginePtr()->CreateDescriptorPool(DescriptorPoolList);
}

void WireFramePipeline::SetUpDescriptorLayout()
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMaterialDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, AssetManagerPtr::GetAssetPtr()->GetTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_MISS_BIT_KHR, 1 });
    DescriptorSetLayout = EnginePtr::GetEnginePtr()->CreateDescriptorSetLayout(LayoutBindingInfo);
}

void WireFramePipeline::SetUpDescriptorSets()
{
    DescriptorSets = EnginePtr::GetEnginePtr()->CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);
    AssetManagerPtr::GetAssetPtr()->SceneData->Update();

    VkDescriptorBufferInfo SceneDataBufferInfo = EnginePtr::GetEnginePtr()->AddBufferDescriptor(AssetManagerPtr::GetAssetPtr()->SceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = AssetManagerPtr::GetAssetPtr()->GetMeshPropertiesListDescriptors();
    std::vector<VkDescriptorBufferInfo> DirectionalLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> PointLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> SpotLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = AssetManagerPtr::GetAssetPtr()->GetIndexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = AssetManagerPtr::GetAssetPtr()->GetTransformBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = AssetManagerPtr::GetAssetPtr()->GetMaterialBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = AssetManagerPtr::GetAssetPtr()->GetTextureBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferListDescriptor();
    VkDescriptorImageInfo CubeMapImage = AssetManagerPtr::GetAssetPtr()->GetSkyBoxTextureBufferListDescriptor();

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(0, DescriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(1, DescriptorSets, MeshPropertyDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(2, DescriptorSets, DirectionalLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(3, DescriptorSets, PointLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(4, DescriptorSets, SpotLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(5, DescriptorSets, TransformBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(6, DescriptorSets, MaterialBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(7, DescriptorSets, TextureBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(8, DescriptorSets, Texture3DBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(9, DescriptorSets, CubeMapImage));

    vkUpdateDescriptorSets(VulkanPtr::GetDevice(), static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void WireFramePipeline::SetUpShaderPipeLine(const VkRenderPass& renderPass)
{
    std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
    PipelineShaderStageList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/WireFrameShaderVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    PipelineShaderStageList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/WireFrameShaderFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

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
    viewport.width = (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
    viewport.height = (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution();

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
    rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
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

    std::array<VkPipelineColorBlendAttachmentState, 2> ColorAttachment = {};
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

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ConstMeshInfo);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;

    if (vkCreatePipelineLayout(VulkanPtr::GetDevice(), &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create gbuffer pipeline layout.");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(PipelineShaderStageList.size());
    pipelineInfo.pStages = PipelineShaderStageList.data();
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

    if (vkCreateGraphicsPipelines(VulkanPtr::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create gbuffer pipeline.");
    }

    for (auto& shader : PipelineShaderStageList)
    {
        vkDestroyShaderModule(VulkanPtr::GetDevice(), shader.module, nullptr);
    }
}

void WireFramePipeline::UpdateGraphicsPipeLine(const VkRenderPass& renderPass)
{
    GraphicsPipeline::UpdateGraphicsPipeLine();
    SetUpDescriptorPool();
    SetUpDescriptorLayout();
    SetUpShaderPipeLine(renderPass);
    SetUpDescriptorSets();
}
