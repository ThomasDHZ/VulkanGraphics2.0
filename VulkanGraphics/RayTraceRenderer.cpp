#include "RayTraceRenderer.h"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include "Vertex.h"
RayTraceRenderer::RayTraceRenderer()
{

}
RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine)
{
    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(3);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 3;

    if (vkCreateDescriptorPool(engine.Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

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

    camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.GetSwapChainResolution().width, (float)engine.GetSwapChainResolution().height), glm::vec3(0.0f, 0.0f, 5.0f));

    createBottomLevelAccelerationStructure(engine);
    createTopLevelAccelerationStructure(engine);
   
    rayTexture = RayTraceTexture(engine);
    //createStorageImage(engine);
    createUniformBuffer(engine);
    createRayTracingPipeline(engine);
    createShaderBindingTable(engine);
    createDescriptorSets(engine);
    buildCommandBuffers(engine);
}
RayTraceRenderer::~RayTraceRenderer()
{

}

void RayTraceRenderer::createBottomLevelAccelerationStructure(VulkanEngine& engine)
{
 
    std::shared_ptr<TextureManager> manager = std::make_shared<TextureManager>(engine);
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    model = Model(engine, manager, "C:/Users/dotha/source/repos/VulkanGraphics/Models/suzanne.obj", RayTraceDescriptorSetLayout, 1, texture);
    uint32_t numTriangles = static_cast<uint32_t>(model.SubMeshList[0].IndexList.size()) / 3;

    VkTransformMatrixKHR transformMatrix = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };

    vertexBuffer.CreateBuffer(engine, model.SubMeshList[0].VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, model.SubMeshList[0].VertexList.data());
    indexBuffer.CreateBuffer(engine, model.SubMeshList[0].IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, model.SubMeshList[0].IndexList.data());
    transformBuffer.CreateBuffer(engine, sizeof(VkTransformMatrixKHR), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &transformMatrix);

    VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress = engine.BufferToDeviceAddress(vertexBuffer.Buffer);
    VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress = engine.BufferToDeviceAddress(indexBuffer.Buffer);
    VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress = engine.BufferToDeviceAddress(transformBuffer.Buffer);

    VkAccelerationStructureGeometryKHR GeometryAccelerationStructure = {};
    GeometryAccelerationStructure.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    GeometryAccelerationStructure.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
    GeometryAccelerationStructure.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    GeometryAccelerationStructure.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    GeometryAccelerationStructure.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    GeometryAccelerationStructure.geometry.triangles.vertexData = vertexBufferDeviceAddress;
    GeometryAccelerationStructure.geometry.triangles.maxVertex = model.SubMeshList[0].VertexList.size() * sizeof(Vertex);
    GeometryAccelerationStructure.geometry.triangles.vertexStride = sizeof(Vertex);
    GeometryAccelerationStructure.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
    GeometryAccelerationStructure.geometry.triangles.indexData = indexBufferDeviceAddress;
    GeometryAccelerationStructure.geometry.triangles.transformData.deviceAddress = 0;
    GeometryAccelerationStructure.geometry.triangles.transformData.hostAddress = nullptr;
    GeometryAccelerationStructure.geometry.triangles.transformData = transformBufferDeviceAddress;

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometry = {};
    AccelerationBuildGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometry.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationBuildGeometry.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometry.geometryCount = 1;
    AccelerationBuildGeometry.pGeometries = &GeometryAccelerationStructure;

    uint32_t geoCount = numTriangles;
    VkAccelerationStructureBuildSizesInfoKHR AccelerationBuildInfo = {};
    AccelerationBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationBuildGeometry, &geoCount, &AccelerationBuildInfo);

    bottomLevelAS.CreateBuffer(engine, AccelerationBuildInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkAccelerationStructureCreateInfoKHR AccelerationStructureInfo = {};
    AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    AccelerationStructureInfo.buffer = bottomLevelAS.Buffer;
    AccelerationStructureInfo.size = AccelerationBuildInfo.accelerationStructureSize;
    AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    VkResult result = vkCreateAccelerationStructureKHR(engine.Device, &AccelerationStructureInfo, nullptr, &bottomLevelAS.BufferHandle);

    VulkanBuffer ScratchBuffer = VulkanBuffer();
    ScratchBuffer.CreateBuffer(engine, AccelerationBuildInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkDeviceOrHostAddressConstKHR ScratchBufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.GetBuffer());

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
    AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationBuildGeometryInfo.dstAccelerationStructure = bottomLevelAS.BufferHandle;
    AccelerationBuildGeometryInfo.geometryCount = 1;
    AccelerationBuildGeometryInfo.pGeometries = &GeometryAccelerationStructure;
    AccelerationBuildGeometryInfo.scratchData.deviceAddress = ScratchBufferDeviceAddress.deviceAddress;

    std::vector<VkAccelerationStructureBuildRangeInfoKHR*> AcclerationBuildRangeList;
    VkAccelerationStructureBuildRangeInfoKHR AcclerationBuildRange = {};
    AcclerationBuildRange.primitiveCount = geoCount;
    AcclerationBuildRange.primitiveOffset = 0;
    AcclerationBuildRange.firstVertex = 0;
    AcclerationBuildRange.transformOffset = 0;
    AcclerationBuildRangeList.emplace_back(&AcclerationBuildRange);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = engine.GetRenderCommandPool();
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(engine.Device, &commandBufferAllocateInfo, &cmdBuffer);

    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &AccelerationBuildGeometryInfo, AcclerationBuildRangeList.data());
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
    vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, 100000000000);
    vkDestroyFence(engine.Device, fence, nullptr);
    vkFreeCommandBuffers(engine.Device, engine.GetRenderCommandPool(), 1, &cmdBuffer);


    VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo{};
    AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    AccelerationDeviceAddressInfo.accelerationStructure = bottomLevelAS.BufferHandle;
    bottomLevelAS.BufferDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &AccelerationDeviceAddressInfo);

    ScratchBuffer.DestoryBuffer(engine);
}
void RayTraceRenderer::createTopLevelAccelerationStructure(VulkanEngine& engine)
{
    VkTransformMatrixKHR transformMatrix = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f
    };

    VkAccelerationStructureInstanceKHR AccelerationInstance = {};
    AccelerationInstance.transform = transformMatrix;
    AccelerationInstance.instanceCustomIndex = 0;
    AccelerationInstance.mask = 0xFF;
    AccelerationInstance.instanceShaderBindingTableRecordOffset = 0;
    AccelerationInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    AccelerationInstance.accelerationStructureReference = bottomLevelAS.BufferDeviceAddress;

    VulkanBuffer instancesBuffer;
    instancesBuffer.CreateBuffer(engine, sizeof(VkAccelerationStructureInstanceKHR), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &AccelerationInstance);

    VkDeviceOrHostAddressConstKHR TopLevelAccelerationInstanceBufferDeviceAddress{};
    TopLevelAccelerationInstanceBufferDeviceAddress.deviceAddress = engine.BufferToDeviceAddress(instancesBuffer.Buffer).deviceAddress;

    VkAccelerationStructureGeometryKHR AccelerationGeometry = {};
    AccelerationGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    AccelerationGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    AccelerationGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
    AccelerationGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    AccelerationGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
    AccelerationGeometry.geometry.instances.data = TopLevelAccelerationInstanceBufferDeviceAddress;

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometry = {};
    AccelerationStructureBuildGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometry.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationStructureBuildGeometry.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationStructureBuildGeometry.geometryCount = 1;
    AccelerationStructureBuildGeometry.pGeometries = &AccelerationGeometry;

    uint32_t geoCount =9;
    VkAccelerationStructureBuildSizesInfoKHR AccelerationBuildInfo = {};
    AccelerationBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometry, &geoCount, &AccelerationBuildInfo);

    topLevelAS.CreateBuffer(engine, AccelerationBuildInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkAccelerationStructureCreateInfoKHR AccelerationStructureInfo = {};
    AccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    AccelerationStructureInfo.buffer = topLevelAS.Buffer;
    AccelerationStructureInfo.size = AccelerationBuildInfo.accelerationStructureSize;
    AccelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    VkResult result = vkCreateAccelerationStructureKHR(engine.Device, &AccelerationStructureInfo, nullptr, &topLevelAS.BufferHandle);

    VulkanBuffer ScratchBuffer = VulkanBuffer();
    ScratchBuffer.CreateBuffer(engine, AccelerationBuildInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkDeviceOrHostAddressConstKHR ScratchBufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.GetBuffer());

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
    AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationBuildGeometryInfo.dstAccelerationStructure = topLevelAS.BufferHandle;
    AccelerationBuildGeometryInfo.geometryCount = 1;
    AccelerationBuildGeometryInfo.pGeometries = &AccelerationGeometry;
    AccelerationBuildGeometryInfo.scratchData.deviceAddress = ScratchBufferDeviceAddress.deviceAddress;

    std::vector<VkAccelerationStructureBuildRangeInfoKHR*> AcclerationBuildRangeList;
    VkAccelerationStructureBuildRangeInfoKHR AcclerationBuildRange = {};
    AcclerationBuildRange.primitiveCount = geoCount;
    AcclerationBuildRange.primitiveOffset = 0;
    AcclerationBuildRange.firstVertex = 0;
    AcclerationBuildRange.transformOffset = 0;
    AcclerationBuildRangeList.emplace_back(&AcclerationBuildRange);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = engine.GetRenderCommandPool();
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(engine.Device, &commandBufferAllocateInfo, &cmdBuffer);

    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &AccelerationBuildGeometryInfo, AcclerationBuildRangeList.data());
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
    vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, 100000000000);
    vkDestroyFence(engine.Device, fence, nullptr);
    vkFreeCommandBuffers(engine.Device, engine.GetRenderCommandPool(), 1, &cmdBuffer);


    VkAccelerationStructureDeviceAddressInfoKHR AccelerationDeviceAddressInfo{};
    AccelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    AccelerationDeviceAddressInfo.accelerationStructure = topLevelAS.BufferHandle;
    topLevelAS.BufferDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &AccelerationDeviceAddressInfo);

    ScratchBuffer.DestoryBuffer(engine);
}
void RayTraceRenderer::createStorageImage(VulkanEngine& engine)
{

    //VkImageCreateInfo image = {};
    //image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    //image.imageType = VK_IMAGE_TYPE_2D;
    //image.format = VK_FORMAT_B8G8R8A8_UNORM;
    //image.extent.width = engine.GetSwapChainResolution().width;
    //image.extent.height = engine.GetSwapChainResolution().height;
    //image.extent.depth = 1;
    //image.mipLevels = 1;
    //image.arrayLayers = 1;
    //image.samples = VK_SAMPLE_COUNT_1_BIT;
    //image.tiling = VK_IMAGE_TILING_OPTIMAL;
    //image.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    //image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //vkCreateImage(engine.Device, &image, nullptr, &storageImage.image);

    //VkMemoryRequirements memReqs;
    //vkGetImageMemoryRequirements(engine.Device, storageImage.image, &memReqs);
    //VkMemoryAllocateInfo memoryAllocateInfo{};
    //memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //memoryAllocateInfo.allocationSize = memReqs.size;
    //memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &storageImage.memory);
    //vkBindImageMemory(engine.Device, storageImage.image, storageImage.memory, 0);

    //VkImageViewCreateInfo colorImageView{};
    //colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    //colorImageView.format = VK_FORMAT_B8G8R8A8_UNORM;
    //colorImageView.subresourceRange = {};
    //colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //colorImageView.subresourceRange.baseMipLevel = 0;
    //colorImageView.subresourceRange.levelCount = 1;
    //colorImageView.subresourceRange.baseArrayLayer = 0;
    //colorImageView.subresourceRange.layerCount = 1;
    //colorImageView.image = storageImage.image;
    //vkCreateImageView(engine.Device, &colorImageView, nullptr, &storageImage.view);

    //VkCommandBuffer cmdBuffer = createCommandBuffer(engine, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    //setImageLayout(engine, cmdBuffer, storageImage.image,
    //    VK_IMAGE_LAYOUT_UNDEFINED,
    //    VK_IMAGE_LAYOUT_GENERAL,
    //    { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
    //flushCommandBuffer(engine, cmdBuffer);
}

