#pragma once
#include <memory>
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndicesBuffer.h"
class DrawMesh
{
private:
protected:
    VertexBuffer MeshVertex;
    IndicesBuffer MeshIndices;

    VkDescriptorPool DescriptorPool;
    std::vector<VkDescriptorSet> DescriptorSets;
public:
	 void Draw(VkCommandBuffer& RenderCommandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int FrameNumber);
};

