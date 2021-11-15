#include "SceneManager.h"
std::shared_ptr<SceneManager> SceneManagerPtr::sceneManagerPtr = nullptr;

SceneManager::SceneManager()
{
}

SceneManager::SceneManager(std::shared_ptr<VulkanEngine> engine)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::LoadScene()
{
	LightManagerPtr::GetLightManagerPtr()->ClearLights();
	CameraManagerPtr::GetCameraManagerPtr()->ClearCameras();
	MaterialManagerPtr::GetMaterialManagerPtr()->ClearMaterials();
	TextureManagerPtr::GetTextureManagerPtr()->ClearTextures();
	MeshManagerPtr::GetMeshManagerPtr()->ClearMeshs();
	ObjManagerPtr::GetObjManagerPtr()->ClearObjects();
}
