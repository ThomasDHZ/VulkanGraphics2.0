#include "RayTraceRenderer.h"
#include <stdexcept>
#include <chrono>
#include <iostream>
RayTraceRenderer::RayTraceRenderer()
{

}
RayTraceRenderer::RayTraceRenderer(VkDevice Device, VkPhysicalDevice PhysicalDevice, VkCommandPool CommandPool, VkQueue GraphicsQueue, VkDescriptorPool DescriptorPool, uint32_t wIDTH, uint32_t hEIGHT, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages)
{
    device = Device;
    physicalDevice = PhysicalDevice;
        commandPool = CommandPool;
        graphicsQueue = GraphicsQueue;
        descriptorPool = DescriptorPool;
        WIDTH = wIDTH;
        HEIGHT = hEIGHT;

        rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rayTracingPipelineProperties;
        vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties2);

        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        VkPhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = &accelerationStructureFeatures;
        vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures2);

    vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));
    vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
    vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(device, "vkBuildAccelerationStructuresKHR"));
    vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
    vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
    vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
    vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
    vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
    vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
    vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));

    camera = std::make_shared<PerspectiveCamera>(glm::vec2(WIDTH, HEIGHT), glm::vec3(0.0f, 0.0f, 5.0f));

        model = RayTraceModel(device, physicalDevice, "C:/Users/dotha/source/repos/VulkanGraphics/Models/vulkanscene_shadow.obj");

        
            createBottomLevelAccelerationStructure(model.MeshList[2]);
        
       
        createTopLevelAccelerationStructure(model.MeshList[2], 0);
    
    createStorageImage();
    createUniformBuffer();
    createRayTracingPipeline();
    createShaderBindingTable();
   
        createDescriptorSets(model.MeshList[2]);
    
    buildCommandBuffers(swapChainFramebuffersSize, swapChainImages);
}
RayTraceRenderer::~RayTraceRenderer()
{

}

