#pragma once
#include <memory>
#include "VulkanEngine.h"
#include "AssetManager.h"

struct GlobalPtr
{
	static std::shared_ptr<VulkanWindow> windowPtr;
	static std::shared_ptr<VulkanEngine> enginePtr;
	static std::shared_ptr<AssetManager> assetManagerPtr;
	static std::shared_ptr<InputManager> inputManagerPtr;
	static std::shared_ptr<CameraManager> cameraManagerPtr;
	static std::shared_ptr<MaterialManager> materialManagerPtr;
	static std::shared_ptr<TextureManager> textureManagerPtr;
	static std::shared_ptr<MeshManager> meshManagerPtr;
	static std::shared_ptr<LightManager> lightManagerPtr;
	static std::shared_ptr<GUIManager> guiManagerPtr;
	static std::shared_ptr<ObjectManager> ObjManagerPtr;

	static void SetUpPtrs(unsigned int width, unsigned int height, const char* WindowName)
	{
		windowPtr = std::make_shared<VulkanWindow>(VulkanWindow(width, height, WindowName));
		enginePtr = std::make_shared<VulkanEngine>(VulkanEngine(windowPtr));
		assetManagerPtr = std::make_shared<AssetManager>(AssetManager(enginePtr, windowPtr));
		cameraManagerPtr = assetManagerPtr->cameraManager;
		textureManagerPtr = assetManagerPtr->textureManager;
		materialManagerPtr = assetManagerPtr->materialManager;
		inputManagerPtr = assetManagerPtr->inputManager;
		meshManagerPtr = assetManagerPtr->meshManager;
		lightManagerPtr = assetManagerPtr->lightManager;
		guiManagerPtr = assetManagerPtr->guiManager;
		ObjManagerPtr = assetManagerPtr->ObjManager;
	}
};