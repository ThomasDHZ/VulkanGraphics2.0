#include "RayTraceRenderer.h"
#include <stdexcept>
#include <chrono>
#include <iostream>

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

    std::shared_ptr<TextureManager> manager = std::make_shared<TextureManager>(engine);
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();

    model = Model(engine, manager, "C:/Users/dotha/source/repos/VulkanGraphics/Models/vulkanscene_shadow.obj", RayTraceDescriptorSetLayout, 1, texture);
    texture2D = Texture2D(engine, VK_FORMAT_R8G8B8A8_UNORM, "C:/Users/dotha/source/repos/VulkanGraphics/texture/Brick_diffuseOriginal.bmp", 1);
    NormalMap = Texture2D(engine, VK_FORMAT_R8G8B8A8_UNORM, "C:/Users/dotha/source/repos/VulkanGraphics/texture/Brick_normal.bmp", 1);

    CubeMapLayout CubeMapFiles;
    CubeMapFiles.Left = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/left.jpg";
    CubeMapFiles.Right = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/right.jpg";
    CubeMapFiles.Top = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/top.jpg";
    CubeMapFiles.Bottom = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/bottom.jpg";
    CubeMapFiles.Back = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/back.jpg";
    CubeMapFiles.Front = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/front.jpg";
    CubeMap = CubeMapTexture(engine, CubeMapFiles);

    for (auto mesh : model.SubMeshList)
    {
        createBottomLevelAccelerationStructure(engine, mesh);
    }
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

void RayTraceRenderer::AddMesh(VulkanEngine& engine)
{
}