void RayTraceRenderer::createBottomLevelAccelerationStructure(RayTraceMesh& mesh)
{
    std::vector<RTVertex> vertices = {
    { {  1.0f,  1.0f, 0.0f } },
    { { -1.0f,  1.0f, 0.0f } },
    { {  0.0f, -1.0f, 0.0f } }
    };

    std::vector<uint32_t> indices = { 0, 1, 2 };

    MeshDetails meshContainer = {};
    meshContainer.vertices = vertices;
    meshContainer.indices = indices;


    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::rotate(transformMatrix, float(180), glm::vec3(0.0f, 0.0f, 1.0f));

    std::vector<uint32_t> PrimitiveCount;
    std::vector<VkAccelerationStructureGeometryKHR> AccelerationStructureGeometryList;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationBuildStructureRangeInfos;

        const uint32_t TriangleCount = mesh.TriangleCount;
        const uint32_t VertexCount = mesh.VertexCount;

        VkAccelerationStructureGeometryKHR AccelerationStructureGeometry = {};
        AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        AccelerationStructureGeometry.geometry.triangles.vertexData = mesh.vertexBufferDeviceAddress;
        AccelerationStructureGeometry.geometry.triangles.maxVertex = VertexCount;
        AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(RTVertex);
        AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
        AccelerationStructureGeometry.geometry.triangles.indexData = mesh.indexBufferDeviceAddress;
        AccelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = mesh.transformBufferDeviceAddress.deviceAddress;
        AccelerationStructureGeometry.geometry.triangles.transformData.hostAddress = mesh.transformBufferDeviceAddress.hostAddress;
        AccelerationStructureGeometryList.emplace_back(AccelerationStructureGeometry);

        VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};
        AccelerationStructureBuildRangeInfo.primitiveCount = TriangleCount;
        AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
        AccelerationStructureBuildRangeInfo.firstVertex = 0;
        AccelerationStructureBuildRangeInfo.transformOffset = 0;
        AccelerationBuildStructureRangeInfos.emplace_back(AccelerationStructureBuildRangeInfo);
    

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo = {};
    AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
    AccelerationStructureBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();

    PrimitiveCount.resize(AccelerationBuildStructureRangeInfos.size());
    for (auto x = 0; x < AccelerationBuildStructureRangeInfos.size(); x++)
    {
        PrimitiveCount[x] = AccelerationBuildStructureRangeInfos[x].primitiveCount;
    }

    VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo = {};
    AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, PrimitiveCount.data(), &AccelerationStructureBuildSizesInfo);

    createAccelerationStructure(mesh.bottomLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationStructureBuildSizesInfo);

    RayTracingScratchBuffer scratchBuffer = createScratchBuffer(AccelerationStructureBuildSizesInfo.buildScratchSize);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
    AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationBuildGeometryInfo.dstAccelerationStructure = mesh.bottomLevelAS.handle;
    AccelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
    AccelerationBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();
    AccelerationBuildGeometryInfo.srcAccelerationStructure = nullptr;
    AccelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;

    AcclerationCommandBuffer(AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

    deleteScratchBuffer(scratchBuffer);
}
void RayTraceRenderer::createTopLevelAccelerationStructure(RayTraceMesh& mesh, int x)
{
    uint32_t PrimitiveCount = 1;
    std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};

    
        auto transtransformMatrix2 = glm::transpose(transformMatrix2);
        VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transtransformMatrix2);

        VkAccelerationStructureInstanceKHR AccelerationStructureInstance{};
        AccelerationStructureInstance.transform = transformMatrix;
        AccelerationStructureInstance.instanceCustomIndex = x;
        AccelerationStructureInstance.mask = 0xFF;
        AccelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
        AccelerationStructureInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        AccelerationStructureInstance.accelerationStructureReference = mesh.bottomLevelAS.deviceAddress;
        AccelerationStructureInstanceList.emplace_back(AccelerationStructureInstance);
    

    VulkanBuffer instancesBuffer;
    instancesBuffer.CreateBuffer(device, physicalDevice, sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, AccelerationStructureInstanceList.data());

    VkDeviceOrHostAddressConstKHR DeviceOrHostAddressConst = {};
    DeviceOrHostAddressConst.deviceAddress = getBufferDeviceAddress(instancesBuffer.Buffer);

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
    vkGetAccelerationStructureBuildSizesKHR( device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, &PrimitiveCount, &accelerationStructureBuildSizesInfo);

   
        createAccelerationStructure(mesh.topLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, accelerationStructureBuildSizesInfo);
   

    RayTracingScratchBuffer ScratchBuffer = createScratchBuffer(accelerationStructureBuildSizesInfo.buildScratchSize);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo2 = {};
    AccelerationStructureBuildGeometryInfo2.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo2.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo2.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo2.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationStructureBuildGeometryInfo2.geometryCount = 1;
    AccelerationStructureBuildGeometryInfo2.pGeometries = &AccelerationStructureGeometry;
    AccelerationStructureBuildGeometryInfo2.scratchData.deviceAddress = ScratchBuffer.deviceAddress;
 
        AccelerationStructureBuildGeometryInfo2.dstAccelerationStructure = mesh.topLevelAS.handle;


    VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo = {};
    AccelerationStructureBuildRangeInfo.primitiveCount = static_cast<uint32_t>(AccelerationStructureInstanceList.size());
    AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
    AccelerationStructureBuildRangeInfo.firstVertex = 0;
    AccelerationStructureBuildRangeInfo.transformOffset = 0;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationStructureBuildRangeInfoList = { AccelerationStructureBuildRangeInfo };

    AcclerationCommandBuffer(AccelerationStructureBuildGeometryInfo2, AccelerationStructureBuildRangeInfoList);

    deleteScratchBuffer(ScratchBuffer);
    instancesBuffer.DestoryBuffer(device);
}

