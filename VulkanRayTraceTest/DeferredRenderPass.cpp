#include "DeferredRenderPass.h"
#include "GraphicsPipeline.h"

DeferredRenderPass::DeferredRenderPass()
{
}

DeferredRenderPass::DeferredRenderPass(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
    //GBuffer
    {
        gBufferRenderPass.GPositionTexture = std::make_shared<RenderedGBufferPositionTexture>(engine);
        gBufferRenderPass.GAlbedoTexture = std::make_shared<RenderedGBufferAlbedoTexture>(engine);
        gBufferRenderPass.GNormalTexture = std::make_shared<RenderedGBufferNormalTexture>(engine);
        gBufferRenderPass.GBloomTexture = std::make_shared<RenderedColorTexture>(engine);
        gBufferRenderPass.DepthTexture = std::make_shared<RenderedDepthTexture>(engine);
    }

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpShaderPipeLine(engine);
    SetUpDescriptorSets(engine, assetManager, sceneData);
    SetUpCommandBuffers(engine);
}

DeferredRenderPass::~DeferredRenderPass()
{
}

void DeferredRenderPass::SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager)
{
    //gbuffer
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
        gBufferRenderPass.DescriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
    }
    //deferred Renderer
    {
        std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
        DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
        DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, assetManager.GetTextureBufferDescriptorCount()));
        deferredRenderPass.DescriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
    }
}

void DeferredRenderPass::SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager)
{
    //gbuffer
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
        gBufferRenderPass.DescriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
    }
    //deferred Renderer
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
        LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 4 });
        LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, assetManager.Get3DTextureBufferDescriptorCount() });
        LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_MISS_BIT_KHR, 1 });
        deferredRenderPass.DescriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
    }
}