void RayTraceRenderer::Destory(VulkanEngine& engine)
{
    vkDestroyPipeline(engine.Device, RayTracePipeline, nullptr);
    vkDestroyPipelineLayout(engine.Device, RayTracePipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, RayTraceDescriptorSetLayout, nullptr);
    rayTexture.Delete(engine);
   // vkDestroyImageView(engine.Device, storageImage.view, nullptr);
  //  vkDestroyImage(engine.Device, storageImage.image, nullptr);
  //  vkFreeMemory(engine.Device, storageImage.memory, nullptr);
   // vkFreeMemory(engine.Device, bottomLevelAS.memory, nullptr);
 //   vkDestroyBuffer(engine.Device, bottomLevelAS.buffer, nullptr);
//    vkDestroyAccelerationStructureKHR(engine.Device, bottomLevelAS.handle, nullptr);
    //vkFreeMemory(engine.Device, topLevelAS.memory, nullptr);
    //vkDestroyBuffer(engine.Device, topLevelAS.buffer, nullptr);
    //vkDestroyAccelerationStructureKHR(engine.Device, topLevelAS.handle, nullptr);
    vertexBuffer.DestoryBuffer(engine);
    indexBuffer.DestoryBuffer(engine);
    transformBuffer.DestoryBuffer(engine);
    raygenShaderBindingTable.DestoryBuffer(engine);
    missShaderBindingTable.DestoryBuffer(engine);
    hitShaderBindingTable.DestoryBuffer(engine);
    ubo.DestoryBuffer(engine);
    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
}

