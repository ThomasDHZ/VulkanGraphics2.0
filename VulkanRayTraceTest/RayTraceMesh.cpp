#include "RayTraceMesh.h"

RayTraceMesh::RayTraceMesh()
{
}

RayTraceMesh::RayTraceMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, std::vector<RTVertex> vertices, std::vector<uint32_t> indices, glm::mat4 TransformMatrix)
{
    vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));

    vertexBuffer.createBuffer(device, physicalDevice,
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &vertexBuffer,
        vertices.size() * sizeof(RTVertex),
        vertices.data());

    indexBuffer.createBuffer(device, physicalDevice,
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &indexBuffer,
        indices.size() * sizeof(uint32_t),
        indices.data());

    TransformMatrix = glm::transpose(TransformMatrix);
    transformBuffer.createBuffer(device, physicalDevice,
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &transformBuffer,
        sizeof(TransformMatrix),
        &TransformMatrix);

    vertexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, vertexBuffer.buffer);
    indexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, indexBuffer.buffer);
    transformBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, transformBuffer.buffer);
}

RayTraceMesh::~RayTraceMesh()
{
}
uint64_t RayTraceMesh::getBufferDeviceAddress(VkDevice& device, VkBuffer buffer)
{
    VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
        bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAI.buffer = buffer;
        return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
}
