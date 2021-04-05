#include "RayTraceRenderer.h"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <stb_image.h>
#include "SceneData.h"

RayTraceRenderer::RayTraceRenderer()
{

}
RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
    rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    VkPhysicalDeviceProperties2 deviceProperties2{};
    deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProperties2.pNext = &rayTracingPipelineProperties;
    vkGetPhysicalDeviceProperties2(engine.PhysicalDevice, &deviceProperties2);

    accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &accelerationStructureFeatures;
    vkGetPhysicalDeviceFeatures2(engine.PhysicalDevice, &deviceFeatures2);

    topLevelAS = AccelerationStructure(engine);
    createTopLevelAccelerationStructure(engine, assetManager);
    createStorageImage(engine);
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    SetUpDescriptorSets(engine, assetManager, sceneData);
    createRayTracingPipeline(engine);
    createShaderBindingTable(engine);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &RayTraceCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

RayTraceRenderer::~RayTraceRenderer()
{
   
}

void RayTraceRenderer::SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager)
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

void RayTraceRenderer::SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager)
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

void RayTraceRenderer::SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
      DescriptorSets = engine.CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

      VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = engine.AddAcclerationStructureBinding(topLevelAS.handle);
      VkDescriptorImageInfo RayTraceImageDescriptor = engine.AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, storageImage->View);
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
      DescriptorList.emplace_back(engine.AddAccelerationBuffer(0, DescriptorSets, AccelerationDescriptorStructure));
      DescriptorList.emplace_back(engine.AddStorageImageBuffer(1, DescriptorSets, RayTraceImageDescriptor));
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


void RayTraceRenderer::Destory(VulkanEngine& engine)
{
    {
        vkFreeMemory(engine.Device, topLevelAS.AccelerationBuffer.BufferMemory, nullptr);
        vkDestroyBuffer(engine.Device, topLevelAS.AccelerationBuffer.Buffer, nullptr);
        engine.vkDestroyAccelerationStructureKHR(engine.Device, topLevelAS.handle, nullptr);

        topLevelAS.AccelerationBuffer.BufferMemory = VK_NULL_HANDLE;
        topLevelAS.AccelerationBuffer.Buffer = VK_NULL_HANDLE;
        topLevelAS.handle = VK_NULL_HANDLE;
        topLevelAS.AccelerationBuffer.BufferDeviceAddress = 0;
    }
    {
        vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, DescriptorSetLayout, nullptr);

        DescriptorPool = VK_NULL_HANDLE;
        DescriptorSetLayout = VK_NULL_HANDLE;
    }
    {
        vkDestroyPipeline(engine.Device, RayTracePipeline, nullptr);
        vkDestroyPipelineLayout(engine.Device, RayTracePipelineLayout, nullptr);

        RayTracePipeline = VK_NULL_HANDLE;
        RayTracePipelineLayout = VK_NULL_HANDLE;
    }
    {
        storageImage->Delete(engine);
    }
    {
        raygenShaderBindingTable.DestoryBuffer(engine.Device);
        missShaderBindingTable.DestoryBuffer(engine.Device);
        hitShaderBindingTable.DestoryBuffer(engine.Device);
    }
}

