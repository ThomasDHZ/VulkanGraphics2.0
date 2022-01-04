#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(PointLightBuffer light) : Light<PointLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<PerspectiveCamera>(PerspectiveCamera("PointShadowCamera", LightBuffer.UniformDataInfo.position, cameraDirection));
	CameraManagerPtr::GetCameraManagerPtr()->CameraList.emplace_back(lightViewCamera);

	LightMeshDebug = std::make_shared<Model>(Model("../Models/Cube.obj", Renderer_Draw_Debug));
	MeshManagerPtr::GetMeshManagerPtr()->AddMesh(LightMeshDebug->MeshList[0]);

	Update();
}

PointLight::~PointLight()
{
}

void PointLight::Update()
{
	lightViewCamera->Position = LightBuffer.UniformDataInfo.position;
	lightViewCamera->Update(EnginePtr::GetEnginePtr());

	LightMeshDebug->MeshList[0]->MeshPosition = LightBuffer.UniformDataInfo.position;
	LightMeshDebug->Update();

	Light::Update();
}

void PointLight::Destroy()
{
	Light::Destroy();
}