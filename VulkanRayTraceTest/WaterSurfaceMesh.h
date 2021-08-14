#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "WaterSurfacePipeline.h"
#include "WaterRenderPass.h"

class WaterSurfaceMesh : public Mesh
{
	WaterRenderToTextureRenderPass waterReflectionRenderPass;
	WaterRenderToTextureRenderPass waterRefractionRenderPass;

	std::shared_ptr<WaterSurfacePipeline> waterSurfacePipeline;

public:
	WaterSurfaceMesh();
	WaterSurfaceMesh(VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer);
	~WaterSurfaceMesh();

	void Update(SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera);
	void RebuildSwapChain(VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void DrawWaterTexture(uint32_t imageIndex, Skybox skybox);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, int a);
	void SubmitToCMDBuffer(std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex);
	void Destory() override;
};
