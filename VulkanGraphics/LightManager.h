#pragma once
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include <GLFW\glfw3.h>
#include <memory>
#include "PointLight.h"

class LightManager
{
private:

public:
	std::vector<std::shared_ptr<PointLight>> PointLightList;
	LightBufferObject light;

	LightManager();
	LightManager(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, VkDescriptorSetLayout& layout, int renderBit, glm::vec3 Pos);
	~LightManager();

	void Draw(VkCommandBuffer& RenderCommandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int FrameNumber);
	void Update(VulkanEngine& engine, std::shared_ptr<Camera> camera);
	void Destory(VulkanEngine& engine);
};
