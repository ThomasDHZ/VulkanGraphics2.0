#include "RayTracingGraphicsPipeline.h"

RayTracingGraphicsPipeline::RayTracingGraphicsPipeline() : GraphicsPipeline()
{
}

RayTracingGraphicsPipeline::RayTracingGraphicsPipeline(VulkanEngine& engine) : GraphicsPipeline()
{
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
}

RayTracingGraphicsPipeline::~RayTracingGraphicsPipeline()
{
}

void RayTracingGraphicsPipeline::UpdateGraphicsPipeLine(VulkanEngine& engine)
{
    vkDestroyPipeline(engine.Device, ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(engine.Device, ShaderPipelineLayout, nullptr);
    vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, DescriptorSetLayout, nullptr);

    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
}

void RayTracingGraphicsPipeline::Destroy(VulkanEngine& engine)
{
	GraphicsPipeline::Destroy(engine);
	raygenShaderBindingTable.DestoryBuffer(engine.Device);
	missShaderBindingTable.DestoryBuffer(engine.Device);
	hitShaderBindingTable.DestoryBuffer(engine.Device);
}
