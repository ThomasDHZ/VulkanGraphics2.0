#include "RayTraceRenderer.h"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <stb_image.h>
#include "SceneData.h"

RayTraceRenderer::RayTraceRenderer()
{

}
RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine, TextureManager& textureManagerz, std::vector<RayTraceModel>& modelList)
{
    ModelList = modelList;

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

    vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetBufferDeviceAddressKHR"));
    vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(engine.Device, "vkCmdBuildAccelerationStructuresKHR"));
    vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(engine.Device, "vkBuildAccelerationStructuresKHR"));
    vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(engine.Device, "vkCreateAccelerationStructureKHR"));
    vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(engine.Device, "vkDestroyAccelerationStructureKHR"));
    vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetAccelerationStructureBuildSizesKHR"));
    vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetAccelerationStructureDeviceAddressKHR"));
    vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(engine.Device, "vkCmdTraceRaysKHR"));
    vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetRayTracingShaderGroupHandlesKHR"));
    vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(engine.Device, "vkCreateRayTracingPipelinesKHR"));
 
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            createBottomLevelAccelerationStructure(engine, ModelList[x], ModelList[x].MeshList[y]);
        }
    }
   createTopLevelAccelerationStructure(engine);
   createStorageImage(engine, storageImage);
}
RayTraceRenderer::~RayTraceRenderer()
{
   
}

void RayTraceRenderer::Destory(VulkanEngine& engine)
{
    for (auto& Blas : bottomLevelASList)
    {
        vkFreeMemory(engine.Device, Blas.memory, nullptr);
        vkDestroyBuffer(engine.Device, Blas.buffer, nullptr);
        vkDestroyAccelerationStructureKHR(engine.Device, Blas.handle, nullptr);

        Blas.memory = VK_NULL_HANDLE;
        Blas.buffer = VK_NULL_HANDLE;
        Blas.handle = VK_NULL_HANDLE;
        Blas.deviceAddress = 0;
    }
    {
        vkFreeMemory(engine.Device, topLevelAS.memory, nullptr);
        vkDestroyBuffer(engine.Device, topLevelAS.buffer, nullptr);
        vkDestroyAccelerationStructureKHR(engine.Device, topLevelAS.handle, nullptr);

        topLevelAS.memory = VK_NULL_HANDLE;
        topLevelAS.buffer = VK_NULL_HANDLE;
        topLevelAS.handle = VK_NULL_HANDLE;
        topLevelAS.deviceAddress = 0;
    }
}

void RayTraceRenderer::createBottomLevelAccelerationStructure(VulkanEngine& engine, RayTraceModel& model, Mesh& mesh)
{
    AccelerationStructure bottomLevelAS{};

    std::vector<uint32_t> PrimitiveCountList;
    std::vector<VkAccelerationStructureGeometryKHR> AccelerationStructureGeometryList;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationBuildStructureRangeInfos;
  
    PrimitiveCountList.emplace_back(mesh.PrimitiveCount);
    AccelerationStructureGeometryList.emplace_back(mesh.AccelerationStructureGeometry);
    AccelerationBuildStructureRangeInfos.emplace_back(mesh.AccelerationStructureBuildRangeInfo);
    
 
    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo = {};
    AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
    AccelerationStructureBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();

    PrimitiveCountList.resize(AccelerationBuildStructureRangeInfos.size());
    for (auto x = 0; x < AccelerationBuildStructureRangeInfos.size(); x++)
    {
        PrimitiveCountList[x] = AccelerationBuildStructureRangeInfos[x].primitiveCount;
    }

    VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo = {};
    AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, PrimitiveCountList.data(), &AccelerationStructureBuildSizesInfo);

    createAccelerationStructure(engine, bottomLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationStructureBuildSizesInfo);

    VulkanBuffer scratchBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, AccelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    scratchBuffer.BufferDeviceAddress = engine.GetBufferDeviceAddress(scratchBuffer.Buffer);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
    AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationBuildGeometryInfo.dstAccelerationStructure = bottomLevelAS.handle;
    AccelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
    AccelerationBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();
    AccelerationBuildGeometryInfo.srcAccelerationStructure = nullptr;
    AccelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.BufferDeviceAddress;

    AcclerationCommandBuffer(engine, AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

    scratchBuffer.DestoryBuffer(engine.Device);

    VertexBufferList.emplace_back(mesh.VertexBuffer);
    IndexBufferList.emplace_back(mesh.IndexBuffer);
    bottomLevelASList.emplace_back(bottomLevelAS);
}

