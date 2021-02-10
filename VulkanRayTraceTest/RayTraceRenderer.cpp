#include "RayTraceRenderer.h"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <stb_image.h>

RayTraceRenderer::RayTraceRenderer()
{

}
RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine, TextureManager& textureManagerz, std::vector<RayTraceModel>& modelList)
{
    textureManager = textureManagerz;
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
   
  camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));

 // ModelList.emplace_back(RayTraceModel(engine, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/Models/Sponza/Sponza.obj"));
 //ModelList.emplace_back(RayTraceModel(textureManager, device, physicalDevice, commandPool, graphicsQueue, "C:/Users/dotha/source/repos/VulkanGraphics/Models/viking_room.obj"));
 // ModelList.emplace_back(RayTraceModel(textureManager, device, physicalDevice, commandPool, graphicsQueue, "C:/Users/dotha/source/repos/VulkanGraphics/Models/Medieval_building.obj"));
  //  ModelList.emplace_back(RayTraceModel(textureManager, device, physicalDevice, commandPool, graphicsQueue, "C:/Users/dotha/source/repos/VulkanGraphics/Models/vulkanscene_shadow.obj"));

 // textureManager.LoadTexture(engine, "C:/Users/dotha/source/repos/VulkanGraphics/Models/viking_room.png", VK_FORMAT_R8G8B8A8_UNORM);
 textureManager.LoadTexture(engine, "C:/Users/dotha/source/repos/VulkanGraphics/texture/texture.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    stbi_set_flip_vertically_on_load(true);
    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/right.jpg";
    CubeMapFiles[1] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/left.jpg";
    CubeMapFiles[2] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/bottom.jpg";
    CubeMapFiles[3] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/top.jpg";
    CubeMapFiles[4] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/back.jpg";
    CubeMapFiles[5] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/front.jpg";

    textureManager.LoadCubeMap(engine, CubeMapFiles);

    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            createBottomLevelAccelerationStructure(engine, ModelList[x], ModelList[x].MeshList[y]);
        }
    }
   createTopLevelAccelerationStructure(engine);
   createStorageImage(engine);
   createRayTracingPipeline(engine);
   createShaderBindingTable(engine);
   createSceneDataBuffer(engine);
   createDescriptorSets(engine);
   buildCommandBuffers(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages);
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
    {
        vkDestroyImageView(engine.Device, storageImage.view, nullptr);
        vkDestroyImage(engine.Device, storageImage.image, nullptr);
        vkFreeMemory(engine.Device, storageImage.memory, nullptr);

        storageImage.view = VK_NULL_HANDLE;
        storageImage.image = VK_NULL_HANDLE;
        storageImage.memory = VK_NULL_HANDLE;
    }
    {
        vkDestroyPipeline(engine.Device, RayTracePipeline, nullptr);
        vkDestroyPipelineLayout(engine.Device, RayTracePipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, RayTraceDescriptorSetLayout, nullptr);

        RayTracePipeline = VK_NULL_HANDLE;
        RayTracePipelineLayout = VK_NULL_HANDLE;
        RayTraceDescriptorSetLayout = VK_NULL_HANDLE;
    }
    {
        raygenShaderBindingTable.DestoryBuffer(engine.Device);
        missShaderBindingTable.DestoryBuffer(engine.Device);
        hitShaderBindingTable.DestoryBuffer(engine.Device);
        SceneDataBuffer.DestoryBuffer(engine.Device);

        vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

   // model.Destory(device);
    {
       // vkDestroySampler(device, DiffuseMap.textureSampler, nullptr);
       // vkDestroyImageView(device, DiffuseMap.textureImageView, nullptr);
       // vkDestroyImage(device, DiffuseMap.textureImage, nullptr);
       // vkFreeMemory(device, DiffuseMap.textureImageMemory, nullptr);

      //  DiffuseMap.textureSampler = VK_NULL_HANDLE;
      //  DiffuseMap.textureImageView = VK_NULL_HANDLE;
      //  DiffuseMap.textureImage = VK_NULL_HANDLE;
      //  DiffuseMap.textureImageMemory = VK_NULL_HANDLE;
    }
    {
    /*    vkDestroySampler(device, CubeMap.textureSampler, nullptr);
        vkDestroyImageView(device, CubeMap.textureImageView, nullptr);
        vkDestroyImage(device, CubeMap.textureImage, nullptr);
        vkFreeMemory(device, CubeMap.textureImageMemory, nullptr);

        CubeMap.textureSampler = VK_NULL_HANDLE;
        CubeMap.textureImageView = VK_NULL_HANDLE;
        CubeMap.textureImage = VK_NULL_HANDLE;
        CubeMap.textureImageMemory = VK_NULL_HANDLE;*/
    }
}