void RayTraceRenderer::createUniformBuffer(VulkanEngine& engine)
{
    ubo.CreateBuffer(engine, sizeof(UniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformData);
}
void RayTraceRenderer::updateUniformBuffers(VulkanEngine& engine, GLFWwindow* window)
{
    keyboard.Update(window, camera);
    mouse.Update(window, camera);
    camera->Update(engine);


    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    uniformData.projInverse = glm::inverse(camera->GetProjectionMatrix());
    uniformData.viewInverse = glm::inverse(camera->GetViewMatrix());
    uniformData.lightPos = glm::vec4(cos(glm::radians(time * 360.0f)) * 40.0f, -50.0f + sin(glm::radians(time * 360.0f)) * 20.0f, 25.0f + sin(glm::radians(time * 360.0f)) * 5.0f, 0.0f);
    uniformData.vertexSize = sizeof(Vertex);
    ubo.CopyBufferToMemory(engine, &uniformData, sizeof(UniformData));
}
void RayTraceRenderer::createRayTracingPipeline(VulkanEngine& engine)
{
    std::vector<VkDescriptorSetLayoutBinding> RTDescriptorSetBindings;
    VkDescriptorSetLayoutBinding AccelerationStructureBinding = {};
    AccelerationStructureBinding.binding = 0;
    AccelerationStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    AccelerationStructureBinding.descriptorCount = 1;
    AccelerationStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(AccelerationStructureBinding);

    VkDescriptorSetLayoutBinding ReturnImageStructureBinding = {};
    ReturnImageStructureBinding.binding = 1;
    ReturnImageStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    ReturnImageStructureBinding.descriptorCount = 1;
    ReturnImageStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(ReturnImageStructureBinding);

    VkDescriptorSetLayoutBinding UniformBufferStructureBinding = {};
    UniformBufferStructureBinding.binding = 2;
    UniformBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    UniformBufferStructureBinding.descriptorCount = 1;
    UniformBufferStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(UniformBufferStructureBinding);

    VkDescriptorSetLayoutBinding VertexBufferStructureBinding = {};
    VertexBufferStructureBinding.binding = 3;
    VertexBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VertexBufferStructureBinding.descriptorCount = 1;
    VertexBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(VertexBufferStructureBinding);

    VkDescriptorSetLayoutBinding IndexBufferStructureBinding = {};
    IndexBufferStructureBinding.binding = 4;
    IndexBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    IndexBufferStructureBinding.descriptorCount = 1;
    IndexBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(IndexBufferStructureBinding);

    VkDescriptorSetLayoutCreateInfo RTDescriptorSetLayout = {};
    RTDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    RTDescriptorSetLayout.bindingCount = static_cast<uint32_t>(RTDescriptorSetBindings.size());
    RTDescriptorSetLayout.pBindings = RTDescriptorSetBindings.data();
    vkCreateDescriptorSetLayout(engine.Device, &RTDescriptorSetLayout, nullptr, &RayTraceDescriptorSetLayout);

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.setLayoutCount = 1;
    PipelineLayoutCreateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
    vkCreatePipelineLayout(engine.Device, &PipelineLayoutCreateInfo, nullptr, &RayTracePipelineLayout);

    std::vector<VkPipelineShaderStageCreateInfo> ShaderList;

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
    RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(RayGeneratorShaderInfo);

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
    MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(MissShaderInfo);

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
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
    const VkPhysicalDeviceRayTracingPipelinePropertiesKHR PhysicalDeviceRayTracingPipelineProperties = engine.GetRayTracingPipelineProperties(engine.PhysicalDevice);
    const uint32_t HandleSize = PhysicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize;
    const uint32_t AlignedHandleSize = engine.GetShaderGroupAlignment(engine.PhysicalDevice);
    const uint32_t GroupCount = static_cast<uint32_t>(RayTraceShaders.size());
    const uint32_t Sizing = GroupCount * AlignedHandleSize;
    std::vector<uint8_t> shaderHandleStorage(Sizing);

    VkResult result = vkGetRayTracingShaderGroupHandlesKHR(engine.Device, RayTracePipeline, 0, GroupCount, Sizing, shaderHandleStorage.data());

    raygenShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data());
    missShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + AlignedHandleSize);
    hitShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + AlignedHandleSize * 2);
}
void RayTraceRenderer::createDescriptorSets(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {
    { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
    };

    VkDescriptorPoolCreateInfo RayTraceDescriptorPoolInfo = {};
    RayTraceDescriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    RayTraceDescriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    RayTraceDescriptorPoolInfo.pPoolSizes = poolSizes.data();
    RayTraceDescriptorPoolInfo.maxSets = 1;
    vkCreateDescriptorPool(engine.Device, &RayTraceDescriptorPoolInfo, nullptr, &descriptorPool);

    VkDescriptorSetAllocateInfo RayTraceDescriptorSetAllocateInfo = {};
    RayTraceDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    RayTraceDescriptorSetAllocateInfo.descriptorPool = descriptorPool;
    RayTraceDescriptorSetAllocateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
    RayTraceDescriptorSetAllocateInfo.descriptorSetCount = 1;
    vkAllocateDescriptorSets(engine.Device, &RayTraceDescriptorSetAllocateInfo, &RTDescriptorSet);

    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = {};
    AccelerationDescriptorStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    AccelerationDescriptorStructure.accelerationStructureCount = 1;
    AccelerationDescriptorStructure.pAccelerationStructures = &topLevelAS.BufferHandle;

    VkWriteDescriptorSet AccelerationDesciptorSet = {};
    AccelerationDesciptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    AccelerationDesciptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    AccelerationDesciptorSet.dstSet = RTDescriptorSet;
    AccelerationDesciptorSet.dstBinding = 0;
    AccelerationDesciptorSet.descriptorCount = 1;
    AccelerationDesciptorSet.pNext = &AccelerationDescriptorStructure;

    VkDescriptorImageInfo RayTraceImageDescriptor{};
    RayTraceImageDescriptor.imageView = rayTexture.View;
    RayTraceImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet ImageDescriptorSet{};
    ImageDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    ImageDescriptorSet.dstSet = RTDescriptorSet;
    ImageDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    ImageDescriptorSet.dstBinding = 1;
    ImageDescriptorSet.pImageInfo = &RayTraceImageDescriptor;
    ImageDescriptorSet.descriptorCount = 1;

    VkDescriptorBufferInfo RTBufferInfo = {};
    RTBufferInfo.buffer = ubo.Buffer;
    RTBufferInfo.offset = 0;
    RTBufferInfo.range = ubo.BufferSize;

    VkWriteDescriptorSet UniformDescriptorSet{};
    UniformDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    UniformDescriptorSet.dstSet = RTDescriptorSet;
    UniformDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    UniformDescriptorSet.dstBinding = 2;
    UniformDescriptorSet.pBufferInfo = &RTBufferInfo;
    UniformDescriptorSet.descriptorCount = 1;


    VkDescriptorBufferInfo VertexBufferInfo = {};
    VertexBufferInfo.buffer = vertexBuffer.Buffer;
    VertexBufferInfo.offset = 0;
    VertexBufferInfo.range = vertexBuffer.BufferSize;

    VkWriteDescriptorSet VertexDescriptorSet{};
    VertexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    VertexDescriptorSet.dstSet = RTDescriptorSet;
    VertexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VertexDescriptorSet.dstBinding = 3;
    VertexDescriptorSet.pBufferInfo = &VertexBufferInfo;
    VertexDescriptorSet.descriptorCount = 1;

    VkDescriptorBufferInfo IndexBufferInfo = {};
    IndexBufferInfo.buffer = indexBuffer.Buffer;
    IndexBufferInfo.offset = 0;
    IndexBufferInfo.range = indexBuffer.BufferSize;

    VkWriteDescriptorSet IndexDescriptorSet{};
    IndexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    IndexDescriptorSet.dstSet = RTDescriptorSet;
    IndexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    IndexDescriptorSet.dstBinding = 4;
    IndexDescriptorSet.pBufferInfo = &IndexBufferInfo;
    IndexDescriptorSet.descriptorCount = 1;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        AccelerationDesciptorSet,
        ImageDescriptorSet,
        UniformDescriptorSet,
        VertexDescriptorSet,
        IndexDescriptorSet
    };
    vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);
}
void RayTraceRenderer::buildCommandBuffers(VulkanEngine& engine)
{
    drawCmdBuffers.resize(engine.SwapChain.GetSwapChainImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.GetRenderCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)drawCmdBuffers.size();

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, drawCmdBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    const uint32_t AlignedHandleSize = engine.GetShaderGroupAlignment(engine.PhysicalDevice);

    VkStridedDeviceAddressRegionKHR RaygenShader{};
    RaygenShader.deviceAddress = engine.BufferToDeviceAddress(raygenShaderBindingTable.GetBuffer()).deviceAddress;
    RaygenShader.stride = AlignedHandleSize;
    RaygenShader.size = AlignedHandleSize;

    VkStridedDeviceAddressRegionKHR MissShaderEntry{};
    MissShaderEntry.deviceAddress = engine.BufferToDeviceAddress(missShaderBindingTable.GetBuffer()).deviceAddress;
    MissShaderEntry.stride = AlignedHandleSize;
    MissShaderEntry.size = AlignedHandleSize;

    VkStridedDeviceAddressRegionKHR HitShaderEntry{};
    HitShaderEntry.deviceAddress = engine.BufferToDeviceAddress(hitShaderBindingTable.GetBuffer()).deviceAddress;
    HitShaderEntry.stride = AlignedHandleSize;
    HitShaderEntry.size = AlignedHandleSize;

    VkStridedDeviceAddressRegionKHR ShaderIndex{};
    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    for (size_t x = 0; x < drawCmdBuffers.size(); x++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(drawCmdBuffers[x], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        vkCmdBindPipeline(drawCmdBuffers[x], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipeline);
        vkCmdBindDescriptorSets(drawCmdBuffers[x], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipelineLayout, 0, 1, &RTDescriptorSet, 0, 0);
        vkCmdTraceRaysKHR(drawCmdBuffers[x], &RaygenShader, &MissShaderEntry, &HitShaderEntry, &ShaderIndex, engine.SwapChain.GetSwapChainResolution().width, engine.SwapChain.GetSwapChainResolution().height, 1);


        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.image = engine.SwapChain.GetSwapChainImages()[x];
        barrier.subresourceRange = subresourceRange;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(drawCmdBuffers[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageMemoryBarrier barrier2 = {};
        barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier2.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier2.image = rayTexture.Image;
        barrier2.subresourceRange = subresourceRange;
        barrier2.srcAccessMask = 0;
        barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(drawCmdBuffers[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

        VkImageCopy copyRegion{};
        copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        copyRegion.srcOffset = { 0, 0, 0 };
        copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        copyRegion.dstOffset = { 0, 0, 0 };
        copyRegion.extent = { engine.GetSwapChainResolution().width, engine.GetSwapChainResolution().height, 1 };
        vkCmdCopyImage(drawCmdBuffers[x], rayTexture.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, engine.SwapChain.SwapChainImages[x], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


        VkImageMemoryBarrier barrier3 = {};
        barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier3.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier3.image = engine.SwapChain.GetSwapChainImages()[x];
        barrier3.subresourceRange = subresourceRange;
        barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier3.dstAccessMask = 0;

        vkCmdPipelineBarrier(drawCmdBuffers[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

        VkImageMemoryBarrier barrier4 = {};
        barrier4.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier4.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier4.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier4.image = rayTexture.Image;
        barrier4.subresourceRange = subresourceRange;
        barrier4.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier4.dstAccessMask = 0;

        vkCmdPipelineBarrier(drawCmdBuffers[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier4);


        if (vkEndCommandBuffer(drawCmdBuffers[x]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}
void RayTraceRenderer::Resize(VulkanEngine& engine)
{
    rayTexture.RecreateRendererTexture(engine);
    VkDescriptorImageInfo storageImageDescriptor{ VK_NULL_HANDLE, rayTexture.View, VK_IMAGE_LAYOUT_GENERAL };

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = RTDescriptorSet;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.pImageInfo = &storageImageDescriptor;
    writeDescriptorSet.descriptorCount = 1;
    vkUpdateDescriptorSets(engine.Device, 1, &writeDescriptorSet, 0, VK_NULL_HANDLE);

    buildCommandBuffers(engine);
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

//
//void RayTraceRenderer::setImageLayout(VulkanEngine& engine,
//    VkCommandBuffer cmdbuffer,
//    VkImage image,
//    VkImageLayout oldImageLayout,
//    VkImageLayout newImageLayout,
//    VkImageSubresourceRange subresourceRange,
//    VkPipelineStageFlags srcStageMask,
//    VkPipelineStageFlags dstStageMask)
//{
//    // Create an image barrier object
//    VkImageMemoryBarrier imageMemoryBarrier{};
//    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    imageMemoryBarrier.oldLayout = oldImageLayout;
//    imageMemoryBarrier.newLayout = newImageLayout;
//    imageMemoryBarrier.image = image;
//    imageMemoryBarrier.subresourceRange = subresourceRange;
//
//    // Source layouts (old)
//    // Source access mask controls actions that have to be finished on the old layout
//    // before it will be transitioned to the new layout
//    switch (oldImageLayout)
//    {
//    case VK_IMAGE_LAYOUT_UNDEFINED:
//        // Image layout is undefined (or does not matter)
//        // Only valid as initial layout
//        // No flags required, listed only for completeness
//        imageMemoryBarrier.srcAccessMask = 0;
//        break;
//
//    case VK_IMAGE_LAYOUT_PREINITIALIZED:
//        // Image is preinitialized
//        // Only valid as initial layout for linear images, preserves memory contents
//        // Make sure host writes have been finished
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//        // Image is a color attachment
//        // Make sure any writes to the color buffer have been finished
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//        // Image is a depth/stencil attachment
//        // Make sure any writes to the depth/stencil buffer have been finished
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
//        // Image is a transfer source
//        // Make sure any reads from the image have been finished
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
//        // Image is a transfer destination
//        // Make sure any writes to the image have been finished
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//        // Image is read by a shader
//        // Make sure any shader reads from the image have been finished
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        break;
//    default:
//        // Other source layouts aren't handled (yet)
//        break;
//    }
//
//    // Target layouts (new)
//    // Destination access mask controls the dependency for the new image layout
//    switch (newImageLayout)
//    {
//    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
//        // Image will be used as a transfer destination
//        // Make sure any writes to the image have been finished
//        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
//        // Image will be used as a transfer source
//        // Make sure any reads from the image have been finished
//        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//        // Image will be used as a color attachment
//        // Make sure any writes to the color buffer have been finished
//        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//        // Image layout will be used as a depth/stencil attachment
//        // Make sure any writes to depth/stencil buffer have been finished
//        imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//        break;
//
//    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//        // Image will be read in a shader (sampler, input attachment)
//        // Make sure any writes to the image have been finished
//        if (imageMemoryBarrier.srcAccessMask == 0)
//        {
//            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
//        }
//        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        break;
//    default:
//        // Other source layouts aren't handled (yet)
//        break;
//    }
//
//    // Put barrier inside setup command buffer
//    vkCmdPipelineBarrier(
//        cmdbuffer,
//        srcStageMask,
//        dstStageMask,
//        0,
//        0, nullptr,
//        0, nullptr,
//        1, &imageMemoryBarrier);
//}

VkPipelineShaderStageCreateInfo RayTraceRenderer::loadShader(VulkanEngine& engine, std::string fileName, VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = stage;

    shaderStage.module = loadShader(fileName.c_str(), engine.Device);
    shaderStage.pName = "main";
    assert(shaderStage.module != VK_NULL_HANDLE);
    shaderModules.push_back(shaderStage.module);
    return shaderStage;
}

VkShaderModule RayTraceRenderer::loadShader(const char* fileName, VkDevice device)
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