void RayTraceRenderer::createTopLevelAccelerationStructure(VulkanEngine& engine, AssetManager& assetManager)
{
    uint32_t PrimitiveCount = 1;
    std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};
    for (int x = 0; x < assetManager.meshManager.MeshList.size(); x++)
    {
        if (assetManager.meshManager.MeshList[x]->ShowMesh)
        {
            glm::mat4 transformMatrix2 = glm::transpose(glm::mat4(1.0f));
            if (assetManager.meshManager.MeshList[x]->ParentModelID != 0)
            {
                const auto model = assetManager.modelManager.GetModelIndex(assetManager.meshManager.MeshList[x]->ParentModelID);
                transformMatrix2 = glm::transpose(model->ModelTransform);
            }
            VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

            VkAccelerationStructureInstanceKHR AccelerationStructureInstance{};
            AccelerationStructureInstance.transform = transformMatrix;
            AccelerationStructureInstance.instanceCustomIndex = assetManager.meshManager.MeshList[x]->MeshBufferIndex;
            AccelerationStructureInstance.mask = 0xFF;
            AccelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
            AccelerationStructureInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            AccelerationStructureInstance.accelerationStructureReference = assetManager.meshManager.MeshList[x]->BottomLevelAccelerationBuffer.AccelerationBuffer.BufferDeviceAddress;
            AccelerationStructureInstanceList.emplace_back(AccelerationStructureInstance);
        }
    }

     instancesBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, AccelerationStructureInstanceList.data());

    VkDeviceOrHostAddressConstKHR DeviceOrHostAddressConst = {};
    DeviceOrHostAddressConst.deviceAddress = engine.GetBufferDeviceAddress(instancesBuffer.Buffer);

    VkAccelerationStructureGeometryKHR AccelerationStructureGeometry{};
    AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    AccelerationStructureGeometry.flags = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;
    AccelerationStructureGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    AccelerationStructureGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
    AccelerationStructureGeometry.geometry.instances.data = DeviceOrHostAddressConst;

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo = {};
    AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo.geometryCount = 1;
    AccelerationStructureBuildGeometryInfo.pGeometries = &AccelerationStructureGeometry;

    VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
    accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    engine.vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, &PrimitiveCount, &accelerationStructureBuildSizesInfo);

    if (topLevelAS.handle == VK_NULL_HANDLE)
    {
        topLevelAS.CreateAccelerationStructure(engine, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, accelerationStructureBuildSizesInfo);
    }

    VulkanBuffer scratchBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, accelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    scratchBuffer.BufferDeviceAddress = engine.GetBufferDeviceAddress(scratchBuffer.Buffer);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo2 = {};
    AccelerationStructureBuildGeometryInfo2.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo2.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo2.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo2.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationStructureBuildGeometryInfo2.geometryCount = 1;
    AccelerationStructureBuildGeometryInfo2.pGeometries = &AccelerationStructureGeometry;
    AccelerationStructureBuildGeometryInfo2.scratchData.deviceAddress = scratchBuffer.BufferDeviceAddress;

    if (topLevelAS.handle == VK_NULL_HANDLE)
    {
        AccelerationStructureBuildGeometryInfo2.dstAccelerationStructure = topLevelAS.handle;
    }
    else
    {
        AccelerationStructureBuildGeometryInfo2.srcAccelerationStructure = topLevelAS.handle;
        AccelerationStructureBuildGeometryInfo2.dstAccelerationStructure = topLevelAS.handle;
    }

    VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo = {};
    AccelerationStructureBuildRangeInfo.primitiveCount = static_cast<uint32_t>(AccelerationStructureInstanceList.size());
    AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
    AccelerationStructureBuildRangeInfo.firstVertex = 0;
    AccelerationStructureBuildRangeInfo.transformOffset = 0;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationStructureBuildRangeInfoList = { AccelerationStructureBuildRangeInfo };

    topLevelAS.AcclerationCommandBuffer(engine, AccelerationStructureBuildGeometryInfo2, AccelerationStructureBuildRangeInfoList);

    scratchBuffer.DestoryBuffer(engine.Device);
    instancesBuffer.DestoryBuffer(engine.Device);
}

void RayTraceRenderer::createStorageImage(VulkanEngine& engine)
{
    if(storageImage.get() != nullptr)
    { 
        storageImage->Delete(engine);
    }
    storageImage = std::make_shared<RenderedRayTracedColorTexture>(RenderedRayTracedColorTexture(engine));
}

