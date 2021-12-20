#include "RayTracedPipeline.h"

RayTracedPipeline::RayTracedPipeline() : RayTracingGraphicsPipeline()
{
}

RayTracedPipeline::RayTracedPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture) : RayTracingGraphicsPipeline(engine)
{
    SetUpDescriptorBindings(engine, assetManager, topLevelAS, RayTracedTexture);
    SetUpPipeline(engine);
    SetUpShaderBindingTable(engine);
}

RayTracedPipeline::~RayTracedPipeline()
{
}

void RayTracedPipeline::UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture)
{
    RayTracingGraphicsPipeline::UpdateGraphicsPipeLine();
    raygenShaderBindingTable.DestoryBuffer();
    missShaderBindingTable.DestoryBuffer();
    hitShaderBindingTable.DestoryBuffer();

    SetUpDescriptorBindings(engine, assetManager, topLevelAS, RayTracedTexture);
    SetUpPipeline(engine);
    SetUpShaderBindingTable(engine);
}

void RayTracedPipeline::SetUpDescriptorBindings(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture)
{
    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = AddAcclerationStructureBinding(topLevelAS.handle);
    VkDescriptorImageInfo RayTracedTextureMaskDescriptor = AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, RayTracedTexture->View);
    VkDescriptorBufferInfo SceneDataBufferInfo = AddBufferDescriptor(AssetManagerPtr::GetAssetPtr()->SceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> DirectionalLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightBufferDescriptorList();
    std::vector<VkDescriptorBufferInfo> PointLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightBufferDescriptorList();
    std::vector<VkDescriptorBufferInfo> SpotLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightBufferDescriptorList();
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = AssetManagerPtr::GetAssetPtr()->GetMeshPropertiesDescriptorsList();
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList = AssetManagerPtr::GetAssetPtr()->GetVertexBufferDescriptorsList();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = AssetManagerPtr::GetAssetPtr()->GetIndexBufferDescriptorsList();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = AssetManagerPtr::GetAssetPtr()->GetTransformBufferDescriptorsList();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = AssetManagerPtr::GetAssetPtr()->GetMaterialBufferDescriptorList();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = AssetManagerPtr::GetAssetPtr()->GetTexture2DBufferDescriptorList();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = AssetManagerPtr::GetAssetPtr()->GetTexture3DBufferDescriptorList();
    std::vector<VkDescriptorImageInfo> CubeMapImage = AssetManagerPtr::GetAssetPtr()->GetCubeMapTextureBufferDescriptorList();

    AddAccelerationDescriptorSetBinding(0, AccelerationDescriptorStructure, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
    AddStorageTextureSetBinding(1, RayTracedTextureMaskDescriptor, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
    AddUniformBufferDescriptorSetBinding(2, SceneDataBufferInfo);
    AddStorageBufferDescriptorSetBinding(3, MeshPropertyDataBufferInfo, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount());
    AddStorageBufferDescriptorSetBinding(4, DirectionalLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetDirectionalLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(5, PointLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetPointLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(6, SpotLightBufferInfoList, LightManagerPtr::GetLightManagerPtr()->GetSpotLightDescriptorCount());
    AddStorageBufferDescriptorSetBinding(7, VertexBufferInfoList, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount());
    AddStorageBufferDescriptorSetBinding(8, IndexBufferInfoList, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount());
    AddTextureDescriptorSetBinding(9, TextureBufferInfo, AssetManagerPtr::GetAssetPtr()->GetTextureBufferDescriptorCount());
    AddTextureDescriptorSetBinding(10, Texture3DBufferInfo, AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferDescriptorCount());
    AddTextureDescriptorSetBinding(11, CubeMapImage, AssetManagerPtr::GetAssetPtr()->GetCubeMapBufferDescriptorCount());
    SubmitDescriptorSet();
}

void RayTracedPipeline::SetUpPipeline(std::shared_ptr<VulkanEngine> engine)
{
    std::vector<VkPipelineShaderStageCreateInfo> ShaderList;

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(RayTraceConstants);

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.setLayoutCount = 1;
    PipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
    PipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    PipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    vkCreatePipelineLayout(EnginePtr::GetEnginePtr()->Device, &PipelineLayoutCreateInfo, nullptr, &ShaderPipelineLayout);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
    RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(RayGeneratorShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
    MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(MissShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/shadow.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ShadowShaderInfo = {};
    ShadowShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ShadowShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ShadowShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ShadowShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ShadowShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ClosestHitShaderInfo = {};
    ClosestHitShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ClosestHitShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    ClosestHitShaderInfo.generalShader = VK_SHADER_UNUSED_KHR;
    ClosestHitShaderInfo.closestHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/anyhit.rahit.spv", VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
    ClosestHitShaderInfo.anyHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ClosestHitShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ClosestHitShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shaders/anyhit1.rahit.spv", VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ShadwoHitShaderInfo = {};
    ShadwoHitShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ShadwoHitShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    ShadwoHitShaderInfo.generalShader = VK_SHADER_UNUSED_KHR;
    ShadwoHitShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    ShadwoHitShaderInfo.anyHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ShadwoHitShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ShadwoHitShaderInfo);

    VkRayTracingPipelineCreateInfoKHR RayTracingPipeline = {};
    RayTracingPipeline.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    RayTracingPipeline.stageCount = static_cast<uint32_t>(ShaderList.size());
    RayTracingPipeline.pStages = ShaderList.data();
    RayTracingPipeline.groupCount = static_cast<uint32_t>(RayTraceShaders.size());
    RayTracingPipeline.pGroups = RayTraceShaders.data();
    RayTracingPipeline.maxPipelineRayRecursionDepth = 2;
    RayTracingPipeline.layout = ShaderPipelineLayout;
    VkResult result = EnginePtr::GetEnginePtr()->vkCreateRayTracingPipelinesKHR(VulkanPtr::GetDevice(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &RayTracingPipeline, nullptr, &ShaderPipeline);

    for (auto shader : ShaderList)
    {
        vkDestroyShaderModule(VulkanPtr::GetDevice(), shader.module, nullptr);
    }
}

void RayTracedPipeline::SetUpShaderBindingTable(std::shared_ptr<VulkanEngine> engine)
{
    const uint32_t handleSize = rayTracingPipelineProperties.shaderGroupHandleSize;
    const uint32_t handleSizeAligned = EnginePtr::GetEnginePtr()->GetAlignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);
    const uint32_t groupCount = static_cast<uint32_t>(RayTraceShaders.size());
    const uint32_t sbtSize = groupCount * handleSizeAligned;

    std::vector<uint8_t> shaderHandleStorage(sbtSize);
    EnginePtr::GetEnginePtr()->vkGetRayTracingShaderGroupHandlesKHR(VulkanPtr::GetDevice(), ShaderPipeline, 0, groupCount, sbtSize, shaderHandleStorage.data());

    raygenShaderBindingTable.CreateBuffer(handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data());
    missShaderBindingTable.CreateBuffer(handleSize * 2, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned);
    hitShaderBindingTable.CreateBuffer(handleSize * 3, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned * 3);
}