void RayTraceRenderer::createBottomLevelAccelerationStructure(VulkanEngine& engine, RayTraceModel& model)
{
    AccelerationStructure bottomLevelAS{};

    std::vector<uint32_t> PrimitiveCountList;
    std::vector<VkAccelerationStructureGeometryKHR> AccelerationStructureGeometryList;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationBuildStructureRangeInfos;
    for (auto x = 0; x < model.MeshList.size(); x++)
    {
        const Mesh mesh = model.MeshList[x];
        PrimitiveCountList.emplace_back(mesh.IndexCount / 3);

        VkAccelerationStructureGeometryKHR AccelerationStructureGeometry = {};
        AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        AccelerationStructureGeometry.geometry.triangles.vertexData = model.ModelVertexBufferDeviceAddress;
        AccelerationStructureGeometry.geometry.triangles.maxVertex = mesh.VertexCount;
        AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
        AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
        AccelerationStructureGeometry.geometry.triangles.indexData = model.ModelIndexBufferDeviceAddress;
        AccelerationStructureGeometryList.emplace_back(AccelerationStructureGeometry);

        VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};
        AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCountList[x];
        AccelerationStructureBuildRangeInfo.primitiveOffset = mesh.FirstIndex * sizeof(uint32_t);
        AccelerationStructureBuildRangeInfo.firstVertex = mesh.VertexOffset;
        AccelerationStructureBuildRangeInfo.transformOffset = 0;
        AccelerationBuildStructureRangeInfos.emplace_back(AccelerationStructureBuildRangeInfo);
    }

    VertexBufferList.emplace_back(model.ModelVertexBuffer);
    IndexBufferList.emplace_back(model.ModelIndexBuffer);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo = {};
    AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
    AccelerationStructureBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();

    PrimitiveCountList.resize(AccelerationBuildStructureRangeInfos.size());
    for (auto x = 0; x < AccelerationBuildStructureRangeInfos.size(); x++)
    {
        PrimitiveCountList[x] = AccelerationBuildStructureRangeInfos[x].primitiveCount;
    }

    VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo = {};
    AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, PrimitiveCountList.data(), &AccelerationStructureBuildSizesInfo);

    createAccelerationStructure(engine, bottomLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationStructureBuildSizesInfo);

    VulkanBuffer scratchBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, AccelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    scratchBuffer.BufferDeviceAddress = engine.GetBufferDeviceAddress(scratchBuffer.Buffer);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
    AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationBuildGeometryInfo.dstAccelerationStructure = bottomLevelAS.handle;
    AccelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
    AccelerationBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();
    AccelerationBuildGeometryInfo.srcAccelerationStructure = nullptr;
    AccelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;

    AcclerationCommandBuffer(AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

    deleteScratchBuffer(scratchBuffer);
    bottomLevelASList.emplace_back(bottomLevelAS);
}