void RayTraceRenderer::createStorageImage()
{

    VkImageCreateInfo image = {};
    image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = VK_FORMAT_B8G8R8A8_UNORM;
    image.extent.width = WIDTH;
    image.extent.height = HEIGHT;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkCreateImage(device, &image, nullptr, &storageImage.image);

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device, storageImage.image, &memReqs);
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memReqs.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &storageImage.memory);
    vkBindImageMemory(device, storageImage.image, storageImage.memory, 0);

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
    vkCreateImageView(device, &colorImageView, nullptr, &storageImage.view);

    VkCommandBuffer cmdBuffer = createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    setImageLayout(cmdBuffer, storageImage.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
    flushCommandBuffer(cmdBuffer, graphicsQueue);
}
RayTracingScratchBuffer RayTraceRenderer::createScratchBuffer(VkDeviceSize size)
{
    RayTracingScratchBuffer scratchBuffer{};

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    vkCreateBuffer(device, &bufferCreateInfo, nullptr, &scratchBuffer.handle);

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(device, scratchBuffer.handle, &memoryRequirements);

    VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
    memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &scratchBuffer.memory);
    vkBindBufferMemory(device, scratchBuffer.handle, scratchBuffer.memory, 0);

    VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
    bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferDeviceAddressInfo.buffer = scratchBuffer.handle;
    scratchBuffer.deviceAddress = vkGetBufferDeviceAddressKHR(device, &bufferDeviceAddressInfo);

    return scratchBuffer;
}
void RayTraceRenderer::createUniformBuffer()
{
    ubo.CreateBuffer(device, physicalDevice, sizeof(UniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformData);
   // updateUniformBuffers();
}

void RayTraceRenderer::AcclerationCommandBuffer(VkAccelerationStructureBuildGeometryInfoKHR& AccelerationStructureBuildGeometryInfo, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& AccelerationStructureBuildRangeInfo)
{
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = commandPool;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(device, &CommandBufferAllocateInfo, &cmdBuffer);

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
    vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(device, fence, nullptr);
    vkFreeCommandBuffers(device, commandPool, 1, &cmdBuffer);
}

void RayTraceRenderer::updateUniformBuffers(GLFWwindow* window)
{
    keyboard.Update(window, camera);
    mouse.Update(window, camera);
    camera->Update(WIDTH, HEIGHT);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  
    uniformData.projInverse = glm::inverse(camera->GetProjectionMatrix());
    uniformData.viewInverse = glm::inverse(camera->GetViewMatrix());
    uniformData.modelInverse = glm::inverse(glm::mat4(1.0f));
    uniformData.lightPos = glm::vec4(cos(glm::radians(time * 360.0f)) * 40.0f, -50.0f + sin(glm::radians(time * 360.0f)) * 20.0f, 25.0f + sin(glm::radians(time * 360.0f)) * 5.0f, 0.0f);
    uniformData.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
    uniformData.vertexSize = sizeof(RTVertex);
    ubo.CopyBufferToMemory(device, &uniformData, sizeof(UniformData));

    //createTopLevelAccelerationStructure();
}

void RayTraceRenderer::UpdateAccelerationStructure(RayTraceMesh& mesh)
{
    uint32_t PrimitiveCount = 1;
    std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};

    auto transtransformMatrix2 = glm::transpose(transformMatrix2);
    VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transtransformMatrix2);

    VkAccelerationStructureInstanceKHR AccelerationStructureInstance{};
    AccelerationStructureInstance.transform = transformMatrix;
    //AccelerationStructureInstance.instanceCustomIndex = mesh.TLASID;
    AccelerationStructureInstance.mask = 0xFF;
    AccelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
    AccelerationStructureInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    AccelerationStructureInstance.accelerationStructureReference = mesh.bottomLevelAS.deviceAddress;
    AccelerationStructureInstanceList.emplace_back(AccelerationStructureInstance);

    VulkanBuffer instancesBuffer;
    instancesBuffer.CreateBuffer(device, physicalDevice,
        sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(),
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        AccelerationStructureInstanceList.data());

    VkDeviceOrHostAddressConstKHR DeviceOrHostAddressConst = {};
    DeviceOrHostAddressConst.deviceAddress = getBufferDeviceAddress(instancesBuffer.Buffer);

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
    vkGetAccelerationStructureBuildSizesKHR(
        device,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &AccelerationStructureBuildGeometryInfo,
        &PrimitiveCount,
        &accelerationStructureBuildSizesInfo);

    RayTracingScratchBuffer ScratchBuffer = createScratchBuffer(accelerationStructureBuildSizesInfo.buildScratchSize);

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo2 = {};
    AccelerationStructureBuildGeometryInfo2.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationStructureBuildGeometryInfo2.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationStructureBuildGeometryInfo2.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
    AccelerationStructureBuildGeometryInfo2.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationStructureBuildGeometryInfo2.geometryCount = 1;
    AccelerationStructureBuildGeometryInfo2.pGeometries = &AccelerationStructureGeometry;
    AccelerationStructureBuildGeometryInfo2.scratchData.deviceAddress = ScratchBuffer.deviceAddress;

   // AccelerationStructureBuildGeometryInfo2.srcAccelerationStructure = mesh.TLAS;
   // AccelerationStructureBuildGeometryInfo2.dstAccelerationStructure = mesh.TLAS;

    VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo = {};
    AccelerationStructureBuildRangeInfo.primitiveCount = static_cast<uint32_t>(AccelerationStructureInstanceList.size());
    AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
    AccelerationStructureBuildRangeInfo.firstVertex = 0;
    AccelerationStructureBuildRangeInfo.transformOffset = 0;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationStructureBuildRangeInfoList = { AccelerationStructureBuildRangeInfo };

    AcclerationCommandBuffer(AccelerationStructureBuildGeometryInfo2, AccelerationStructureBuildRangeInfoList);

    deleteScratchBuffer(ScratchBuffer);
    instancesBuffer.DestoryBuffer(device);
}

