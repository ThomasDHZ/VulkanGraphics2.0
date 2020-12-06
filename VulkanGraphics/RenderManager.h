#pragma once
#include "VulkanEngine.h"
#include "MainRenderPass.h"
#include "InterfaceRenderPass.h"
#include "Mesh.h"
#include "Model.h"
#include "SkyBoxMesh.h"
#include "SceneRenderPass.h"
#include "ShadowRenderPass.h"
#include "FrameBufferRenderPass.h"
#include "FrameBufferMesh.h"
#include "GBufferRenderPass.h"
#include "LightManager.h"
#include "DeferredFrameBufferMesh.h"

class RenderManager
{
private:
	size_t currentFrame = 0;
	bool framebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<RenderManager*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	FrameBufferMesh frameBuffer;
	DeferredFrameBufferMesh SSAOFrameBuffer;
	//FrameBufferMesh SSAOBlurframeBuffer;

	void MainRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex, LightManager& lightmanager);
	void SceneRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex, LightManager& lightmanager);
	void GBufferRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex);
	void SSAORenderCMDBuffer(VulkanEngine& engine, std::shared_ptr<PerspectiveCamera> camera, int SwapBufferImageIndex);
	void FrameBufferRenderCMDBuffer(VulkanEngine& engine, int SwapBufferImageIndex);
	void ShadowRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, int SwapBufferImageIndex);

public:
	std::vector<VkCommandBuffer> commandBuffers;

	MainRenderPass mainRenderPass;
	SceneRenderPass sceneRenderPass;
	GBufferRenderPass gBufferRenderPass;
	FrameBufferRenderPass frameBufferRenderPass;
	ShadowRenderPass shadowRenderPass;
	InterfaceRenderPass interfaceRenderPass;

	RenderManager();
	RenderManager(VulkanEngine& vEngine, GLFWwindow* window);
	~RenderManager();

	void CMDBuffer(VulkanEngine& engine, std::shared_ptr<PerspectiveCamera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager);
	void UpdateCommandBuffer(VulkanEngine& engine, std::shared_ptr<PerspectiveCamera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager);
	void UpdateRenderManager(VulkanEngine& engine, GLFWwindow* window, std::shared_ptr<PerspectiveCamera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager);
	void Draw(VulkanEngine& engine, GLFWwindow* window, std::shared_ptr<PerspectiveCamera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager);
	void Destroy(VulkanEngine& engine);
};

