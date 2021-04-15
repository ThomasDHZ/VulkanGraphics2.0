#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"

struct SkyboxUniformBuffer 
{
	alignas(16) glm::mat4 viewInverse;
	alignas(16) glm::mat4 projInverse;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 viewPos;
};

class Skybox : public Mesh
{
private:

	UniformData<SkyboxUniformBuffer> SkyUniformBuffer;

	VkDescriptorSetLayout DescriptorLayout;
	VkDescriptorPool DescriptorPool;
	VkDescriptorSet DescriptorSets;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;

	void CreateDescriptorSetLayout(VulkanEngine& engine, AssetManager& assetManager);
	void CreateShaderPipeLine(VulkanEngine& engine, VkRenderPass& RenderPass);
	void CreateDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void CreateDescriptorSets(VulkanEngine& engine, AssetManager& assetManager);

public:
	Skybox();
	Skybox(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass);
	~Skybox();

	void Update(VulkanEngine& engine, MaterialManager& materialManager, std::shared_ptr<PerspectiveCamera> camera);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo) override;
	void Destory(VulkanEngine& engine) override;
};

