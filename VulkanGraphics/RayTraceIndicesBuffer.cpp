#include "RayTraceIndicesBuffer.h"
#include "IndicesBuffer.h"
#include "VulkanBufferManager.h"

RayTraceIndicesBuffer::RayTraceIndicesBuffer()
{
}

RayTraceIndicesBuffer::RayTraceIndicesBuffer(VulkanEngine& renderer, const std::vector<uint32_t>& indicesdata)
{
    IndiceCount = indicesdata.size();

    VkDeviceSize bufferSize = sizeof(indicesdata[0]) * indicesdata.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkanBufferManager::CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer.Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indicesdata.data(), (size_t)bufferSize);
    vkUnmapMemory(renderer.Device, stagingBufferMemory);

    VulkanBufferManager::CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indiceBuffer, IndiceBufferMemory);

    VulkanBufferManager::CopyBuffer(renderer, stagingBuffer, indiceBuffer, bufferSize);

    vkDestroyBuffer(renderer.Device, stagingBuffer, nullptr);
    vkFreeMemory(renderer.Device, stagingBufferMemory, nullptr);


    //IndiceCount = indicesdata.size();
    //VkDeviceSize bufferSize = sizeof(indicesdata[0]) * IndiceCount;

    //auto b = indicesdata[0];
    //auto a = static_cast<void*>(&b);
    //CreateStagingBuffer(engine, bufferSize);
    //MapMemory(engine, a, bufferSize);
    //CreateBuffer(engine, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //CopyBuffer(engine, StagingBuffer, Buffer, bufferSize);
    //DestoryStagingBuffer(engine);
}

RayTraceIndicesBuffer::~RayTraceIndicesBuffer()
{
}

void RayTraceIndicesBuffer::Destory(VulkanEngine& renderer)
{
    if (IndiceCount != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(renderer.Device, indiceBuffer, nullptr);
        vkFreeMemory(renderer.Device, IndiceBufferMemory, nullptr);

        indiceBuffer = VK_NULL_HANDLE;
        IndiceBufferMemory = VK_NULL_HANDLE;
        IndiceCount = 0;
    }
}
