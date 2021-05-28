#include "HybridFrameBufferPipeline.h"
#include "Vertex.h"

HybridFrameBufferPipeline::HybridFrameBufferPipeline() : GraphicsPipeline()
{
}

HybridFrameBufferPipeline::HybridFrameBufferPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, HybridFrameBufferTextures& HybridTextures) : GraphicsPipeline()
{
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpShaderPipeLine(engine, renderPass);
    SetUpDescriptorSets(engine, assetManager, renderPass, HybridTextures);
}

HybridFrameBufferPipeline::~HybridFrameBufferPipeline()
{
}

void HybridFrameBufferPipeline::SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager->lightManager.GetDirectionalLightDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager->lightManager.GetPointLightDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager->lightManager.GetSpotLightDescriptorCount()));
    DescriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void HybridFrameBufferPipeline::SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 11, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 12, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 13, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager->lightManager.GetDirectionalLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 14, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager->lightManager.GetPointLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 15, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager->lightManager.GetSpotLightDescriptorCount() });
    DescriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void HybridFrameBufferPipeline::SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, HybridFrameBufferTextures& HybridTextures)
{
    DescriptorSets = engine.CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

    VkDescriptorImageInfo PositionTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.PositionTexture->View, HybridTextures.PositionTexture->Sampler);
    VkDescriptorImageInfo AlebdoTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.AlebdoTexture->View, HybridTextures.AlebdoTexture->Sampler);
    VkDescriptorImageInfo NormalTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.NormalTexture->View, HybridTextures.NormalTexture->Sampler);
    VkDescriptorImageInfo TangentTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.TangentTexture->View, HybridTextures.TangentTexture->Sampler);
    VkDescriptorImageInfo BiTangentTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.BiTangentTexture->View, HybridTextures.BiTangentTexture->Sampler);
    VkDescriptorImageInfo ShadowTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.ShadowTexture->View, HybridTextures.ShadowTexture->Sampler);
    VkDescriptorImageInfo ReflectionTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.ReflectionTexture->View, HybridTextures.ReflectionTexture->Sampler);
    VkDescriptorImageInfo SSAOTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.SSA0Texture->View, HybridTextures.SSA0Texture->Sampler);
    VkDescriptorImageInfo SkyBoxTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.SkyBoxTexture->View, HybridTextures.SkyBoxTexture->Sampler);
    VkDescriptorImageInfo BloomTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.BloomTexture->View, HybridTextures.BloomTexture->Sampler);
    VkDescriptorImageInfo NormalMapTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.NormalMapTexture->View, HybridTextures.NormalMapTexture->Sampler);
    VkDescriptorImageInfo SpecularMapTextureBufferInfo = engine.AddTextureDescriptor(HybridTextures.SpecularMapTexture->View, HybridTextures.SpecularMapTexture->Sampler);
    VkDescriptorBufferInfo SceneDataBufferInfo = engine.AddBufferDescriptor(assetManager->SceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> DirectionalLightBufferInfoList = assetManager->lightManager.GetDirectionalLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> PointLightBufferInfoList = assetManager->lightManager.GetPointLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> SpotLightBufferInfoList = assetManager->lightManager.GetSpotLightBufferListDescriptor();

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(0, DescriptorSets, PositionTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(1, DescriptorSets, AlebdoTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(2, DescriptorSets, NormalTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(3, DescriptorSets, TangentTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(4, DescriptorSets, BiTangentTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(5, DescriptorSets, ShadowTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(6, DescriptorSets, ReflectionTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(7, DescriptorSets, SSAOTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(8, DescriptorSets, SkyBoxTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(9, DescriptorSets, BloomTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(10, DescriptorSets, NormalMapTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(11, DescriptorSets, SpecularMapTextureBufferInfo));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(12, DescriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(13, DescriptorSets, DirectionalLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(14, DescriptorSets, PointLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(14, DescriptorSets, SpotLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void HybridFrameBufferPipeline::SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass)
{
    std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
    PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/HybridFrameBufferVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/HybridFrameBufferFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

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
    viewport.width = (float)engine.SwapChain.GetSwapChainResolution().width;
    viewport.height = (float)engine.SwapChain.GetSwapChainResolution().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = engine.SwapChain.GetSwapChainResolution();

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

    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 1;
    PipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;

    if (vkCreatePipelineLayout(engine.Device, &PipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS)
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

    if (vkCreateGraphicsPipelines(engine.Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create FrameBuffer Pipeline.");
    }

    for (auto& shader : PipelineShaderStageList)
    {
        vkDestroyShaderModule(engine.Device, shader.module, nullptr);
    }
}

void HybridFrameBufferPipeline::UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, HybridFrameBufferTextures& HybridTextures)
{
    GraphicsPipeline::UpdateGraphicsPipeLine(engine);
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpShaderPipeLine(engine, renderPass);
    SetUpDescriptorSets(engine, assetManager, renderPass, HybridTextures);
}
