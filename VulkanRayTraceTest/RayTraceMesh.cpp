#include "RayTraceMesh.h"

RayTraceMesh::RayTraceMesh()
{
}

RayTraceMesh::RayTraceMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshContainer meshContainer, glm::mat4 TransformMatrix)
{
    vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));
    vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));

    vertexBuffer.CreateBuffer(device, physicalDevice,
        meshContainer.vertices.size() * sizeof(RTVertex),
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshContainer.vertices.data());

    indexBuffer.CreateBuffer(device, physicalDevice,
        meshContainer.indices.size() * sizeof(uint32_t),
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshContainer.indices.data());

    TransformMatrix = glm::transpose(TransformMatrix);
    transformBuffer.CreateBuffer(device, physicalDevice,
        sizeof(TransformMatrix),
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &TransformMatrix);

    vertexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, vertexBuffer.Buffer);
    indexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, indexBuffer.Buffer);
    transformBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, transformBuffer.Buffer);
}

RayTraceMesh::~RayTraceMesh()
{
}
void RayTraceMesh::Update()
{
}
uint64_t RayTraceMesh::getBufferDeviceAddress(VkDevice& device, VkBuffer buffer)
{
    VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
        bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferDeviceAI.buffer = buffer;
        return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
}