void RayTraceRenderer::createTopLevelAccelerationStructure(VulkanEngine& engine)
{
    uint32_t PrimitiveCount = 1;
    std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};
    for (auto x = 0; x < bottomLevelASList.size(); x++)
    {
        glm::mat4 transformMatrix2 = ModelList[0].ModelTransform;
        VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

        VkAccelerationStructureInstanceKHR AccelerationStructureInstance{};
        AccelerationStructureInstance.transform = transformMatrix;
        AccelerationStructureInstance.instanceCustomIndex = x;
        AccelerationStructureInstance.mask = 0xFF;
        AccelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
        AccelerationStructureInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        AccelerationStructureInstance.accelerationStructureReference = bottomLevelASList[x].deviceAddress;
        AccelerationStructureInstanceList.emplace_back(AccelerationStructureInstance);
    }

    VulkanBuffer instancesBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, AccelerationStructureInstanceList.data());

    VkDeviceOrHostAddressConstKHR DeviceOrHostAddressConst = {};
    DeviceOrHostAddressConst.deviceAddress = engine.GetBufferDeviceAddress(instancesBuffer.Buffer);

    VkAccelerationStructureGeometryKHR AccelerationStructureGeometry{};
    AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
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
    vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, &PrimitiveCount, &accelerationStructureBuildSizesInfo);

    if (topLevelAS.handle == VK_NULL_HANDLE)
    {
        createAccelerationStructure(engine, topLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, accelerationStructureBuildSizesInfo);
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

    AcclerationCommandBuffer(engine, AccelerationStructureBuildGeometryInfo2, AccelerationStructureBuildRangeInfoList);

    scratchBuffer.DestoryBuffer(engine.Device);
    instancesBuffer.DestoryBuffer(engine.Device);
}

void RayTraceRenderer::createStorageImage(VulkanEngine& engine, StorageImage& StoreImage)
{
    VkImageCreateInfo image = {};
    image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = VK_FORMAT_B8G8R8A8_UNORM;
    image.extent.width = engine.SwapChain.SwapChainResolution.width;
    image.extent.height = engine.SwapChain.SwapChainResolution.height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkCreateImage(engine.Device, &image, nullptr, &StoreImage.image);

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(engine.Device, StoreImage.image, &memReqs);
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memReqs.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &StoreImage.memory);
    vkBindImageMemory(engine.Device, StoreImage.image, StoreImage.memory, 0);

    VkImageViewCreateInfo colorImageView{};
    colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = StoreImage.image;
    vkCreateImageView(engine.Device, &colorImageView, nullptr, &StoreImage.view);

    VkCommandBuffer cmdBuffer = createCommandBuffer(engine, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    setImageLayout(cmdBuffer, StoreImage.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(engine.Device, &fenceCreateInfo, nullptr, &fence);

    vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, fence);

    vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, INT64_MAX);
    vkDestroyFence(engine.Device, fence, nullptr);

    vkFreeCommandBuffers(engine.Device, engine.CommandPool, 1, &cmdBuffer);
}

void RayTraceRenderer::setImageLayout(
    VkCommandBuffer cmdbuffer,
    VkImage image,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkImageSubresourceRange subresourceRange,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask)
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;
    imageMemoryBarrier.srcAccessMask = 0;

    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);
}

void RayTraceRenderer::AcclerationCommandBuffer(VulkanEngine& engine, VkAccelerationStructureBuildGeometryInfoKHR& AccelerationStructureBuildGeometryInfo, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& AccelerationStructureBuildRangeInfo)
{
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = engine.CommandPool;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(engine.Device, &CommandBufferAllocateInfo, &cmdBuffer);

    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    auto AccelerationStructureBuildRangeInfoPtr = AccelerationStructureBuildRangeInfo.data();
    vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &AccelerationStructureBuildGeometryInfo, &AccelerationStructureBuildRangeInfoPtr);
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;

    VkFence fence;
    vkCreateFence(engine.Device, &fenceCreateInfo, nullptr, &fence);
    vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, fence);
    vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(engine.Device, fence, nullptr);
    vkFreeCommandBuffers(engine.Device, engine.CommandPool, 1, &cmdBuffer);
}

void RayTraceRenderer::UpdateAccelerationStructure(VulkanEngine& engine)
{
    createTopLevelAccelerationStructure(engine);
}

