#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::BaseRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr)
{
	interfaceRenderPass = InterfaceRenderPass(engine, window.GetWindowPtr());
	assetManager = assetManagerPtr;
}

BaseRenderer::~BaseRenderer()
{
}