void RayTraceRenderer::createBottomLevelAccelerationStructure(VulkanEngine& engine, RayTraceModel& model, Mesh& mesh)
{
    AccelerationStructure bottomLevelAS{};

    std::vector<uint32_t> PrimitiveCountList;
    uint32_t FirstVertex = 0;
    uint32_t PrimitiveOffset = 0;
    std::vector<VkAccelerationStructureGeometryKHR> AccelerationStructureGeometryList;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationBuildStructureRangeInfos;
    for (auto x = 0; x < 1; x++)
    {
        PrimitiveCountList.emplace_back(mesh.TriangleCount);

        VkAccelerationStructureGeometryKHR AccelerationStructureGeometry = {};
        AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        AccelerationStructureGeometry.geometry.triangles.vertexData = mesh.VertexBufferDeviceAddress;
        AccelerationStructureGeometry.geometry.triangles.maxVertex = mesh.VertexCount;
        AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
        AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
        AccelerationStructureGeometry.geometry.triangles.indexData = mesh.IndexBufferDeviceAddress;
        AccelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = mesh.TransformBufferDeviceAddress.deviceAddress;
        AccelerationStructureGeometry.geometry.triangles.transformData.hostAddress = mesh.TransformBufferDeviceAddress.hostAddress;
        AccelerationStructureGeometryList.emplace_back(AccelerationStructureGeometry);

        VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};
        AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCountList[x];
        AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
        AccelerationStructureBuildRangeInfo.firstVertex = 0;
        AccelerationStructureBuildRangeInfo.transformOffset = 0;
        AccelerationBuildStructureRangeInfos.emplace_back(AccelerationStructureBuildRangeInfo);

        VertexBufferList.emplace_back(mesh.VertexBuffer);
        IndexBufferList.emplace_back(mesh.IndexBuffer);
    }
 
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

    RayTracingScratchBuffer scratchBuffer = createScratchBuffer(engine, AccelerationStructureBuildSizesInfo.buildScratchSize);

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

    AcclerationCommandBuffer(engine, AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

    deleteScratchBuffer(engine, scratchBuffer);
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

    VulkanBuffer instancesBuffer;
    instancesBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, AccelerationStructureInstanceList.data());

    VkDeviceOrHostAddressConstKHR DeviceOrHostAddressConst = {};
    DeviceOrHostAddressConst.deviceAddress = getBufferDeviceAddress(engine, instancesBuffer.Buffer);

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

    RayTracingScratchBuffer ScratchBuffer = createScratchBuffer(engine, accelerationStructureBuildSizesInfo.buildScratchSize);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo2 = {};
    AccelerationStructureBuildGeometryInfo2.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo2.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo2.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo2.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationStructureBuildGeometryInfo2.geometryCount = 1;
    AccelerationStructureBuildGeometryInfo2.pGeometries = &AccelerationStructureGeometry;
    AccelerationStructureBuildGeometryInfo2.scratchData.deviceAddress = ScratchBuffer.deviceAddress;

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

    deleteScratchBuffer(engine, ScratchBuffer);
    instancesBuffer.DestoryBuffer(engine.Device);
}

