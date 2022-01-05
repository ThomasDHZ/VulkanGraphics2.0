#include "PBRReflectionPipeline.h"
#include "Mesh.h"
#include "AssetManager.h"

PBRReflectionPipeline::PBRReflectionPipeline() : GraphicsPipeline()
{
}

PBRReflectionPipeline::PBRReflectionPipeline(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::shared_ptr<CubeSampler> cubeSampler, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList) : GraphicsPipeline()
{
    SetUpDescriptorBindings(IrradianceMap, PrefilerMap, BRDFMap, ShadowMapTextureList, RenderedCubeMap, cubeSampler, SpotLightShadowMapTextureList);
    SetUpShaderPipeLine(renderPass);
}

PBRReflectionPipeline::~PBRReflectionPipeline()
{
}

void PBRReflectionPipeline::SetUpDescriptorBindings(std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::shared_ptr<CubeSampler>  cubeSampler, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList)
{
    uint32_t count = (uint32_t)ShadowMapTextureList.size();
    if (count == 0)
    {
        count = 1;
    }

    std::vector<VkDescriptorImageInfo> ShadowDescriptorImageList;
    if (ShadowMapTextureList.size() == 0)
    {
        VkDescriptorImageInfo nullBuffer;
        nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        nullBuffer.imageView = VK_NULL_HANDLE;
        nullBuffer.sampler = NullSampler;
        ShadowDescriptorImageList.emplace_back(nullBuffer);
    }
    else
    {
        for (auto texture : ShadowMapTextureList)
        {
            VkDescriptorImageInfo DescriptorImage{};
            DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            DescriptorImage.imageView = texture->GetTextureView();
            DescriptorImage.sampler = texture->GetTextureSampler();
            ShadowDescriptorImageList.emplace_back(DescriptorImage);
        }
    }

    //std::vector<VkDescriptorImageInfo> PointShadowDescriptorImageList;
    //if (ShadowMapTextureList.size() == 0)
    //{
    //    VkDescriptorImageInfo nullBuffer;
    //    nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    nullBuffer.imageView = VK_NULL_HANDLE;
    //    nullBuffer.sampler = NullSampler;
    //    PointShadowDescriptorImageList.emplace_back(nullBuffer);
    //}
    //else
    //{
    //    for (auto texture : RenderedCubeMap)
    //    {
    //        VkDescriptorImageInfo DescriptorImage{};
    //        DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //        DescriptorImage.imageView = texture->GetTextureView();
    //        DescriptorImage.sampler = texture->GetTextureSampler();
    //        PointShadowDescriptorImageList.emplace_back(DescriptorImage);
    //    }
    //}

    std::vector<VkDescriptorImageInfo> SpotLightShadowDescriptorImageList;
    if (ShadowMapTextureList.size() == 0)
    {
        VkDescriptorImageInfo nullBuffer;
        nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        nullBuffer.imageView = VK_NULL_HANDLE;
        nullBuffer.sampler = NullSampler;
        SpotLightShadowDescriptorImageList.emplace_back(nullBuffer);
    }
    else
    {
        for (auto texture : SpotLightShadowMapTextureList)
        {
            VkDescriptorImageInfo DescriptorImage{};
            DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            DescriptorImage.imageView = texture->GetTextureView();
            DescriptorImage.sampler = texture->GetTextureSampler();
            SpotLightShadowDescriptorImageList.emplace_back(DescriptorImage);
        }
    }

    VkDescriptorBufferInfo SceneDataBufferInfo = AddBufferDescriptor(AssetManagerPtr::GetAssetPtr()->SceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = AssetManagerPtr::GetAssetPtr()->GetMeshPropertiesDescriptorsList();
    std::vector<VkDescriptorBufferInfo> DirectionalLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightBufferDescriptorList();
    std::vector<VkDescriptorBufferInfo> PointLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightBufferDescriptorList();
    std::vector<VkDescriptorBufferInfo> SpotLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightBufferDescriptorList();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = AssetManagerPtr::GetAssetPtr()->GetTexture2DBufferDescriptorList();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = AssetManagerPtr::GetAssetPtr()->GetTexture3DBufferDescriptorList();
    std::vector<VkDescriptorImageInfo> CubeMapImage = AssetManagerPtr::GetAssetPtr()->GetCubeMapTextureBufferDescriptorList();
    std::vector<VkDescriptorBufferInfo> SphereAreaLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetSphereAreaLightDescriptorList();
    std::vector<VkDescriptorBufferInfo> TubeAreaLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetTubeAreaLightDescriptorList();
    std::vector<VkDescriptorBufferInfo> RectangleAreaBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetRectangleAreaLightDescriptorList();
    VkDescriptorImageInfo IrradianceMapImage = AddTextureDescriptor(IrradianceMap->View, IrradianceMap->Sampler);
    VkDescriptorImageInfo PrefilerMapImage = AddTextureDescriptor(PrefilerMap->View, PrefilerMap->Sampler);
    VkDescriptorImageInfo BRDFMapImage = AddTextureDescriptor(BRDFMap->View, BRDFMap->Sampler);
    VkDescriptorBufferInfo ViewMatrixBufferInfo = AddBufferDescriptor(cubeSampler->VulkanBufferData);
    VkDescriptorImageInfo RenderedCubeMapImage = AddTextureDescriptor(RenderedCubeMap->View, RenderedCubeMap->Sampler);

    AddUniformBufferDescriptorSetBinding(0, SceneDataBufferInfo);
    AddStorageBufferDescriptorSetBinding(1, MeshPropertyDataBufferInfo, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount());
    AddStorageBufferDescriptorSetBinding(2, DirectionalLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetDirectionalLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(3, PointLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetPointLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(4, SpotLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetSpotLightDescriptorCount());
    AddTextureDescriptorSetBinding(5, TextureBufferInfo, AssetManagerPtr::GetAssetPtr()->GetTextureBufferDescriptorCount());
    AddTextureDescriptorSetBinding(6, Texture3DBufferInfo, AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferDescriptorCount());
    AddTextureDescriptorSetBinding(7, CubeMapImage, AssetManagerPtr::GetAssetPtr()->GetCubeMapBufferDescriptorCount());
    AddStorageBufferDescriptorSetBinding(8, SphereAreaLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetSphereAreaLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(9, TubeAreaLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetTubeAreaLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(10, RectangleAreaBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetRectangleAreaLightDescriptorCount());
    AddTextureDescriptorSetBinding(11, IrradianceMapImage);
    AddTextureDescriptorSetBinding(12, PrefilerMapImage);
    AddTextureDescriptorSetBinding(13, BRDFMapImage);
    AddTextureDescriptorSetBinding(14, ShadowDescriptorImageList, count);
    AddTextureDescriptorSetBinding(15, RenderedCubeMapImage);
    AddTextureDescriptorSetBinding(16, SpotLightShadowDescriptorImageList, count);
    AddUniformBufferDescriptorSetBinding(17, ViewMatrixBufferInfo);
    SubmitDescriptorSet();
}

void PBRReflectionPipeline::SetUpShaderPipeLine(const VkRenderPass& renderPass)
{
    std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
    PipelineShaderStageList.emplace_back(CreateShader("shaders/PBRReflectionShaderVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    PipelineShaderStageList.emplace_back(CreateShader("shaders/PBRReflectionShaderFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

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

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

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
    multisampling.sampleShadingEnable = VK_TRUE;
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

void PBRReflectionPipeline::UpdateGraphicsPipeLine(const VkRenderPass& renderPass, std::shared_ptr<RenderedCubeMapTexture> IrradianceMap, std::shared_ptr<RenderedCubeMapTexture> PrefilerMap, std::shared_ptr<RenderedColorTexture> BRDFMap, std::vector<std::shared_ptr<RenderedDepthTexture>> ShadowMapTextureList, std::shared_ptr<RenderedCubeMapDepthTexture> RenderedCubeMap, std::shared_ptr<CubeSampler> cubeSampler, std::vector<std::shared_ptr<RenderedDepthTexture>>& SpotLightShadowMapTextureList)
{
    GraphicsPipeline::UpdateGraphicsPipeLine();
    SetUpDescriptorBindings(IrradianceMap, PrefilerMap, BRDFMap, ShadowMapTextureList, RenderedCubeMap, cubeSampler, SpotLightShadowMapTextureList);
    SetUpShaderPipeLine(renderPass);
}