void RayTraceRenderer::createRayTracingPipeline(VulkanEngine& engine, VkDescriptorSetLayout& layout)
{
    std::vector<VkPipelineShaderStageCreateInfo> ShaderList;

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.setLayoutCount = 1;
    PipelineLayoutCreateInfo.pSetLayouts = &layout;
    vkCreatePipelineLayout(engine.Device, &PipelineLayoutCreateInfo, nullptr, &RayTracePipelineLayout);

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
    RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(RayGeneratorShaderInfo);

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
    MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(MissShaderInfo);

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/shadow.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ShadowShaderInfo = {};
    ShadowShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ShadowShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ShadowShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ShadowShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    ShadowShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ShadowShaderInfo);

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ClosestHitShaderInfo = {};
    ClosestHitShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ClosestHitShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    ClosestHitShaderInfo.generalShader = VK_SHADER_UNUSED_KHR;
    ClosestHitShaderInfo.closestHitShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ClosestHitShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    ClosestHitShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ClosestHitShaderInfo);

    VkRayTracingPipelineCreateInfoKHR RayTracingPipeline = {};
    RayTracingPipeline.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    RayTracingPipeline.stageCount = static_cast<uint32_t>(ShaderList.size());
    RayTracingPipeline.pStages = ShaderList.data();
    RayTracingPipeline.groupCount = static_cast<uint32_t>(RayTraceShaders.size());
    RayTracingPipeline.pGroups = RayTraceShaders.data();
    RayTracingPipeline.maxPipelineRayRecursionDepth = 1;
    RayTracingPipeline.layout = RayTracePipelineLayout;
    VkResult result = vkCreateRayTracingPipelinesKHR(engine.Device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &RayTracingPipeline, nullptr, &RayTracePipeline);

    for (auto shader : ShaderList)
    {
        vkDestroyShaderModule(engine.Device, shader.module, nullptr);
    }
}
void RayTraceRenderer::createShaderBindingTable(VulkanEngine& engine) {
    const uint32_t handleSize = rayTracingPipelineProperties.shaderGroupHandleSize;
    const uint32_t handleSizeAligned = alignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);
    const uint32_t groupCount = static_cast<uint32_t>(RayTraceShaders.size());
    const uint32_t sbtSize = groupCount * handleSizeAligned;

    std::vector<uint8_t> shaderHandleStorage(sbtSize);
    vkGetRayTracingShaderGroupHandlesKHR(engine.Device, RayTracePipeline, 0, groupCount, sbtSize, shaderHandleStorage.data());

    raygenShaderBindingTable.CreateBuffer(engine.Device, engine.PhysicalDevice, handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data());
    missShaderBindingTable.CreateBuffer(engine.Device, engine.PhysicalDevice, handleSize * 2, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned);
    hitShaderBindingTable.CreateBuffer(engine.Device, engine.PhysicalDevice, handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned * 3);
}

