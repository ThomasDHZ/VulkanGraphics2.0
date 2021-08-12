#include "RayTracedHybridPipeline.h"

RayTracedHybridPipeline::RayTracedHybridPipeline() : RayTracingGraphicsPipeline()
{
}

RayTracedHybridPipeline::RayTracedHybridPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture) : RayTracingGraphicsPipeline(engine)
{
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpDescriptorSets(engine, assetManager, topLevelAS, ShadowTextureMask, ReflectionMaskTexture, ReflectionTexture, SSAOTexture);
    SetUpPipeline(engine);
    SetUpShaderBindingTable(engine);
}

RayTracedHybridPipeline::~RayTracedHybridPipeline()
{
}

void RayTracedHybridPipeline::UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture)
{
    RayTracingGraphicsPipeline::UpdateGraphicsPipeLine();
    raygenShaderBindingTable.DestoryBuffer();
    missShaderBindingTable.DestoryBuffer();
    hitShaderBindingTable.DestoryBuffer();

    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpDescriptorSets(engine, assetManager, topLevelAS, ShadowTextureMask, ReflectionMaskTexture, ReflectionTexture, SSAOTexture);
    SetUpPipeline(engine);
    SetUpShaderBindingTable(engine);
}

void RayTracedHybridPipeline::SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, AssetManagerPtr::GetAssetPtr()->GetMaterialDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManagerPtr::GetAssetPtr()->GetTextureBufferDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferDescriptorCount()));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1));
    DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1));
    DescriptorPool = EnginePtr::GetEnginePtr()->CreateDescriptorPool(DescriptorPoolList);
}

void RayTracedHybridPipeline::SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, AssetManagerPtr::GetAssetPtr()->GetMaterialDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 11, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, AssetManagerPtr::GetAssetPtr()->GetTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 12, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 13, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 14, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 15, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 16, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    DescriptorSetLayout = EnginePtr::GetEnginePtr()->CreateDescriptorSetLayout(LayoutBindingInfo);
}

void RayTracedHybridPipeline::SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture)
{
    DescriptorSets = EnginePtr::GetEnginePtr()->CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = EnginePtr::GetEnginePtr()->AddAcclerationStructureBinding(topLevelAS.handle);
    VkDescriptorImageInfo ShadowTextureMaskDescriptor = EnginePtr::GetEnginePtr()->AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, ShadowTextureMask->View);
    VkDescriptorImageInfo ReflectionMaskTextureDescriptor = EnginePtr::GetEnginePtr()->AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, ReflectionMaskTexture->View);
    VkDescriptorImageInfo ReflectionTextureDescriptor = EnginePtr::GetEnginePtr()->AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, ReflectionTexture->View);
    VkDescriptorImageInfo SSAOTexturDescriptor = EnginePtr::GetEnginePtr()->AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, SSAOTexture->View);
    VkDescriptorBufferInfo SceneDataBufferInfo = EnginePtr::GetEnginePtr()->AddBufferDescriptor(AssetManagerPtr::GetAssetPtr()->SceneData->VulkanBufferData);

    std::vector<VkDescriptorBufferInfo> DirectionalLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetDirectionalLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> PointLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetPointLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> SpotLightBufferInfoList = AssetManagerPtr::GetAssetPtr()->lightManager->GetSpotLightBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = AssetManagerPtr::GetAssetPtr()->GetMeshPropertiesListDescriptors();
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList = AssetManagerPtr::GetAssetPtr()->GetVertexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = AssetManagerPtr::GetAssetPtr()->GetIndexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = AssetManagerPtr::GetAssetPtr()->GetTransformBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = AssetManagerPtr::GetAssetPtr()->GetMaterialBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = AssetManagerPtr::GetAssetPtr()->GetTextureBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = AssetManagerPtr::GetAssetPtr()->Get3DTextureBufferListDescriptor();
    VkDescriptorImageInfo CubeMapImage = AssetManagerPtr::GetAssetPtr()->GetSkyBoxTextureBufferListDescriptor();

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddAccelerationBuffer(0, DescriptorSets, AccelerationDescriptorStructure));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddStorageImageBuffer(1, DescriptorSets, ShadowTextureMaskDescriptor));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(2, DescriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(3, DescriptorSets, MeshPropertyDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(4, DescriptorSets, DirectionalLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(5, DescriptorSets, PointLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(6, DescriptorSets, SpotLightBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(7, DescriptorSets, VertexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(8, DescriptorSets, IndexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(9, DescriptorSets, TransformBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddBufferDescriptorSet(10, DescriptorSets, MaterialBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(11, DescriptorSets, TextureBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(12, DescriptorSets, Texture3DBufferInfo));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddTextureDescriptorSet(13, DescriptorSets, CubeMapImage));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddStorageImageBuffer(14, DescriptorSets, ReflectionMaskTextureDescriptor));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddStorageImageBuffer(14, DescriptorSets, ReflectionTextureDescriptor));
    DescriptorList.emplace_back(EnginePtr::GetEnginePtr()->AddStorageImageBuffer(15, DescriptorSets, SSAOTexturDescriptor));

    vkUpdateDescriptorSets(VulkanPtr::GetDevice(), static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void RayTracedHybridPipeline::SetUpPipeline(std::shared_ptr<VulkanEngine> engine)
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

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/hybridraygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
    RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(RayGeneratorShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/hybridmiss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
    MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(MissShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/hybridshadow.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ShadowShaderInfo = {};
    ShadowShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ShadowShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ShadowShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ShadowShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ShadowShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/hybridclosesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ClosestHitShaderInfo = {};
    ClosestHitShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ClosestHitShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    ClosestHitShaderInfo.generalShader = VK_SHADER_UNUSED_KHR;
    ClosestHitShaderInfo.closestHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/hybridanyhit.rahit.spv", VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
    ClosestHitShaderInfo.anyHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ClosestHitShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ClosestHitShaderInfo);

    ShaderList.emplace_back(EnginePtr::GetEnginePtr()->CreateShader("Shader/hybridanyhit1.rahit.spv", VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
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
    EnginePtr::GetEnginePtr()->vkCreateRayTracingPipelinesKHR(VulkanPtr::GetDevice(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &RayTracingPipeline, nullptr, &ShaderPipeline);

    for (auto shader : ShaderList)
    {
        vkDestroyShaderModule(VulkanPtr::GetDevice(), shader.module, nullptr);
    }
}

void RayTracedHybridPipeline::SetUpShaderBindingTable(std::shared_ptr<VulkanEngine> engine)
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
