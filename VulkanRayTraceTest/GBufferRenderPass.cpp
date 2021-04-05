#include "GBufferRenderPass.h"

GBufferRenderPass::GBufferRenderPass()
{
}

GBufferRenderPass::GBufferRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
    GPositionTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
    GNormalTexture = std::make_shared<RenderedGBufferNormalTexture>(engine);
    GAlbedoTexture = std::make_shared<RenderedGBufferAlbedoTexture>(engine);
    GBloomTexture = std::make_shared<RenderedColorTexture>(engine);
    DepthTexture = std::make_shared<RenderedDepthTexture>(engine);

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpShaderPipeLine(engine);
    SetUpDescriptorSets(engine, assetManager, sceneData);
}

GBufferRenderPass::~GBufferRenderPass()
{
}

void GBufferRenderPass::SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager.GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager.GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager.GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager.GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, assetManager.GetMaterialDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, assetManager.GetTextureBufferDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, assetManager.Get3DTextureBufferDescriptorCount()));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void GBufferRenderPass::SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMaterialDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, assetManager.GetTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, assetManager.Get3DTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_MISS_BIT_KHR, 1 });
    DescriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void GBufferRenderPass::SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
    DescriptorSets = engine.CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

    VkDescriptorBufferInfo SceneDataBufferInfo = engine.AddBufferDescriptor(sceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = assetManager.GetMeshPropertiesListDescriptors();
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList = assetManager.GetVertexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = assetManager.GetIndexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = assetManager.GetTransformBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = assetManager.GetMaterialBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = assetManager.GetTextureBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = assetManager.Get3DTextureBufferListDescriptor();
    VkDescriptorImageInfo CubeMapImage = assetManager.GetSkyBoxTextureBufferListDescriptor();

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(2, DescriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(3, DescriptorSets, MeshPropertyDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(4, DescriptorSets, VertexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(5, DescriptorSets, IndexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(6, DescriptorSets, TransformBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(7, DescriptorSets, MaterialBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(8, DescriptorSets, TextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(9, DescriptorSets, Texture3DBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(10, DescriptorSets, CubeMapImage));

    vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void GBufferRenderPass::SetUpShaderPipeLine(VulkanEngine& engine)
{
    std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
    PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/GBufferVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/GBufferFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

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
   viewport.width = (float)engine.SwapChain.GetSwapChainResolution().width;
   viewport.height = (float)engine.SwapChain.GetSwapChainResolution().height;
   viewport.minDepth = 0.0f;
   viewport.maxDepth = 1.0f;

   VkRect2D scissor{};
   scissor.offset = { 0, 0 };
   scissor.extent = engine.SwapChain.GetSwapChainResolution();

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

   std::array<VkPipelineColorBlendAttachmentState, 5> ColorAttachment = {};
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

   ColorAttachment[4].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
   ColorAttachment[4].blendEnable = VK_TRUE;
   ColorAttachment[4].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
   ColorAttachment[4].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
   ColorAttachment[4].colorBlendOp = VK_BLEND_OP_ADD;
   ColorAttachment[4].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
   ColorAttachment[4].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
   ColorAttachment[4].alphaBlendOp = VK_BLEND_OP_SUBTRACT;

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
   pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;

   if (vkCreatePipelineLayout(engine.Device, &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS) {
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
   pipelineInfo.renderPass = RenderPass;
   pipelineInfo.subpass = 0;
   pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

   if (vkCreateGraphicsPipelines(engine.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
       throw std::runtime_error("Failed to create gbuffer pipeline.");
   }

   for (auto& shader : PipelineShaderStageList)
   {
       vkDestroyShaderModule(engine.Device, shader.module, nullptr);
   }
}

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
