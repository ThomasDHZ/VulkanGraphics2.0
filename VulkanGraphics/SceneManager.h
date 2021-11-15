#pragma once
#include "VulkanEngine.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "ObjectManager.h"

class SceneManager
{
private:
public:
	SceneManager();
	SceneManager(std::shared_ptr<VulkanEngine> engine);
	~SceneManager();

	void LoadScene();
};

class SceneManagerPtr
{
private:
	static std::shared_ptr<SceneManager> sceneManagerPtr;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		if (sceneManagerPtr == nullptr)
		{
			sceneManagerPtr = std::make_shared<SceneManager>(SceneManager());
		}
		else
		{
			std::cout << "Scene Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<SceneManager> GetSceneManagerPtr()
	{
		return sceneManagerPtr;
	}
};
