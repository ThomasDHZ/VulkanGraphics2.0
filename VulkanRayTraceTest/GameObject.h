#pragma once
#include "Object.h"
class GameObject : public Object
{
private:
	std::vector<std::shared_ptr<Model>> ModelList;
protected:

public:
	GameObject();
	GameObject(VulkanEngine& engine, glm::vec3 position);
	~GameObject();

	void AddChildModel(std::shared_ptr<Model> model);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager);
	virtual void Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceModeFlag);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID);

};