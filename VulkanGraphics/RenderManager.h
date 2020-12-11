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
#include "Object2D.h"
#include "SceneToTextureRendererPass.h"
#include "BloomRenderPass.h"

class RenderManager
{
private:
	size_t currentFrame = 0;
	bool framebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<RenderManager*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	std::vector<BaseMesh> MeshDrawList;

	FrameBufferMesh frameBuffer;
	DeferredFrameBufferMesh SSAOFrameBuffer;
	//FrameBufferMesh SSAOBlurframeBuffer;

	void MainRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void SceneRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void GBufferRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex);
	void SSAORenderCMDBuffer(VulkanEngine& engine, std::shared_ptr<Camera> camera, int SwapBufferImageIndex);
	void FrameBufferRenderCMDBuffer(VulkanEngine& engine, int SwapBufferImageIndex);
	void TextureRenderCMDBuffer(VulkanEngine& engine, int SwapBufferImageIndex, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void ShadowRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, int SwapBufferImageIndex);

public:
	std::vector<VkCommandBuffer> commandBuffers;

	MainRenderPass mainRenderPass;
	SceneRenderPass sceneRenderPass;
	GBufferRenderPass gBufferRenderPass;
	FrameBufferRenderPass frameBufferRenderPass;
	ShadowRenderPass shadowRenderPass;
	SceneToTextureRendererPass textureRenderPass;
	BloomRenderPass bloomRenderPass;
	InterfaceRenderPass interfaceRenderPass;

	RenderManager();
	RenderManager(VulkanEngine& vEngine, std::shared_ptr<TextureManager> textureManager, GLFWwindow* window);
	~RenderManager();

	void FrameDebug();
	void CMDBuffer(VulkanEngine& engine, std::shared_ptr<Camera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void RendererUpdate(VulkanEngine& engine, std::shared_ptr<Camera> camera);
	void UpdateCommandBuffer(VulkanEngine& engine, std::shared_ptr<Camera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void ResizeWindowUpdate(VulkanEngine& engine, GLFWwindow* window, std::shared_ptr<Camera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void Draw(VulkanEngine& engine, GLFWwindow* window, std::shared_ptr<PerspectiveCamera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList);
	void Destroy(VulkanEngine& engine);
};