void RayTraceRenderer::createRayTracingPipeline(VulkanEngine& engine)
{
    std::vector<VkPipelineShaderStageCreateInfo> ShaderList;

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.setLayoutCount = 1;
    PipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
    vkCreatePipelineLayout(engine.Device, &PipelineLayoutCreateInfo, nullptr, &RayTracePipelineLayout);

    ShaderList.emplace_back(engine.CreateShader("Shader/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
    RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(RayGeneratorShaderInfo);

    ShaderList.emplace_back(engine.CreateShader("Shader/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
    MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(MissShaderInfo);

    ShaderList.emplace_back(engine.CreateShader("Shader/shadow.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ShadowShaderInfo = {};
    ShadowShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ShadowShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ShadowShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ShadowShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ShadowShaderInfo);

    ShaderList.emplace_back(engine.CreateShader("Shader/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ClosestHitShaderInfo = {};
    ClosestHitShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ClosestHitShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    ClosestHitShaderInfo.generalShader = VK_SHADER_UNUSED_KHR;
    ClosestHitShaderInfo.closestHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;

    ShaderList.emplace_back(engine.CreateShader("Shader/anyhit.rahit.spv", VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
    ClosestHitShaderInfo.anyHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ClosestHitShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ClosestHitShaderInfo);

    ShaderList.emplace_back(engine.CreateShader("Shader/anyhit1.rahit.spv", VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
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
    RayTracingPipeline.maxPipelineRayRecursionDepth = 1;
    RayTracingPipeline.layout = RayTracePipelineLayout;
    VkResult result = engine.vkCreateRayTracingPipelinesKHR(engine.Device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &RayTracingPipeline, nullptr, &RayTracePipeline);

    for (auto shader : ShaderList)
    {
        vkDestroyShaderModule(engine.Device, shader.module, nullptr);
    }
}

void RayTraceRenderer::createShaderBindingTable(VulkanEngine& engine) {
    const uint32_t handleSize = rayTracingPipelineProperties.shaderGroupHandleSize;
    const uint32_t handleSizeAligned = engine.GetAlignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);
    const uint32_t groupCount = static_cast<uint32_t>(RayTraceShaders.size());
    const uint32_t sbtSize = groupCount * handleSizeAligned;

    std::vector<uint8_t> shaderHandleStorage(sbtSize);
    engine.vkGetRayTracingShaderGroupHandlesKHR(engine.Device, RayTracePipeline, 0, groupCount, sbtSize, shaderHandleStorage.data());

    raygenShaderBindingTable.CreateBuffer(engine.Device, engine.PhysicalDevice, handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data());
    missShaderBindingTable.CreateBuffer(engine.Device, engine.PhysicalDevice, handleSize * 2, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned);
    hitShaderBindingTable.CreateBuffer(engine.Device, engine.PhysicalDevice, handleSize * 3, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned * 3);
}

void RayTraceRenderer::buildCommandBuffers(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };


    vkBeginCommandBuffer(RayTraceCommandBuffer, &cmdBufInfo);

    const uint32_t handleSizeAligned = engine.GetAlignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);

    VkStridedDeviceAddressRegionKHR raygenShaderSbtEntry{};
    raygenShaderSbtEntry.deviceAddress = engine.GetBufferDeviceAddress(raygenShaderBindingTable.Buffer);
    raygenShaderSbtEntry.stride = handleSizeAligned;
    raygenShaderSbtEntry.size = handleSizeAligned;

    VkStridedDeviceAddressRegionKHR missShaderSbtEntry{};
    missShaderSbtEntry.deviceAddress = engine.GetBufferDeviceAddress(missShaderBindingTable.Buffer);
    missShaderSbtEntry.stride = handleSizeAligned;
    missShaderSbtEntry.size = handleSizeAligned;

    VkStridedDeviceAddressRegionKHR hitShaderSbtEntry{};
    hitShaderSbtEntry.deviceAddress = engine.GetBufferDeviceAddress(hitShaderBindingTable.Buffer);
    hitShaderSbtEntry.stride = handleSizeAligned;
    hitShaderSbtEntry.size = handleSizeAligned;

    VkStridedDeviceAddressRegionKHR callableShaderSbtEntry{};

    vkCmdBindPipeline(RayTraceCommandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipeline);
    vkCmdBindDescriptorSets(RayTraceCommandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipelineLayout, 0, 1, &DescriptorSets, 0, 0);

    engine.vkCmdTraceRaysKHR(
        RayTraceCommandBuffer,
        &raygenShaderSbtEntry,
        &missShaderSbtEntry,
        &hitShaderSbtEntry,
        &callableShaderSbtEntry,
        engine.SwapChain.SwapChainResolution.width,
        engine.SwapChain.SwapChainResolution.height,
        1);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.image = engine.SwapChain.GetSwapChainImages()[imageIndex];
    barrier.subresourceRange = subresourceRange;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(RayTraceCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    VkImageMemoryBarrier barrier2 = {};
    barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier2.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier2.image = storageImage->Image;
    barrier2.subresourceRange = subresourceRange;
    barrier2.srcAccessMask = 0;
    barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(RayTraceCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

    VkImageCopy copyRegion{};
    copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
    copyRegion.srcOffset = { 0, 0, 0 };
    copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
    copyRegion.dstOffset = { 0, 0, 0 };
    copyRegion.extent = { engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height, 1 };
    vkCmdCopyImage(RayTraceCommandBuffer, storageImage->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, engine.SwapChain.GetSwapChainImages()[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


    VkImageMemoryBarrier barrier3 = {};
    barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier3.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier3.image = engine.SwapChain.GetSwapChainImages()[imageIndex];
    barrier3.subresourceRange = subresourceRange;
    barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier3.dstAccessMask = 0;

    vkCmdPipelineBarrier(RayTraceCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

    VkImageMemoryBarrier barrier4 = {};
    barrier4.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier4.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier4.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier4.image = storageImage->Image;
    barrier4.subresourceRange = subresourceRange;
    barrier4.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier4.dstAccessMask = 0;

    vkCmdPipelineBarrier(RayTraceCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier4);
    vkEndCommandBuffer(RayTraceCommandBuffer);
}

void RayTraceRenderer::Resize(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, uint32_t imageIndex)
{
    vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, DescriptorSetLayout, nullptr);

    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;

    vkDestroyPipeline(engine.Device, RayTracePipeline, nullptr);
    vkDestroyPipelineLayout(engine.Device, RayTracePipelineLayout, nullptr);

    RayTracePipeline = VK_NULL_HANDLE;
    RayTracePipelineLayout = VK_NULL_HANDLE;

    createStorageImage(engine);
    SetUpDescriptorPool(engine, assetManager);
    SetUpDescriptorLayout(engine, assetManager);
    createRayTracingPipeline(engine);
    SetUpDescriptorSets(engine, assetManager, sceneData);
    buildCommandBuffers(engine, assetManager, imageIndex);
}