void RayTraceRenderer::buildCommandBuffers(VulkanEngine& engine, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, VkDescriptorSet& set)
{
    drawCmdBuffers.resize(swapChainFramebuffersSize);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)drawCmdBuffers.size();

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, drawCmdBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
        vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo);

        const uint32_t handleSizeAligned = alignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);

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

        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipeline);
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipelineLayout, 0, 1, &set, 0, 0);

        vkCmdTraceRaysKHR(
            drawCmdBuffers[i],
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
        barrier.image = swapChainImages[i];
        barrier.subresourceRange = subresourceRange;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(drawCmdBuffers[i], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageMemoryBarrier barrier2 = {};
        barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier2.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier2.image = storageImage.image;
        barrier2.subresourceRange = subresourceRange;
        barrier2.srcAccessMask = 0;
        barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(drawCmdBuffers[i], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

        VkImageCopy copyRegion{};
        copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        copyRegion.srcOffset = { 0, 0, 0 };
        copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        copyRegion.dstOffset = { 0, 0, 0 };
        copyRegion.extent = { engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height, 1 };
        vkCmdCopyImage(drawCmdBuffers[i], storageImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapChainImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


        VkImageMemoryBarrier barrier3 = {};
        barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier3.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier3.image = swapChainImages[i];
        barrier3.subresourceRange = subresourceRange;
        barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier3.dstAccessMask = 0;

        vkCmdPipelineBarrier(drawCmdBuffers[i], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

        VkImageMemoryBarrier barrier4 = {};
        barrier4.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier4.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier4.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier4.image = storageImage.image;
        barrier4.subresourceRange = subresourceRange;
        barrier4.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier4.dstAccessMask = 0;

        vkCmdPipelineBarrier(drawCmdBuffers[i], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier4);
        vkEndCommandBuffer(drawCmdBuffers[i]);
    }
}

void RayTraceRenderer::Resize(VulkanEngine& engine, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, uint32_t width, uint32_t height, VkDescriptorSet& set)
{
    buildCommandBuffers(engine, swapChainFramebuffersSize, swapChainImages, set);
}

void RayTraceRenderer::createAccelerationStructure(VulkanEngine& engine, AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = buildSizeInfo.accelerationStructureSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    vkCreateBuffer(engine.Device, &bufferCreateInfo, nullptr, &accelerationStructure.buffer);

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(engine.Device, accelerationStructure.buffer, &memoryRequirements);

    VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
    memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &accelerationStructure.memory);
    vkBindBufferMemory(engine.Device, accelerationStructure.buffer, accelerationStructure.memory, 0);

    VkAccelerationStructureCreateInfoKHR accelerationStructureCreate_info{};
    accelerationStructureCreate_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    accelerationStructureCreate_info.buffer = accelerationStructure.buffer;
    accelerationStructureCreate_info.size = buildSizeInfo.accelerationStructureSize;
    accelerationStructureCreate_info.type = type;
    vkCreateAccelerationStructureKHR(engine.Device, &accelerationStructureCreate_info, nullptr, &accelerationStructure.handle);
   
    VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
    accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    accelerationDeviceAddressInfo.accelerationStructure = accelerationStructure.handle;
    accelerationStructure.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &accelerationDeviceAddressInfo);
}

uint32_t RayTraceRenderer::getMemoryType(VulkanEngine& engine, uint32_t typeBits, VkMemoryPropertyFlags properties)
{
    VkBool32* memTypeFound = nullptr;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(engine.PhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                if (memTypeFound)
                {
                    *memTypeFound = true;
                }
                return i;
            }
        }
        typeBits >>= 1;
    }

    if (memTypeFound)
    {
        *memTypeFound = false;
        return 0;
    }
    else
    {
        throw std::runtime_error("Could not find a matching memory type");
    }
}

VkCommandBuffer RayTraceRenderer::createCommandBuffer(VulkanEngine& engine, VkCommandBufferLevel level, VkCommandPool pool, bool begin)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = pool;
    commandBufferAllocateInfo.level = level;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(engine.Device, &commandBufferAllocateInfo, &cmdBuffer);
    // If requested, also start recording for the new command buffer
    if (begin)
    {
        VkCommandBufferBeginInfo cmdBufferBeginInfo{};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    }
    return cmdBuffer;
}

VkCommandBuffer RayTraceRenderer::createCommandBuffer(VulkanEngine& engine, VkCommandBufferLevel level, bool begin)
{
    return createCommandBuffer(engine, level, engine.CommandPool, begin);
}

VkPipelineShaderStageCreateInfo RayTraceRenderer::loadShader(VulkanEngine& engine, std::string fileName, VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = stage;

    shaderStage.module = loadShader(engine, fileName.c_str(), engine.Device);
    shaderStage.pName = "main";
    assert(shaderStage.module != VK_NULL_HANDLE);
    shaderModules.push_back(shaderStage.module);
    return shaderStage;
}

VkShaderModule RayTraceRenderer::loadShader(VulkanEngine& engine, const char* fileName, VkDevice device)
{
    std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open())
    {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        char* shaderCode = new char[size];
        is.read(shaderCode, size);
        is.close();

        assert(size > 0);

        VkShaderModule shaderModule;
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = size;
        moduleCreateInfo.pCode = (uint32_t*)shaderCode;

        vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);

        delete[] shaderCode;

        return shaderModule;
    }
    else
    {
        std::cerr << "Error: Could not open shader file \"" << fileName << "\"" << "\n";
        return VK_NULL_HANDLE;
    }
}

uint32_t RayTraceRenderer::alignedSize(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