void RayTraceRenderer::createRayTracingPipeline()
{
    std::vector<VkDescriptorSetLayoutBinding> RTDescriptorSetBindings;
    VkDescriptorSetLayoutBinding              AccelerationStructureBinding = {};
    AccelerationStructureBinding.binding = 0;
    AccelerationStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    AccelerationStructureBinding.descriptorCount = 1;
    AccelerationStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
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

    VkDescriptorSetLayoutCreateInfo RTDescriptorSetLayout = {};
    RTDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    RTDescriptorSetLayout.bindingCount = static_cast<uint32_t>(RTDescriptorSetBindings.size());
    RTDescriptorSetLayout.pBindings = RTDescriptorSetBindings.data();
    vkCreateDescriptorSetLayout(device, &RTDescriptorSetLayout, nullptr, &RayTraceDescriptorSetLayout);

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.setLayoutCount = 1;
    PipelineLayoutCreateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
    vkCreatePipelineLayout(device, &PipelineLayoutCreateInfo, nullptr, &RayTracePipelineLayout);

    std::vector<VkPipelineShaderStageCreateInfo> ShaderList;

    ShaderList.emplace_back(loadShader("C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR RayGeneratorShaderInfo = {};
    RayGeneratorShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    RayGeneratorShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    RayGeneratorShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    RayGeneratorShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    RayGeneratorShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(RayGeneratorShaderInfo);

    ShaderList.emplace_back(loadShader("C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR MissShaderInfo = {};
    MissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    MissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    MissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    MissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    MissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(MissShaderInfo);

    ShaderList.emplace_back(loadShader("C:/Users/dotha/source/repos/VulkanGraphics/VulkanRayTraceTest/Shader/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
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
    VkResult result = vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &RayTracingPipeline, nullptr, &RayTracePipeline);

    for (auto shader : ShaderList)
    {
        vkDestroyShaderModule(device, shader.module, nullptr);
    }
}
void RayTraceRenderer::createShaderBindingTable() {
    const uint32_t handleSize = rayTracingPipelineProperties.shaderGroupHandleSize;
    const uint32_t handleSizeAligned = alignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);
    const uint32_t groupCount = static_cast<uint32_t>(RayTraceShaders.size());
    const uint32_t sbtSize = groupCount * handleSizeAligned;

    std::vector<uint8_t> shaderHandleStorage(sbtSize);
    vkGetRayTracingShaderGroupHandlesKHR(device, RayTracePipeline, 0, groupCount, sbtSize, shaderHandleStorage.data());

    raygenShaderBindingTable.CreateBuffer(device, physicalDevice, handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data());
    missShaderBindingTable.CreateBuffer(device, physicalDevice, handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned);
    hitShaderBindingTable.CreateBuffer(device, physicalDevice, handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + handleSizeAligned * 2);
}
void RayTraceRenderer::createDescriptorSets(RayTraceMesh& mesh)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1} };

    VkDescriptorPoolCreateInfo RayTraceDescriptorPoolInfo = {};
    RayTraceDescriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    RayTraceDescriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    RayTraceDescriptorPoolInfo.pPoolSizes = poolSizes.data();
    RayTraceDescriptorPoolInfo.maxSets = 1;
    vkCreateDescriptorPool(device, &RayTraceDescriptorPoolInfo, nullptr, &descriptorPool);

    VkDescriptorSetAllocateInfo RayTraceDescriptorSetAllocateInfo = {};
    RayTraceDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    RayTraceDescriptorSetAllocateInfo.descriptorPool = descriptorPool;
    RayTraceDescriptorSetAllocateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
    RayTraceDescriptorSetAllocateInfo.descriptorSetCount = 1;
    vkAllocateDescriptorSets(device, &RayTraceDescriptorSetAllocateInfo, &RTDescriptorSet);

    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = {};
    AccelerationDescriptorStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    AccelerationDescriptorStructure.accelerationStructureCount = 1;
    AccelerationDescriptorStructure.pAccelerationStructures = &mesh.topLevelAS.handle;

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
    VertexBufferInfo.buffer = mesh.vertexBuffer.Buffer;
    VertexBufferInfo.offset = 0;
    VertexBufferInfo.range = mesh.vertexBuffer.BufferSize;

    VkWriteDescriptorSet VertexDescriptorSet{};
    VertexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    VertexDescriptorSet.dstSet = RTDescriptorSet;
    VertexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VertexDescriptorSet.dstBinding = 3;
    VertexDescriptorSet.pBufferInfo = &VertexBufferInfo;
    VertexDescriptorSet.descriptorCount = 1;

    VkDescriptorBufferInfo IndexBufferInfo = {};
    IndexBufferInfo.buffer = mesh.indexBuffer.Buffer;
    IndexBufferInfo.offset = 0;
    IndexBufferInfo.range = mesh.indexBuffer.BufferSize;

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
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);


}
void RayTraceRenderer::buildCommandBuffers(int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages)
{
    drawCmdBuffers.resize(swapChainFramebuffersSize);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)drawCmdBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, drawCmdBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
        vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo);

        /*
            Setup the buffer regions pointing to the shaders in our shader binding table
        */

        const uint32_t handleSizeAligned = alignedSize(rayTracingPipelineProperties.shaderGroupHandleSize, rayTracingPipelineProperties.shaderGroupHandleAlignment);

        VkStridedDeviceAddressRegionKHR raygenShaderSbtEntry{};
        raygenShaderSbtEntry.deviceAddress = getBufferDeviceAddress(raygenShaderBindingTable.Buffer);
        raygenShaderSbtEntry.stride = handleSizeAligned;
        raygenShaderSbtEntry.size = handleSizeAligned;

        VkStridedDeviceAddressRegionKHR missShaderSbtEntry{};
        missShaderSbtEntry.deviceAddress = getBufferDeviceAddress(missShaderBindingTable.Buffer);
        missShaderSbtEntry.stride = handleSizeAligned;
        missShaderSbtEntry.size = handleSizeAligned;

        VkStridedDeviceAddressRegionKHR hitShaderSbtEntry{};
        hitShaderSbtEntry.deviceAddress = getBufferDeviceAddress(hitShaderBindingTable.Buffer);
        hitShaderSbtEntry.stride = handleSizeAligned;
        hitShaderSbtEntry.size = handleSizeAligned;

        VkStridedDeviceAddressRegionKHR callableShaderSbtEntry{};

        /*
            Dispatch the ray tracing commands
        */
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipeline);
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, RayTracePipelineLayout, 0, 1, &RTDescriptorSet, 0, 0);

        vkCmdTraceRaysKHR(
            drawCmdBuffers[i],
            &raygenShaderSbtEntry,
            &missShaderSbtEntry,
            &hitShaderSbtEntry,
            &callableShaderSbtEntry,
            WIDTH,
            HEIGHT,
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
        copyRegion.extent = { WIDTH, HEIGHT, 1 };
        vkCmdCopyImage(drawCmdBuffers[i], storageImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapChainImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


        VkImageMemoryBarrier barrier3 = {};
        barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier3.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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
void RayTraceRenderer::Resize(int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, uint32_t width, uint32_t height)
{
    WIDTH = width;
    HEIGHT = height;

    // Delete allocated resources
    vkDestroyImageView(device, storageImage.view, nullptr);
    vkDestroyImage(device, storageImage.image, nullptr);
    vkFreeMemory(device, storageImage.memory, nullptr);
    // Recreate image
    createStorageImage();
    // Update descriptor
    VkDescriptorImageInfo storageImageDescriptor{ VK_NULL_HANDLE, storageImage.view, VK_IMAGE_LAYOUT_GENERAL };

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = RTDescriptorSet;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.pImageInfo = &storageImageDescriptor;
    writeDescriptorSet.descriptorCount = 1;
    vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, VK_NULL_HANDLE);

    buildCommandBuffers(swapChainFramebuffersSize, swapChainImages);
}

void RayTraceRenderer::deleteScratchBuffer(RayTracingScratchBuffer& scratchBuffer)
{
    if (scratchBuffer.memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, scratchBuffer.memory, nullptr);
    }
    if (scratchBuffer.handle != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, scratchBuffer.handle, nullptr);
    }
}

