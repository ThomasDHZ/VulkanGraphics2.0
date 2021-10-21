#include "PointLight.h"
#include "AssetManager.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(PointLightBuffer light) : Light<PointLightBuffer>(EnginePtr::GetEnginePtr())
{
	DebugMesh = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
	DebugMesh->AddChildModel(std::make_shared<Model>(Model("../Models/PointLight.obj", MeshDrawFlags::Mesh_Draw_Debug)));
	AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(DebugMesh);
   DebugMesh->MeshList[0]->MeshPosition = LightBuffer.UniformDataInfo.position;

	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<OrthographicLightViewCamera>(OrthographicLightViewCamera(LightBuffer.UniformDataInfo.position));
	Update();
}

PointLight::~PointLight()
{
}

void PointLight::Update()
{
	Light::Update();
	DebugMesh->MeshList[0]->MeshPosition = LightBuffer.UniformDataInfo.position;
	lightViewCamera->Update(LightBuffer.UniformDataInfo.position);
}

void PointLight::Destroy()
{
	Light::Destroy();
}