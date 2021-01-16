#pragma once
#include <cstdint>
#include "VulkanBuffer.h"
#include "Buffer.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct RTVertex {
    float pos[3];
};

struct MeshContainer
{
    std::vector<RTVertex> vertices;
    std::vector<uint32_t> indices;
};

struct AccelerationStructure {
    VkAccelerationStructureKHR handle;
    uint64_t deviceAddress = 0;
    VkDeviceMemory memory;
    VkBuffer buffer;
};

class RayTraceMesh
{
private:
    PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
    PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;

public:
    VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress{};

    AccelerationStructure bottomLevelAS{};
    uint32_t TLASID = -1;
    uint64_t DeviceAddress = VK_NULL_HANDLE;
    VkAccelerationStructureKHR TLAS = VK_NULL_HANDLE;

    VulkanBuffer vertexBuffer;
    VulkanBuffer indexBuffer;
    VulkanBuffer transformBuffer;
    uint64_t BLASAddress;

    glm::mat4 MeshTransformMatrix = glm::mat4(1.0f);
    glm::vec3 MeshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 MeshRotate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 MeshScale = glm::vec3(1.0f);

    RayTraceMesh();
    RayTraceMesh(VkDevice& device, VkPhysicalDevice& VkPhysicalDevice, MeshContainer meshContainer, glm::mat4 TransformMatrix);
    ~RayTraceMesh();

    void Update();

    uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);
};