void RayTraceRenderer::createStorageImage(VulkanEngine& engine)
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
    vkCreateImage(engine.Device, &image, nullptr, &storageImage.image);

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(engine.Device, storageImage.image, &memReqs);
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memReqs.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &storageImage.memory);
    vkBindImageMemory(engine.Device, storageImage.image, storageImage.memory, 0);

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
    colorImageView.image = storageImage.image;
    vkCreateImageView(engine.Device, &colorImageView, nullptr, &storageImage.view);

    VkCommandBuffer cmdBuffer = createCommandBuffer(engine, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    setImageLayout(cmdBuffer, storageImage.image,
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

RayTracingScratchBuffer RayTraceRenderer::createScratchBuffer(VulkanEngine& engine, VkDeviceSize size)
{
    RayTracingScratchBuffer scratchBuffer{};

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    vkCreateBuffer(engine.Device, &bufferCreateInfo, nullptr, &scratchBuffer.handle);

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(engine.Device, scratchBuffer.handle, &memoryRequirements);

    VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
    memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(engine, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &scratchBuffer.memory);
    vkBindBufferMemory(engine.Device, scratchBuffer.handle, scratchBuffer.memory, 0);

    VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
    bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferDeviceAddressInfo.buffer = scratchBuffer.handle;
    scratchBuffer.deviceAddress = vkGetBufferDeviceAddressKHR(engine.Device, &bufferDeviceAddressInfo);

    return scratchBuffer;
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

void RayTraceRenderer::updateUniformBuffers(VulkanEngine& engine, GLFWwindow* window, SceneDataBufferData& sceneData)
{
    keyboard.Update(window, camera);
    mouse.Update(window, camera);
    camera->Update(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    ModelList[0].ModelTransform = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
   // ModelList[0].Update();


    sceneData.model = ModelList[0].ModelTransform;
    sceneData.viewInverse = glm::inverse(glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    sceneData.projInverse = glm::inverse(glm::perspective(glm::radians(45.0f), engine.SwapChain.SwapChainResolution.width / (float)engine.SwapChain.SwapChainResolution.height, 0.1f, 10.0f));
    sceneData.projInverse[1][1] *= -1;
    sceneData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    sceneData.proj = glm::perspective(glm::radians(45.0f), engine.SwapChain.SwapChainResolution.width / (float)engine.SwapChain.SwapChainResolution.height, 0.1f, 10.0f);
    sceneData.proj[1][1] *= -1;
    sceneData.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
    sceneData.vertexSize = sizeof(Vertex);
    SceneDataBuffer.CopyBufferToMemory(engine.Device, &sceneData, sizeof(sceneData));

    createTopLevelAccelerationStructure(engine);
}

void RayTraceRenderer::createRayTracingPipeline(VulkanEngine& engine)
{
    std::vector<VkDescriptorSetLayoutBinding> RTDescriptorSetBindings;
    VkDescriptorSetLayoutBinding              AccelerationStructureBinding = {};
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
    UniformBufferStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
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

    VkDescriptorSetLayoutBinding MaterialBufferStructureBinding = {};
    MaterialBufferStructureBinding.binding = 5;
    MaterialBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    MaterialBufferStructureBinding.descriptorCount = 1;
    MaterialBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(MaterialBufferStructureBinding);

    VkDescriptorSetLayoutBinding DiffuseBinding = {};
    DiffuseBinding.binding = 6;
    DiffuseBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DiffuseBinding.descriptorCount = static_cast<uint32_t>(textureManager.GetTextureList().size());
    DiffuseBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(DiffuseBinding);

    VkDescriptorSetLayoutBinding CubeMapStructureBinding = {};
    CubeMapStructureBinding.binding = 10;
    CubeMapStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    CubeMapStructureBinding.descriptorCount = 1;
    CubeMapStructureBinding.stageFlags = VK_SHADER_STAGE_MISS_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(CubeMapStructureBinding);

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
void RayTraceRenderer::createSceneDataBuffer(VulkanEngine& engine)
{

    SceneDataBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(SceneDataBufferData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
void RayTraceRenderer::createDescriptorSets(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4},
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 }};

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
    AccelerationDescriptorStructure.pAccelerationStructures = &topLevelAS.handle;

    VkWriteDescriptorSet AccelerationDesciptorSet = {};
    AccelerationDesciptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    AccelerationDesciptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    AccelerationDesciptorSet.dstSet = RTDescriptorSet;
    AccelerationDesciptorSet.dstBinding = 0;
    AccelerationDesciptorSet.descriptorCount = 1;
    AccelerationDesciptorSet.pNext = &AccelerationDescriptorStructure;

    VkDescriptorImageInfo RayTraceImageDescriptor{};
    RayTraceImageDescriptor.imageView = storageImage.view;
    RayTraceImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet ImageDescriptorSet{};
    ImageDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    ImageDescriptorSet.dstSet = RTDescriptorSet;
    ImageDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    ImageDescriptorSet.dstBinding = 1;
    ImageDescriptorSet.pImageInfo = &RayTraceImageDescriptor;
    ImageDescriptorSet.descriptorCount = 1;

    VkDescriptorBufferInfo UniformBuffer = {};
    UniformBuffer.buffer = SceneDataBuffer.Buffer;
    UniformBuffer.offset = 0;
    UniformBuffer.range = SceneDataBuffer.BufferSize;

    VkWriteDescriptorSet UniformDescriptorSet{};
    UniformDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    UniformDescriptorSet.dstSet = RTDescriptorSet;
    UniformDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    UniformDescriptorSet.dstBinding = 2;
    UniformDescriptorSet.pBufferInfo = &UniformBuffer;
    UniformDescriptorSet.descriptorCount = 1;

    //std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    //std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    //std::vector<VkDescriptorBufferInfo> OffsetBufferInfoList;
    //for (int x = 0; x < VertexBufferList.size(); x++)
    //{
        VkDescriptorBufferInfo VertexBufferInfo = {};
        VertexBufferInfo.buffer = ModelList[0].MeshList[0].VertexBuffer.Buffer;
        VertexBufferInfo.offset = 0;
        VertexBufferInfo.range = VK_WHOLE_SIZE;
    //    VertexBufferInfoList.emplace_back(VertexBufferInfo);
    //}
    //    for (int x = 0; x < IndexBufferList.size(); x++)
    //    {
        VkDescriptorBufferInfo IndexBufferInfo = {};
        IndexBufferInfo.buffer = ModelList[0].MeshList[0].IndexBuffer.Buffer;
        IndexBufferInfo.offset = 0;
        IndexBufferInfo.range = VK_WHOLE_SIZE;
    //    IndexBufferInfoList.emplace_back(IndexBufferInfo);
    //}


    VkWriteDescriptorSet VertexDescriptorSet{};
    VertexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    VertexDescriptorSet.dstSet = RTDescriptorSet;
    VertexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VertexDescriptorSet.dstBinding = 3;
    VertexDescriptorSet.descriptorCount = 1;
    VertexDescriptorSet.pBufferInfo = &VertexBufferInfo;

    VkWriteDescriptorSet IndexDescriptorSet{};
    IndexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    IndexDescriptorSet.dstSet = RTDescriptorSet;
    IndexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    IndexDescriptorSet.dstBinding = 4;
    IndexDescriptorSet.descriptorCount = 1;
    IndexDescriptorSet.pBufferInfo = &IndexBufferInfo;

    std::vector<Material> MaterialList;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            MaterialList.emplace_back(ModelList[x].MeshList[y].material);
        }
    }
    MaterialBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(Material) * MaterialList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, MaterialList.data());

    VkDescriptorBufferInfo MaterialBufferInfo = {};
    MaterialBufferInfo.buffer = MaterialBuffer.Buffer;
    MaterialBufferInfo.offset = 0;
    MaterialBufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet MaterialDescriptorSet{};
    MaterialDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    MaterialDescriptorSet.dstSet = RTDescriptorSet;
    MaterialDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    MaterialDescriptorSet.dstBinding = 5;
    MaterialDescriptorSet.pBufferInfo = &MaterialBufferInfo;
    MaterialDescriptorSet.descriptorCount = 1;

    std::vector<VkDescriptorImageInfo> DiffuseMapInfoList;
    for (auto texture : textureManager.GetTextureList())
    {
        VkDescriptorImageInfo DiffuseMapImage = {};
        DiffuseMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DiffuseMapImage.imageView = texture->GetTextureView();
        DiffuseMapImage.sampler = texture->GetTextureSampler();
        DiffuseMapInfoList.emplace_back(DiffuseMapImage);
    }
    VkWriteDescriptorSet DiffuseMapDescriptor = {};
    DiffuseMapDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    DiffuseMapDescriptor.dstSet = RTDescriptorSet;
    DiffuseMapDescriptor.dstBinding = 6;
    DiffuseMapDescriptor.dstArrayElement = 0;
    DiffuseMapDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DiffuseMapDescriptor.descriptorCount = static_cast<uint32_t>(textureManager.GetTextureList().size());
    DiffuseMapDescriptor.pImageInfo = DiffuseMapInfoList.data();

    VkDescriptorImageInfo CubeMapImage = {};
    CubeMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    CubeMapImage.imageView = textureManager.GetCubeMapTexture().GetTextureView();
    CubeMapImage.sampler = textureManager.GetCubeMapTexture().GetTextureSampler();

    VkWriteDescriptorSet CubeMapDescriptor = {};
    CubeMapDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    CubeMapDescriptor.dstSet = RTDescriptorSet;
    CubeMapDescriptor.dstBinding = 10;
    CubeMapDescriptor.dstArrayElement = 0;
    CubeMapDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    CubeMapDescriptor.descriptorCount = 1;
    CubeMapDescriptor.pImageInfo = &CubeMapImage;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        AccelerationDesciptorSet,
        ImageDescriptorSet,
        UniformDescriptorSet,
        VertexDescriptorSet,
        IndexDescriptorSet,
        MaterialDescriptorSet,
        DiffuseMapDescriptor,
        CubeMapDescriptor
    };
    vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);


}
void RayTraceRenderer::buildCommandBuffers(VulkanEngine& engine, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages)
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
        raygenShaderSbtEntry.deviceAddress = getBufferDeviceAddress(engine, raygenShaderBindingTable.Buffer);
        raygenShaderSbtEntry.stride = handleSizeAligned;
        raygenShaderSbtEntry.size = handleSizeAligned;

        VkStridedDeviceAddressRegionKHR missShaderSbtEntry{};
        missShaderSbtEntry.deviceAddress = getBufferDeviceAddress(engine, missShaderBindingTable.Buffer);
        missShaderSbtEntry.stride = handleSizeAligned;
        missShaderSbtEntry.size = handleSizeAligned;

        VkStridedDeviceAddressRegionKHR hitShaderSbtEntry{};
        hitShaderSbtEntry.deviceAddress = getBufferDeviceAddress(engine, hitShaderBindingTable.Buffer);
        hitShaderSbtEntry.stride = handleSizeAligned;
        hitShaderSbtEntry.size = handleSizeAligned;

        VkStridedDeviceAddressRegionKHR callableShaderSbtEntry{};

        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipeline);
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipelineLayout, 0, 1, &RTDescriptorSet, 0, 0);

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
void RayTraceRenderer::Resize(VulkanEngine& engine, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, uint32_t width, uint32_t height)
{
    vkDestroyImageView(engine.Device, storageImage.view, nullptr);
    vkDestroyImage(engine.Device, storageImage.image, nullptr);
    vkFreeMemory(engine.Device, storageImage.memory, nullptr);

    createStorageImage(engine);

    VkDescriptorImageInfo storageImageDescriptor{ VK_NULL_HANDLE, storageImage.view, VK_IMAGE_LAYOUT_GENERAL };

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = RTDescriptorSet;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.pImageInfo = &storageImageDescriptor;
    writeDescriptorSet.descriptorCount = 1;
    vkUpdateDescriptorSets(engine.Device, 1, &writeDescriptorSet, 0, VK_NULL_HANDLE);

    buildCommandBuffers(engine, swapChainFramebuffersSize, swapChainImages);
}

