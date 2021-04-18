#include "GraphicsPipeline.h"
#include <fstream>

GraphicsPipeline::GraphicsPipeline()
{

}

GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass)
{
    vkDestroyPipeline(renderer.Device, ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(renderer.Device, ShaderPipelineLayout, nullptr);

    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
}

void GraphicsPipeline::Destroy(VulkanEngine& engine)
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