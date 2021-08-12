#include "RayTracingGraphicsPipeline.h"

RayTracingGraphicsPipeline::RayTracingGraphicsPipeline() : GraphicsPipeline()
{
}

RayTracingGraphicsPipeline::RayTracingGraphicsPipeline(std::shared_ptr<VulkanEngine> engine) : GraphicsPipeline()
{
    rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    VkPhysicalDeviceProperties2 deviceProperties2{};
    deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProperties2.pNext = &rayTracingPipelineProperties;
    vkGetPhysicalDeviceProperties2(engine->PhysicalDevice, &deviceProperties2);

    accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &accelerationStructureFeatures;
    vkGetPhysicalDeviceFeatures2(engine->PhysicalDevice, &deviceFeatures2);
}

RayTracingGraphicsPipeline::~RayTracingGraphicsPipeline()
{
}

void RayTracingGraphicsPipeline::UpdateGraphicsPipeLine()
{
    vkDestroyPipeline(VulkanPtr::GetDevice(), ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(VulkanPtr::GetDevice(), ShaderPipelineLayout, nullptr);
    vkDestroyDescriptorPool(VulkanPtr::GetDevice(), DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(VulkanPtr::GetDevice(), DescriptorSetLayout, nullptr);

    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
}

void RayTracingGraphicsPipeline::Destroy()
{
	GraphicsPipeline::Destroy();
	raygenShaderBindingTable.DestoryBuffer();
	missShaderBindingTable.DestoryBuffer();
	hitShaderBindingTable.DestoryBuffer();
}
