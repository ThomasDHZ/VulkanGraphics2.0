#include "DirectionalLight.h"
#include "AssetManager.h"

DirectionalLight::DirectionalLight() : Light<DirectionalLightBuffer>()
{

}

DirectionalLight::DirectionalLight(DirectionalLightBuffer light) : Light<DirectionalLightBuffer>(EnginePtr::GetEnginePtr())
{
	DebugMesh = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
	DebugMesh->AddChildModel(std::make_shared<Model>(Model("../Models/flashlight.obj", MeshDrawFlags::Mesh_Draw_Debug)));
	AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(DebugMesh);
	DebugMesh->MeshList[0]->MeshPosition = glm::vec3(-LightBuffer.UniformDataInfo.direction * 360.0f);

	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<OrthographicLightViewCamera>(OrthographicLightViewCamera(LightBuffer.UniformDataInfo.direction));
	Update();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update()
{
	Light::Update();
	DebugMesh->MeshList[0]->MeshRotation = glm::vec3(-LightBuffer.UniformDataInfo.direction * 360.0f);
	lightViewCamera->Update(-LightBuffer.UniformDataInfo.direction);
}

void DirectionalLight::Destroy()
{
	Light::Destroy();
}
