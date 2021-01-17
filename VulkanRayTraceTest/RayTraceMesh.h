#pragma once
#include <cstdint>
#include "VulkanBuffer.h"
#include "Buffer.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct RTVertex 
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Normal = glm::vec3(0.0f);
    glm::vec2 TexureCoord = glm::vec2(0.0f);
    glm::vec4 Tangant = glm::vec4(0.0f);
    glm::vec4 BiTangant = glm::vec4(0.0f);
    glm::vec4 Color = glm::vec4(0.0f);
    glm::ivec4 BoneID = glm::ivec4(0);
    glm::vec4 BoneWeights = glm::vec4(0.0f);
};

struct MeshDetails
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

public:
    VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{};
    VkDeviceOrHostAddressConstKHR transformBufferDeviceAddress{};

     uint32_t TriangleCount;
     uint32_t VertexCount;

    AccelerationStructure bottomLevelAS{};
    AccelerationStructure topLevelAS{};

    VulkanBuffer vertexBuffer;
    VulkanBuffer indexBuffer;
    VulkanBuffer transformBuffer;

    glm::mat4 MeshTransformMatrix = glm::mat4(1.0f);
    glm::vec3 MeshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 MeshRotate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 MeshScale = glm::vec3(1.0f);

    RayTraceMesh();
    RayTraceMesh(VkDevice& device, VkPhysicalDevice& VkPhysicalDevice, MeshDetails meshContainer, glm::mat4 TransformMatrix);
    ~RayTraceMesh();

    void Update();

    uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);
};

