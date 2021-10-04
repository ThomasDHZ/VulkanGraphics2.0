#include "PBRRayTraceRenderPass.h"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <stb_image.h>
#include "SceneData.h"
#include "ImGui/imgui.h"

PBRRayTraceRenderPass::PBRRayTraceRenderPass()
{

}
PBRRayTraceRenderPass::PBRRayTraceRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{
    RayTracedTexture = std::make_shared<RenderedRayTracedColorTexture>(RenderedRayTracedColorTexture(engine));
    ShadowTextureMask = std::make_shared<RenderedRayTracedColorTexture>(RenderedRayTracedColorTexture(engine));
    ReflectionTextureMask = std::make_shared<RenderedRayTracedColorTexture>(RenderedRayTracedColorTexture(engine));
    ReflectionTexture = std::make_shared<RenderedRayTracedColorTexture>(RenderedRayTracedColorTexture(engine));
    SSAOTexture = std::make_shared<RenderedRayTracedColorTexture>(RenderedRayTracedColorTexture(engine));

    topLevelAS = AccelerationStructure(engine);
    SetUpTopLevelAccelerationStructure(engine, assetManager);
    PBRRayTracePipeline = std::make_shared<PBRRayTracedPipeline>(PBRRayTracedPipeline(engine, assetManager, topLevelAS, RayTracedTexture));

    SetUpCommandBuffers(engine);
}

PBRRayTraceRenderPass::~PBRRayTraceRenderPass()
{

}