void RayTraceRenderer::deleteScratchBuffer(VulkanEngine& engine, RayTracingScratchBuffer& scratchBuffer)
{
    if (scratchBuffer.memory != VK_NULL_HANDLE) {
        vkFreeMemory(engine.Device, scratchBuffer.memory, nullptr);
    }
    if (scratchBuffer.handle != VK_NULL_HANDLE) {
        vkDestroyBuffer(engine.Device, scratchBuffer.handle, nullptr);
    }
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

uint64_t RayTraceRenderer::getBufferDeviceAddress(VulkanEngine& engine, VkBuffer buffer)
{
    VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
    bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferDeviceAI.buffer = buffer;
    return vkGetBufferDeviceAddressKHR(engine.Device, &bufferDeviceAI);
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


VkCommandBuffer  RayTraceRenderer::beginSingleTimeCommands(VulkanEngine& engine) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = engine.CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(engine.Device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void  RayTraceRenderer::endSingleTimeCommands(VulkanEngine& engine, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(engine.GraphicsQueue);

    vkFreeCommandBuffers(engine.Device, engine.CommandPool, 1, &commandBuffer);
}
uint32_t RayTraceRenderer::findMemoryType(VulkanEngine& engine, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(engine.PhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

uint64_t RayTraceRenderer::getBufferDeviceAddress(VkDevice& device, VkBuffer buffer)
{
    VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
    bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferDeviceAI.buffer = buffer;
    return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
}