void RayTraceRenderer::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool)
{
    if (commandBuffer == VK_NULL_HANDLE)
    {
        return;
    }

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
    // Submit to the queue
    vkQueueSubmit(queue, 1, &submitInfo, fence);
    // Wait for the fence to signal that command buffer has finished executing
    vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000);
    vkDestroyFence(device, fence, nullptr);

    vkFreeCommandBuffers(device, pool, 1, &commandBuffer);

}

void RayTraceRenderer::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue)
{
    return flushCommandBuffer(commandBuffer, queue, commandPool);
}

uint64_t RayTraceRenderer::getBufferDeviceAddress(VkBuffer buffer)
{
    VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
    bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bufferDeviceAI.buffer = buffer;
    return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
}
void RayTraceRenderer::createAccelerationStructure(AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo)
{
    // Buffer and memory
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = buildSizeInfo.accelerationStructureSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    vkCreateBuffer(device, &bufferCreateInfo, nullptr, &accelerationStructure.buffer);
    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(device, accelerationStructure.buffer, &memoryRequirements);
    VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
    memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = getMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &accelerationStructure.memory);
    vkBindBufferMemory(device, accelerationStructure.buffer, accelerationStructure.memory, 0);
    // Acceleration structure
    VkAccelerationStructureCreateInfoKHR accelerationStructureCreate_info{};
    accelerationStructureCreate_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    accelerationStructureCreate_info.buffer = accelerationStructure.buffer;
    accelerationStructureCreate_info.size = buildSizeInfo.accelerationStructureSize;
    accelerationStructureCreate_info.type = type;
    vkCreateAccelerationStructureKHR(device, &accelerationStructureCreate_info, nullptr, &accelerationStructure.handle);
    // AS device address
    VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
    accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    accelerationDeviceAddressInfo.accelerationStructure = accelerationStructure.handle;
    accelerationStructure.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &accelerationDeviceAddressInfo);
}