void PBRRayTraceRenderPass::SetUpTopLevelAccelerationStructure(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{
    uint32_t PrimitiveCount = 1;
    std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};
    for (int x = 0; x < assetManager->meshManager->MeshList.size(); x++)
    {
        if (assetManager->meshManager->MeshList[x]->ShowMesh)
        {
            glm::mat4 transformMatrix2 = glm::transpose(glm::mat4(1.0f));
            if (assetManager->meshManager->MeshList[x]->ParentModelID != 0)
            {
                const auto model = assetManager->ObjManager->GetModel(assetManager->meshManager->MeshList[x]->ParentModelID);
                transformMatrix2 = glm::transpose(model->ModelTransform);
            }
            VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

            VkAccelerationStructureInstanceKHR AccelerationStructureInstance{};
            AccelerationStructureInstance.transform = transformMatrix;
            AccelerationStructureInstance.instanceCustomIndex = assetManager->meshManager->MeshList[x]->MeshBufferIndex;
            AccelerationStructureInstance.mask = 0xFF;
            AccelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
            AccelerationStructureInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            AccelerationStructureInstance.accelerationStructureReference = assetManager->meshManager->MeshList[x]->BottomLevelAccelerationBuffer.AccelerationBuffer.BufferDeviceAddress;
            AccelerationStructureInstanceList.emplace_back(AccelerationStructureInstance);
        }
    }

    VkDeviceOrHostAddressConstKHR DeviceOrHostAddressConst = {};
    if (AccelerationStructureInstanceList.size() > 0)
    {
        instancesBuffer = VulkanBuffer(sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, AccelerationStructureInstanceList.data());

        DeviceOrHostAddressConst.deviceAddress = engine->GetBufferDeviceAddress(instancesBuffer.Buffer);
    }

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
    engine->vkGetAccelerationStructureBuildSizesKHR(engine->Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, &PrimitiveCount, &accelerationStructureBuildSizesInfo);

    if (topLevelAS.handle == VK_NULL_HANDLE)
    {
        topLevelAS.CreateAccelerationStructure(engine, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, accelerationStructureBuildSizesInfo);
    }

    VulkanBuffer scratchBuffer = VulkanBuffer(accelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    scratchBuffer.BufferDeviceAddress = engine->GetBufferDeviceAddress(scratchBuffer.Buffer);

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

    scratchBuffer.DestoryBuffer();
    instancesBuffer.DestoryBuffer();
}

void PBRRayTraceRenderPass::GUIUpdate()
{
    ImGui::SliderInt("Apply Anti-Aliasing:", &RTXConst.ApplyAntiAliasing, 0, 1);
    ImGui::SliderFloat("SphereSample:", &RTXConst.SampleRate, 0.0f, 3.0f);
    ImGui::SliderFloat("Roughness:", &RTXConst.TestRoughness, 0.0f, 3.0f);
    ImGui::SliderInt("Max Anti-Aliasing Coun", &RTXConst.AntiAliasingCount, 0, 15);
    ImGui::SliderInt("Max Refeflect Count", &RTXConst.MaxRefeflectCount, 0, 1024);
}

void PBRRayTraceRenderPass::SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine->CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(engine->Device, &allocInfo, &RayTraceCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void PBRRayTraceRenderPass::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, RendererID renderID, std::shared_ptr<Camera> ViewCamera)
{
    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    vkBeginCommandBuffer(RayTraceCommandBuffer, &cmdBufInfo);
    const uint32_t handleSizeAligned = engine->GetAlignedSize(PBRRayTracePipeline->rayTracingPipelineProperties.shaderGroupHandleSize, PBRRayTracePipeline->rayTracingPipelineProperties.shaderGroupHandleAlignment);

    VkStridedDeviceAddressRegionKHR raygenShaderSbtEntry{};
    raygenShaderSbtEntry.deviceAddress = engine->GetBufferDeviceAddress(PBRRayTracePipeline->raygenShaderBindingTable.Buffer);
    raygenShaderSbtEntry.stride = handleSizeAligned;
    raygenShaderSbtEntry.size = handleSizeAligned;

    VkStridedDeviceAddressRegionKHR missShaderSbtEntry{};
    missShaderSbtEntry.deviceAddress = engine->GetBufferDeviceAddress(PBRRayTracePipeline->missShaderBindingTable.Buffer);
    missShaderSbtEntry.stride = handleSizeAligned;
    missShaderSbtEntry.size = handleSizeAligned;

    VkStridedDeviceAddressRegionKHR hitShaderSbtEntry{};
    hitShaderSbtEntry.deviceAddress = engine->GetBufferDeviceAddress(PBRRayTracePipeline->hitShaderBindingTable.Buffer);
    hitShaderSbtEntry.stride = handleSizeAligned;
    hitShaderSbtEntry.size = handleSizeAligned;

    VkStridedDeviceAddressRegionKHR callableShaderSbtEntry{};

    RTXConst.proj = glm::inverse(ViewCamera->GetProjectionMatrix());
    RTXConst.proj[1][1] *= -1;
    RTXConst.view = glm::inverse(ViewCamera->GetViewMatrix());
    RTXConst.CameraPos = ViewCamera->GetPosition();
    RTXConst.frame = Frame++;

    if (RTXConst.proj != LastCameraProjection)
    {
        Frame = 0;
    }
    if (RTXConst.view != LastCameraView)
    {
        Frame = 0;
    }

    LastCameraProjection = RTXConst.proj;
    LastCameraView = RTXConst.view;

    vkCmdPushConstants(RayTraceCommandBuffer, PBRRayTracePipeline->ShaderPipelineLayout, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 0, sizeof(RayTraceConstants), &RTXConst);
    vkCmdBindPipeline(RayTraceCommandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, PBRRayTracePipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(RayTraceCommandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, PBRRayTracePipeline->ShaderPipelineLayout, 0, 1, &PBRRayTracePipeline->DescriptorSet, 0, 0);

    RayTracedTexture->UpdateImageLayout(RayTraceCommandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
    engine->vkCmdTraceRaysKHR(RayTraceCommandBuffer, &raygenShaderSbtEntry, &missShaderSbtEntry, &hitShaderSbtEntry, &callableShaderSbtEntry, engine->SwapChain.SwapChainResolution.width, engine->SwapChain.SwapChainResolution.height, 1);
    RayTracedTexture->UpdateImageLayout(RayTraceCommandBuffer, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkEndCommandBuffer(RayTraceCommandBuffer);
}

void PBRRayTraceRenderPass::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager)
{
    RayTracedTexture->RecreateRendererTexture();
    ShadowTextureMask->RecreateRendererTexture();
    ReflectionTextureMask->RecreateRendererTexture();
    ReflectionTexture->RecreateRendererTexture();
    SSAOTexture->RecreateRendererTexture();

    PBRRayTracePipeline->UpdateGraphicsPipeLine(engine, assetManager, topLevelAS, RayTracedTexture);
}

void PBRRayTraceRenderPass::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    {
        vkFreeMemory(engine->Device, topLevelAS.AccelerationBuffer.BufferMemory, nullptr);
        vkDestroyBuffer(engine->Device, topLevelAS.AccelerationBuffer.Buffer, nullptr);
        engine->vkDestroyAccelerationStructureKHR(engine->Device, topLevelAS.handle, nullptr);

        topLevelAS.AccelerationBuffer.BufferMemory = VK_NULL_HANDLE;
        topLevelAS.AccelerationBuffer.Buffer = VK_NULL_HANDLE;
        topLevelAS.handle = VK_NULL_HANDLE;
        topLevelAS.AccelerationBuffer.BufferDeviceAddress = 0;
    }
    {
        RayTracedTexture->Delete();
        ShadowTextureMask->Delete();
        ReflectionTextureMask->Delete();
        ReflectionTexture->Delete();
        SSAOTexture->Delete();
    }
    PBRRayTracePipeline->Destroy();
}
