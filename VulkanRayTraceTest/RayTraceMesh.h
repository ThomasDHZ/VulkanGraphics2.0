#pragma once
#include <cstdint>
#include "Buffer.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct RTVertex {
    float pos[3];
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

public:
    VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress{};

    AccelerationStructure bottomLevelAS{};

    Buffer vertexBuffer;
    Buffer indexBuffer;
    Buffer transformBuffer;
    uint64_t BLASAddress;

    RayTraceMesh();
    RayTraceMesh(VkDevice& device, VkPhysicalDevice& VkPhysicalDevice, std::vector<RTVertex> vertices, std::vector<uint32_t> indices, glm::mat4 TransformMatrix);
    ~RayTraceMesh();

    uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);
};

