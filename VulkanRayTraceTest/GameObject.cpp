#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(VulkanEngine& engine, glm::vec3 position)
{
}

GameObject::~GameObject()
{
}

void GameObject::AddChildModel(std::shared_ptr<Model> model)
{
}

void GameObject::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, inputManager, materialManager);
	}
}

void GameObject::Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceModeFlag)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, ModelMatrix, BoneList, inputManager, materialManager, RayTraceModeFlag);
	}
}

void GameObject::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, layout, RendererID, CameraView);
	}
}

void GameObject::Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, renderPassInfo, RendererID);
	}
}