void DeferredRenderPass::SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
    //gbuffer
    {
        gBufferRenderPass.DescriptorSets = engine.CreateDescriptorSets(gBufferRenderPass.DescriptorPool, gBufferRenderPass.DescriptorSetLayout);

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
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(2, gBufferRenderPass.DescriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(3, gBufferRenderPass.DescriptorSets, MeshPropertyDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(4, gBufferRenderPass.DescriptorSets, VertexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(5, gBufferRenderPass.DescriptorSets, IndexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(6, gBufferRenderPass.DescriptorSets, TransformBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(7, gBufferRenderPass.DescriptorSets, MaterialBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
        DescriptorList.emplace_back(engine.AddTextureDescriptorSet(8, gBufferRenderPass.DescriptorSets, TextureBufferInfo));
        DescriptorList.emplace_back(engine.AddTextureDescriptorSet(9, gBufferRenderPass.DescriptorSets, Texture3DBufferInfo));
        DescriptorList.emplace_back(engine.AddTextureDescriptorSet(10, gBufferRenderPass.DescriptorSets, CubeMapImage));

        vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
    }
    //deferred Renderer
    {
        deferredRenderPass.DescriptorSets = engine.CreateDescriptorSets(deferredRenderPass.DescriptorPool, deferredRenderPass.DescriptorSetLayout);

        VkDescriptorBufferInfo SceneDataBufferInfo = engine.AddBufferDescriptor(sceneData->VulkanBufferData);

        std::vector<VkDescriptorImageInfo> TextureBufferInfo;
        VkDescriptorImageInfo GPositionTextureBufferInfo{};
        GPositionTextureBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        GPositionTextureBufferInfo.imageView = gBufferRenderPass.GPositionTexture->GetTextureView();
        GPositionTextureBufferInfo.sampler = gBufferRenderPass.GPositionTexture->GetTextureSampler();
        TextureBufferInfo.emplace_back(GPositionTextureBufferInfo);

        VkDescriptorImageInfo GNormalTextureImage{};
        GNormalTextureImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        GNormalTextureImage.imageView = gBufferRenderPass.GNormalTexture->GetTextureView();
        GNormalTextureImage.sampler = gBufferRenderPass.GNormalTexture->GetTextureSampler();
        TextureBufferInfo.emplace_back(GNormalTextureImage);

        VkDescriptorImageInfo GAlbedoTextureImage{};
        GAlbedoTextureImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        GAlbedoTextureImage.imageView = gBufferRenderPass.GAlbedoTexture->GetTextureView();
        GAlbedoTextureImage.sampler = gBufferRenderPass.GAlbedoTexture->GetTextureSampler();
        TextureBufferInfo.emplace_back(GAlbedoTextureImage);

        VkDescriptorImageInfo GBloomTextureImage{};
        GBloomTextureImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        GBloomTextureImage.imageView = gBufferRenderPass.GBloomTexture->GetTextureView();
        GBloomTextureImage.sampler = gBufferRenderPass.GBloomTexture->GetTextureSampler();
        TextureBufferInfo.emplace_back(GBloomTextureImage);

        std::vector<VkWriteDescriptorSet> DescriptorList;
        DescriptorList.emplace_back(engine.AddBufferDescriptorSet(2, deferredRenderPass.DescriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
        DescriptorList.emplace_back(engine.AddTextureDescriptorSet(8, deferredRenderPass.DescriptorSets, TextureBufferInfo));
        vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr); 
    }
}

void DeferredRenderPass::SetUpShaderPipeLine(VulkanEngine& engine)
{
    //gbuffer
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

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(ConstMeshInfo);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        pipelineLayoutInfo.pSetLayouts = &gBufferRenderPass.DescriptorSetLayout;

        if (vkCreatePipelineLayout(engine.Device, &pipelineLayoutInfo, nullptr, &gBufferRenderPass.ShaderPipelineLayout) != VK_SUCCESS) {
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
        pipelineInfo.layout = gBufferRenderPass.ShaderPipelineLayout;
        pipelineInfo.renderPass = gBufferRenderPass.RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(engine.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &gBufferRenderPass.ShaderPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create gbuffer pipeline.");
        }

        for (auto& shader : PipelineShaderStageList)
        {
            vkDestroyShaderModule(engine.Device, shader.module, nullptr);
        }
    }
    //deferred Renderer
    {
        std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
        PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/DeferedShaderVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
        PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/DeferedShaderFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

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
        PipelineLayoutInfo.pSetLayouts = &deferredRenderPass.DescriptorSetLayout;

        if (vkCreatePipelineLayout(engine.Device, &PipelineLayoutInfo, nullptr, &deferredRenderPass.ShaderPipelineLayout) != VK_SUCCESS)
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
        PipelineInfo.layout = deferredRenderPass.ShaderPipelineLayout;
        PipelineInfo.renderPass = deferredRenderPass.RenderPass;
        PipelineInfo.subpass = 0;
        PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(engine.Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &deferredRenderPass.ShaderPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create FrameBuffer Pipeline.");
        }

        for (auto& shader : PipelineShaderStageList)
        {
            vkDestroyShaderModule(engine.Device, shader.module, nullptr);
        }
    }
}

void DeferredRenderPass::CreateRenderPass(VulkanEngine& engine)
{
    //gbuffer
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

        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &gBufferRenderPass.RenderPass))
        {
            throw std::runtime_error("failed to create GBuffer RenderPass!");
        }
    }
    //deferred Renderer
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &deferredRenderPass.RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
}

void DeferredRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
{
    //gbuffer
    {
        gBufferRenderPass.SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());

        for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
        {
            std::vector<VkImageView> AttachmentList;
            AttachmentList.emplace_back(gBufferRenderPass.GPositionTexture->View);
            AttachmentList.emplace_back(gBufferRenderPass.GAlbedoTexture->View);
            AttachmentList.emplace_back(gBufferRenderPass.GNormalTexture->View);
            AttachmentList.emplace_back(gBufferRenderPass.GBloomTexture->View);
            AttachmentList.emplace_back(gBufferRenderPass.DepthTexture->View);

            VkFramebufferCreateInfo frameBufferCreateInfo = {};
            frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferCreateInfo.renderPass = gBufferRenderPass.RenderPass;
            frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(AttachmentList.size());
            frameBufferCreateInfo.pAttachments = AttachmentList.data();
            frameBufferCreateInfo.width = engine.SwapChain.GetSwapChainResolution().width;
            frameBufferCreateInfo.height = engine.SwapChain.GetSwapChainResolution().height;
            frameBufferCreateInfo.layers = 1;

            if (vkCreateFramebuffer(engine.Device, &frameBufferCreateInfo, nullptr, &gBufferRenderPass.SwapChainFramebuffers[i]))
            {
                throw std::runtime_error("Failed to create Gbuffer FrameBuffer.");
            }
        }
    }
    //deferred Renderer
    {
        deferredRenderPass.SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());

        for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++) {
            std::array<VkImageView, 1> attachments = {
                engine.SwapChain.GetSwapChainImageViews()[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = deferredRenderPass.RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = engine.SwapChain.GetSwapChainResolution().width;
            framebufferInfo.height = engine.SwapChain.GetSwapChainResolution().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(engine.Device, &framebufferInfo, nullptr, &deferredRenderPass.SwapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
}

void DeferredRenderPass::SetUpCommandBuffers(VulkanEngine& engine)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void DeferredRenderPass::Draw(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(CommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = gBufferRenderPass.RenderPass;
        renderPassInfo.framebuffer = gBufferRenderPass.SwapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = engine.SwapChain.SwapChainResolution;

        std::array<VkClearValue, 5> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[3].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[4].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gBufferRenderPass.ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gBufferRenderPass.ShaderPipelineLayout, 0, 1, &gBufferRenderPass.DescriptorSets, 0, nullptr);

        assetManager.Draw(CommandBuffer, gBufferRenderPass.ShaderPipelineLayout, RendererID);

        vkCmdEndRenderPass(CommandBuffer);
    }

    {
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = deferredRenderPass.RenderPass;
        renderPassInfo.framebuffer = deferredRenderPass.SwapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, deferredRenderPass.ShaderPipeline);
        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, deferredRenderPass.ShaderPipelineLayout, 0, 1, &deferredRenderPass.DescriptorSets, 0, nullptr);
        vkCmdDraw(CommandBuffer, 6, 1, 0, 0);
        vkCmdEndRenderPass(CommandBuffer);
    }
    if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void DeferredRenderPass::UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
    {
        gBufferRenderPass.GPositionTexture->RecreateRendererTexture(engine);
        gBufferRenderPass.GNormalTexture->RecreateRendererTexture(engine);
        gBufferRenderPass.GAlbedoTexture->RecreateRendererTexture(engine);
        gBufferRenderPass.GBloomTexture->RecreateRendererTexture(engine);
        gBufferRenderPass.DepthTexture->RecreateRendererTexture(engine);

        vkDestroyDescriptorPool(engine.Device, gBufferRenderPass.DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, gBufferRenderPass.DescriptorSetLayout, nullptr);
        gBufferRenderPass.DescriptorPool = VK_NULL_HANDLE;
        gBufferRenderPass.DescriptorSetLayout = VK_NULL_HANDLE;

        vkDestroyRenderPass(engine.Device, gBufferRenderPass.RenderPass, nullptr);
        gBufferRenderPass.RenderPass = VK_NULL_HANDLE;

        vkDestroyPipeline(engine.Device, gBufferRenderPass.ShaderPipeline, nullptr);
        vkDestroyPipelineLayout(engine.Device, gBufferRenderPass.ShaderPipelineLayout, nullptr);

        gBufferRenderPass.ShaderPipeline = VK_NULL_HANDLE;
        gBufferRenderPass.ShaderPipelineLayout = VK_NULL_HANDLE;
    }
    {
        vkDestroyDescriptorPool(engine.Device, deferredRenderPass.DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, deferredRenderPass.DescriptorSetLayout, nullptr);
        deferredRenderPass.DescriptorPool = VK_NULL_HANDLE;
        deferredRenderPass.DescriptorSetLayout = VK_NULL_HANDLE;

        vkDestroyRenderPass(engine.Device, deferredRenderPass.RenderPass, nullptr);
        deferredRenderPass.RenderPass = VK_NULL_HANDLE;

        vkDestroyPipeline(engine.Device, deferredRenderPass.ShaderPipeline, nullptr);
        vkDestroyPipelineLayout(engine.Device, deferredRenderPass.ShaderPipelineLayout, nullptr);

        deferredRenderPass.ShaderPipeline = VK_NULL_HANDLE;
        deferredRenderPass.ShaderPipelineLayout = VK_NULL_HANDLE;
    }
    for (auto& framebuffer : gBufferRenderPass.SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }

    CreateRenderPass(engine);
    CreateRendererFramebuffers(engine);
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpShaderPipeLine(engine);
    SetUpDescriptorSets(engine, assetManager, sceneData);
    SetUpCommandBuffers(engine);
}

void DeferredRenderPass::Destroy(VulkanEngine& engine)
{
    {
        gBufferRenderPass.GPositionTexture->Delete(engine);
        gBufferRenderPass.GNormalTexture->Delete(engine);
        gBufferRenderPass.GAlbedoTexture->Delete(engine);
        gBufferRenderPass.GBloomTexture->Delete(engine);
        gBufferRenderPass.DepthTexture->Delete(engine);

        vkDestroyDescriptorPool(engine.Device, gBufferRenderPass.DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, gBufferRenderPass.DescriptorSetLayout, nullptr);
        gBufferRenderPass.DescriptorPool = VK_NULL_HANDLE;
        gBufferRenderPass.DescriptorSetLayout = VK_NULL_HANDLE;

        vkDestroyRenderPass(engine.Device, gBufferRenderPass.RenderPass, nullptr);
        gBufferRenderPass.RenderPass = VK_NULL_HANDLE;

        vkDestroyPipeline(engine.Device, gBufferRenderPass.ShaderPipeline, nullptr);
        vkDestroyPipelineLayout(engine.Device, gBufferRenderPass.ShaderPipelineLayout, nullptr);

        gBufferRenderPass.ShaderPipeline = VK_NULL_HANDLE;
        gBufferRenderPass.ShaderPipelineLayout = VK_NULL_HANDLE;

        for (auto& framebuffer : gBufferRenderPass.SwapChainFramebuffers)
        {
            vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }
    {
        vkDestroyDescriptorPool(engine.Device, deferredRenderPass.DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, deferredRenderPass.DescriptorSetLayout, nullptr);
        deferredRenderPass.DescriptorPool = VK_NULL_HANDLE;
        deferredRenderPass.DescriptorSetLayout = VK_NULL_HANDLE;

        vkDestroyRenderPass(engine.Device, deferredRenderPass.RenderPass, nullptr);
        deferredRenderPass.RenderPass = VK_NULL_HANDLE;

        vkDestroyPipeline(engine.Device, deferredRenderPass.ShaderPipeline, nullptr);
        vkDestroyPipelineLayout(engine.Device, deferredRenderPass.ShaderPipelineLayout, nullptr);

        deferredRenderPass.ShaderPipeline = VK_NULL_HANDLE;
        deferredRenderPass.ShaderPipelineLayout = VK_NULL_HANDLE;

        for (auto& framebuffer : deferredRenderPass.SwapChainFramebuffers)
        {
            vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }
}