void RayTraceRenderer::createBottomLevelAccelerationStructure(VulkanEngine& engine, MeshData& mesh)
{
    AccelerationStructure bottomLevelAS = {};
   

    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::rotate(transformMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));


        uint32_t numTriangles = static_cast<uint32_t>(mesh.IndexList.size()) / 3;
        uint32_t maxVertex = mesh.VertexList.size();

        VulkanBuffer vertexBuffer;
        VulkanBuffer indexBuffer;
        vertexBuffer.CreateBuffer(engine, mesh.VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.VertexList.data());
        indexBuffer.CreateBuffer(engine, mesh.IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.IndexList.data());
        transformBuffer.CreateBuffer(engine, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &transformMatrix);

        VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress = engine.BufferToDeviceAddress(vertexBuffer.Buffer);
        VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress = engine.BufferToDeviceAddress(indexBuffer.Buffer);
        VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress = engine.BufferToDeviceAddress(transformBuffer.Buffer);

        VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
        accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        accelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        accelerationStructureGeometry.geometry.triangles.vertexData = vertexBufferDeviceAddress;
        accelerationStructureGeometry.geometry.triangles.maxVertex = maxVertex;
        accelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
        accelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
        accelerationStructureGeometry.geometry.triangles.indexData = indexBufferDeviceAddress;
        accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = transformBufferDeviceAddress.deviceAddress;
        accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = transformBufferDeviceAddress.hostAddress;
        GeometryAccelerationStructureList.emplace_back(accelerationStructureGeometry);

        VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
        accelerationStructureBuildRangeInfo.primitiveCount = numTriangles;
        accelerationStructureBuildRangeInfo.primitiveOffset = 0;
        accelerationStructureBuildRangeInfo.firstVertex = 0;
        accelerationStructureBuildRangeInfo.transformOffset = 0;
        AcclerationBuildRangeList.emplace_back(accelerationStructureBuildRangeInfo);
    

    VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
    accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    accelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(GeometryAccelerationStructureList.size());
    accelerationStructureBuildGeometryInfo.pGeometries = GeometryAccelerationStructureList.data();

    maxPrimCount.resize(AcclerationBuildRangeList.size());
    for (auto x = 0; x < AcclerationBuildRangeList.size(); x++)
    {
        maxPrimCount[x] = AcclerationBuildRangeList[x].primitiveCount;
    }

    VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
    accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(
        engine.Device,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &accelerationStructureBuildGeometryInfo,
        maxPrimCount.data(),
        &accelerationStructureBuildSizesInfo);
    createAccelerationStructure(engine, bottomLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, accelerationStructureBuildSizesInfo);

    VulkanBuffer ScratchBuffer = VulkanBuffer();
    ScratchBuffer.CreateBuffer(engine, accelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ScratchBuffer.BufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.GetBuffer()).deviceAddress;

    VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
    accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    accelerationBuildGeometryInfo.dstAccelerationStructure = bottomLevelAS.handle;
    accelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(GeometryAccelerationStructureList.size());
    accelerationBuildGeometryInfo.pGeometries = GeometryAccelerationStructureList.data();
    accelerationBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer.BufferDeviceAddress;

    AcclerationCommandBuffer(engine, accelerationBuildGeometryInfo, AcclerationBuildRangeList);
    ScratchBuffer.DestoryBuffer(engine);
    vertexBufferList.emplace_back(vertexBuffer);
    indexBufferList.emplace_back(indexBuffer);
    bottomLevelASList.emplace_back(bottomLevelAS);
}
void RayTraceRenderer::createTopLevelAccelerationStructure(VulkanEngine& engine)
{
    std::vector<VkAccelerationStructureInstanceKHR> AccelerationStructureInstanceList = {};
    for (auto x = 0; x < bottomLevelASList.size(); x++)
    {
        glm::mat4 Transform = glm::mat4(1.0f);
        Transform = glm::translate(Transform, glm::vec3(0.0f));
        Transform = glm::transpose(Transform);
        VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(Transform);

        VkAccelerationStructureInstanceKHR AccelerationInstance = {};
        AccelerationInstance.transform = transformMatrix;
        AccelerationInstance.instanceCustomIndex = x;
        AccelerationInstance.mask = 0xFF;
        AccelerationInstance.instanceShaderBindingTableRecordOffset = 0;
        AccelerationInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        AccelerationInstance.accelerationStructureReference = bottomLevelASList[10].deviceAddress;
        AccelerationStructureInstanceList.emplace_back(AccelerationInstance);
    }

    VulkanBuffer instancesBuffer;
    instancesBuffer.CreateBuffer(engine, 
        sizeof(VkAccelerationStructureInstanceKHR) * AccelerationStructureInstanceList.size(),
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        AccelerationStructureInstanceList.data());

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

    uint32_t primitive_count = 1;
    VkAccelerationStructureBuildSizesInfoKHR AccelerationBuildInfo = {};
    AccelerationBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometry, &primitive_count, &AccelerationBuildInfo);

    createAccelerationStructure(engine, topLevelAS, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationBuildInfo);

    VulkanBuffer ScratchBuffer = VulkanBuffer();
    ScratchBuffer.CreateBuffer(engine, AccelerationBuildInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ScratchBuffer.BufferDeviceAddress = engine.BufferToDeviceAddress(ScratchBuffer.GetBuffer()).deviceAddress;

    VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
    AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    AccelerationBuildGeometryInfo.dstAccelerationStructure = topLevelAS.handle;
    AccelerationBuildGeometryInfo.geometryCount = 1;
    AccelerationBuildGeometryInfo.pGeometries = &AccelerationGeometry;
    AccelerationBuildGeometryInfo.scratchData.deviceAddress = ScratchBuffer.BufferDeviceAddress;

    VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
    accelerationStructureBuildRangeInfo.primitiveCount = 1;
    accelerationStructureBuildRangeInfo.primitiveOffset = 0;
    accelerationStructureBuildRangeInfo.firstVertex = 0;
    accelerationStructureBuildRangeInfo.transformOffset = 0;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> accelerationBuildStructureRangeInfos = { accelerationStructureBuildRangeInfo };

    AcclerationCommandBuffer(engine, AccelerationBuildGeometryInfo, accelerationBuildStructureRangeInfos);
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
    //vertexBuffer.DestoryBuffer(engine);
   // indexBuffer.DestoryBuffer(engine);
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

    glm::mat4 transformMatrix(1.0f);

    uniformData.projInverse = glm::inverse(camera->GetProjectionMatrix());
    uniformData.viewInverse = glm::inverse(camera->GetViewMatrix());
    uniformData.modelInverse = glm::inverse(transformMatrix);
    uniformData.lightPos = glm::vec4(cos(glm::radians(time * 360.0f)) * 40.0f, -50.0f + sin(glm::radians(time * 360.0f)) * 20.0f, 25.0f + sin(glm::radians(time * 360.0f)) * 5.0f, 0.0f);
    uniformData.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
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
    VertexBufferStructureBinding.descriptorCount = 3;
    VertexBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(VertexBufferStructureBinding);

    VkDescriptorSetLayoutBinding IndexBufferStructureBinding = {};
    IndexBufferStructureBinding.binding = 4;
    IndexBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    IndexBufferStructureBinding.descriptorCount = 3;
    IndexBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(IndexBufferStructureBinding);

    VkDescriptorSetLayoutBinding DiffuseBinding = {};
    DiffuseBinding.binding = 5;
    DiffuseBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DiffuseBinding.descriptorCount = 1;
    DiffuseBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(DiffuseBinding);

    VkDescriptorSetLayoutBinding NormalBinding = {};
    NormalBinding.binding = 6;
    NormalBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    NormalBinding.descriptorCount = 1;
    NormalBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(NormalBinding);

    VkDescriptorSetLayoutBinding CubeMapBinding = {};
    CubeMapBinding.binding = 7;
    CubeMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    CubeMapBinding.descriptorCount = 1;
    CubeMapBinding.stageFlags = VK_SHADER_STAGE_MISS_BIT_KHR;
    RTDescriptorSetBindings.emplace_back(CubeMapBinding);

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

    ShaderList.emplace_back(loadShader(engine, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/shaders/shadow.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
    VkRayTracingShaderGroupCreateInfoKHR ShadowMissShaderInfo = {};
    ShadowMissShaderInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    ShadowMissShaderInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    ShadowMissShaderInfo.generalShader = static_cast<uint32_t>(ShaderList.size()) - 1;
    ShadowMissShaderInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
    ShadowMissShaderInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
    ShadowMissShaderInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
    RayTraceShaders.emplace_back(ShadowMissShaderInfo);

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
    RayTracingPipeline.maxPipelineRayRecursionDepth = 2;
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
    missShaderBindingTable.CreateBuffer(engine, HandleSize * 2, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + AlignedHandleSize);
    hitShaderBindingTable.CreateBuffer(engine, HandleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, shaderHandleStorage.data() + AlignedHandleSize * 3);
}
void RayTraceRenderer::createDescriptorSets(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {
    { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
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
    AccelerationDescriptorStructure.pAccelerationStructures = &topLevelAS.handle;

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


    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    for (int x = 0; x < 3; x++)
    {
        VkDescriptorBufferInfo VertexBufferInfo = {};
        VertexBufferInfo.buffer = vertexBufferList[x].Buffer;
        VertexBufferInfo.offset = 0;
        VertexBufferInfo.range = vertexBufferList[x].BufferSize;
        VertexBufferInfoList.emplace_back(VertexBufferInfo);

        VkDescriptorBufferInfo IndexBufferInfo = {};
        IndexBufferInfo.buffer = indexBufferList[x].Buffer;
        IndexBufferInfo.offset = 0;
        IndexBufferInfo.range = indexBufferList[x].BufferSize;
        IndexBufferInfoList.emplace_back(IndexBufferInfo);
    }

    VkWriteDescriptorSet VertexDescriptorSet{};
    VertexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    VertexDescriptorSet.dstSet = RTDescriptorSet;
    VertexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VertexDescriptorSet.dstBinding = 3;
    VertexDescriptorSet.pBufferInfo = VertexBufferInfoList.data();
    VertexDescriptorSet.descriptorCount = 3;

    VkWriteDescriptorSet IndexDescriptorSet{};
    IndexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    IndexDescriptorSet.dstSet = RTDescriptorSet;
    IndexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    IndexDescriptorSet.dstBinding = 4;
    IndexDescriptorSet.pBufferInfo = IndexBufferInfoList.data();
    IndexDescriptorSet.descriptorCount = 3;

    VkDescriptorImageInfo DescriptorImage = {};
    DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    DescriptorImage.imageView = texture2D.GetTextureView();
    DescriptorImage.sampler = texture2D.GetTextureSampler();

    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = RTDescriptorSet;
    TextureDescriptor.dstBinding = 5;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    TextureDescriptor.descriptorCount = 1;
    TextureDescriptor.pImageInfo = &DescriptorImage;


    VkDescriptorImageInfo NormalMapImage = {};
    NormalMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    NormalMapImage.imageView = NormalMap.GetTextureView();
    NormalMapImage.sampler = NormalMap.GetTextureSampler();

    VkWriteDescriptorSet NormalMapTextureDescriptor = {};
    NormalMapTextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    NormalMapTextureDescriptor.dstSet = RTDescriptorSet;
    NormalMapTextureDescriptor.dstBinding = 6;
    NormalMapTextureDescriptor.dstArrayElement = 0;
    NormalMapTextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    NormalMapTextureDescriptor.descriptorCount = 1;
    NormalMapTextureDescriptor.pImageInfo = &NormalMapImage;

    VkDescriptorImageInfo CubeMapImage = {};
    CubeMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    CubeMapImage.imageView = NormalMap.GetTextureView();
    CubeMapImage.sampler = NormalMap.GetTextureSampler();

    VkWriteDescriptorSet CubeMapTextureDescriptor = {};
    CubeMapTextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    CubeMapTextureDescriptor.dstSet = RTDescriptorSet;
    CubeMapTextureDescriptor.dstBinding = 7;
    CubeMapTextureDescriptor.dstArrayElement = 0;
    CubeMapTextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    CubeMapTextureDescriptor.descriptorCount = 1;
    CubeMapTextureDescriptor.pImageInfo = &CubeMapImage;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        AccelerationDesciptorSet,
        ImageDescriptorSet,
        UniformDescriptorSet,
        VertexDescriptorSet,
        IndexDescriptorSet,
        TextureDescriptor,
        NormalMapTextureDescriptor,
        CubeMapTextureDescriptor
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

std::vector<Vertex> RayTraceRenderer::CalcVertex()
{
    glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 pos4(1.0f, 1.0f, 0.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);

    return {
        //    // positions            // normal         // texcoords  // tangent                          // bitangent
        //    {{pos1.x, pos1.y, pos1.z}, {nm.x, nm.y, nm.z}, {uv1.x, uv1.y}, {tangent1.x, tangent1.y, tangent1.z}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {tangent1.x, tangent1.y, tangent1.z, 0.0f}},
        //    {{pos2.x, pos2.y, pos2.z}, {nm.x, nm.y, nm.z}, {uv2.x, uv2.y}, {tangent1.x, tangent1.y, tangent1.z}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {tangent1.x, tangent1.y, tangent1.z, 0.0f}},
        //    {{pos3.x, pos3.y, pos3.z}, {nm.x, nm.y, nm.z}, {uv3.x, uv3.y, {tangent1.x, tangent1.y, tangent1.z}}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {tangent1.x, tangent1.y, tangent1.z, 0.0f}},

        //    {{pos1.x, pos1.y, pos1.z}, {nm.x, nm.y, nm.z}, {uv1.x, uv1.y}, {tangent2.x, tangent2.y, tangent2.z}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {tangent2.x, tangent2.y, tangent2.z, 0.0f}},
        //    {{pos3.x, pos3.y, pos3.z}, {nm.x, nm.y, nm.z}, {uv3.x, uv3.y}, {tangent2.x, tangent2.y, tangent2.z}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {tangent2.x, tangent2.y, tangent2.z, 0.0f}},
        //    {{pos4.x, pos4.y, pos4.z}, {nm.x, nm.y, nm.z}, {uv4.x, uv4.y}, {tangent2.x, tangent2.y, tangent2.z}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {tangent2.x, tangent2.y, tangent2.z, 0.0f}}
    };
}

void RayTraceRenderer::createAccelerationStructure(VulkanEngine& engine, AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo)
{
    // Buffer and memory
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
    // Acceleration structure
    VkAccelerationStructureCreateInfoKHR accelerationStructureCreate_info{};
    accelerationStructureCreate_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    accelerationStructureCreate_info.buffer = accelerationStructure.buffer;
    accelerationStructureCreate_info.size = buildSizeInfo.accelerationStructureSize;
    accelerationStructureCreate_info.type = type;
    vkCreateAccelerationStructureKHR(engine.Device, &accelerationStructureCreate_info, nullptr, &accelerationStructure.handle);
    // AS device address
    VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
    accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    accelerationDeviceAddressInfo.accelerationStructure = accelerationStructure.handle;
    accelerationStructure.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &accelerationDeviceAddressInfo);
}

void RayTraceRenderer::AcclerationCommandBuffer(VulkanEngine& engine, VkAccelerationStructureBuildGeometryInfoKHR& VkAccelerationStructureBuildGeometryInfoKHR, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& accelerationStructureBuildRangeInfoKHR)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = engine.GetRenderCommandPool();
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(engine.Device, &commandBufferAllocateInfo, &cmdBuffer);

    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    auto asdf = accelerationStructureBuildRangeInfoKHR.data();
    vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &VkAccelerationStructureBuildGeometryInfoKHR, &asdf);
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

}

VkTransformMatrixKHR RayTraceRenderer::GLMToVkTransformMatrix(glm::mat4 matrix)
{
    return VkTransformMatrixKHR
    {
        matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
        matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
        matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
    };
}