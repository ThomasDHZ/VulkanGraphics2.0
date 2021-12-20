#pragma once
#include "GraphicsPipeline.h"
#include "Mesh.h"

class ComputeAnimationPipeline : public GraphicsPipeline
{
private:
	std::shared_ptr<VulkanBuffer> VertexBufferCopy;
	std::shared_ptr<Mesh> mesh;

	void SetUpDescriptorBindings();
	void CreateShaderPipeLine();

public:
	ComputeAnimationPipeline();
	ComputeAnimationPipeline(std::shared_ptr<Mesh> meshptr);
	~ComputeAnimationPipeline();

	VkCommandBuffer commandBuffer;
	void Compute();
};