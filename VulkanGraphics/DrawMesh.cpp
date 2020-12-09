#include "DrawMesh.h"

void DrawMesh::Draw(VkCommandBuffer& RenderCommandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int FrameNumber)
{
    VkBuffer vertexBuffers[] = { MeshVertex.GetVertexBuffer() };
    VkDeviceSize offsets[] = { 0 };

    {
        vkCmdBindPipeline(RenderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->ShaderPipeline);
        vkCmdBindVertexBuffers(RenderCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindDescriptorSets(RenderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->ShaderPipelineLayout, 0, 1, &DescriptorSets[FrameNumber], 0, nullptr);
        if (MeshIndices.GetIndiceBuffer() == 0)
        {
            vkCmdDraw(RenderCommandBuffer, MeshVertex.GetVertexCount(), 1, 0, 0);
        }
        else
        {
            vkCmdBindIndexBuffer(RenderCommandBuffer, MeshIndices.GetIndiceBuffer(), 0, VK_INDEX_TYPE_UINT16);
            vkCmdDrawIndexed(RenderCommandBuffer, static_cast<uint32_t>(MeshIndices.GetIndiceCount()), 1, 0, 0, 0);
        }
    }
}
