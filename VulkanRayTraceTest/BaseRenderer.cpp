#include "BaseRenderer.h"

BaseRenderer::BaseRenderer()
{
}

BaseRenderer::BaseRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr)
{
	assetManager = assetManagerPtr;
}

BaseRenderer::~BaseRenderer()
{
}