uint32_t RayTraceRenderer::getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties)
{
    VkBool32* memTypeFound = nullptr;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

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

VkCommandBuffer RayTraceRenderer::createCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = pool;
    commandBufferAllocateInfo.level = level;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &cmdBuffer);
    // If requested, also start recording for the new command buffer
    if (begin)
    {
        VkCommandBufferBeginInfo cmdBufferBeginInfo{};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    }
    return cmdBuffer;
}

VkCommandBuffer RayTraceRenderer::createCommandBuffer(VkCommandBufferLevel level, bool begin)
{
    return createCommandBuffer(level, commandPool, begin);
}

void RayTraceRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
VkResult RayTraceRenderer::createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer* buffer, VkDeviceSize size, void* data)
{
    buffer->device = device;

    // Create the buffer handle
    VkBufferCreateInfo bufCreateInfo{};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCreateInfo.usage = usageFlags;
    bufCreateInfo.size = size;
    vkCreateBuffer(device, &bufCreateInfo, nullptr, &buffer->buffer);

    // Create the memory backing up the buffer handle
    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memReqs);
    memAllocInfo.allocationSize = memReqs.size;
    // Find a memory type index that fits the properties of the buffer
    memAllocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
    // If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
    VkMemoryAllocateFlagsInfoKHR allocFlagsInfo{};
    if (usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
        allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
        allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        memAllocInfo.pNext = &allocFlagsInfo;
    }
    vkAllocateMemory(device, &memAllocInfo, nullptr, &buffer->memory);

    buffer->alignment = memReqs.alignment;
    buffer->size = size;
    buffer->usageFlags = usageFlags;
    buffer->memoryPropertyFlags = memoryPropertyFlags;

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data
    if (data != nullptr)
    {
        buffer->map();
        memcpy(buffer->mapped, data, size);
        if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
            buffer->flush();

        buffer->unmap();
    }

    // Initialize a default descriptor that covers the whole buffer size
    buffer->setupDescriptor();

    // Attach the memory to the buffer object
    return buffer->bind();
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
    // Create an image barrier object
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    // Source layouts (old)
    // Source access mask controls actions that have to be finished on the old layout
    // before it will be transitioned to the new layout
    switch (oldImageLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        // Image layout is undefined (or does not matter)
        // Only valid as initial layout
        // No flags required, listed only for completeness
        imageMemoryBarrier.srcAccessMask = 0;
        break;

    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Image is preinitialized
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image is a color attachment
        // Make sure any writes to the color buffer have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        // Image is a depth/stencil attachment
        // Make sure any writes to the depth/stencil buffer have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image is a transfer source
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image is a transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image is read by a shader
        // Make sure any shader reads from the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (newImageLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image will be used as a transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image will be used as a transfer source
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image will be used as a color attachment
        // Make sure any writes to the color buffer have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        // Image layout will be used as a depth/stencil attachment
        // Make sure any writes to depth/stencil buffer have been finished
        imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image will be read in a shader (sampler, input attachment)
        // Make sure any writes to the image have been finished
        if (imageMemoryBarrier.srcAccessMask == 0)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);
}

VkPipelineShaderStageCreateInfo RayTraceRenderer::loadShader(std::string fileName, VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = stage;

    shaderStage.module = loadShader(fileName.c_str(), device);
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

uint32_t RayTraceRenderer::alignedSize(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

