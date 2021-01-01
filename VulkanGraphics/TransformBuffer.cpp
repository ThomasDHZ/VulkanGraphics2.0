#include "TransformBuffer.h"
#include "VulkanBufferManager.h"

TransformBuffer::TransformBuffer()
{
}

TransformBuffer::TransformBuffer(VulkanEngine& engine, const VkTransformMatrixKHR& TransformData)
{
    VkDeviceSize bufferSize = sizeof(VkTransformMatrixKHR);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkanBufferManager::CreateBuffer(engine, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(engine.Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, &TransformData,(size_t)bufferSize);
    vkUnmapMemory(engine.Device, stagingBufferMemory);

    VulkanBufferManager::CreateBuffer(engine, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, transformBuffer, TransformBufferMemory);
    VulkanBufferManager::CopyBuffer(engine, stagingBuffer, transformBuffer, bufferSize);

    vkDestroyBuffer(engine.Device, stagingBuffer, nullptr);
    vkFreeMemory(engine.Device, stagingBufferMemory, nullptr);
}

TransformBuffer::~TransformBuffer()
{
}

void TransformBuffer::Destory(VulkanEngine& engine)
{
    vkDestroyBuffer(engine.Device, transformBuffer, nullptr);
    vkFreeMemory(engine.Device, TransformBufferMemory, nullptr);

    transformBuffer = VK_NULL_HANDLE;
    TransformBufferMemory = VK_NULL_HANDLE;
}
