#pragma once
#include "Light.h"
#include "OrthographicLightViewCamera.h"
#include "Model.h"
#include "MeshManager.h"
#include "PerspectiveLightViewCamera.h"

class PointLight : public Light<PointLightBuffer>
{
private:

public:

	std::shared_ptr<Model> LightMeshDebug;

	glm::vec2 LeftRight = glm::vec2(-3000.0f, 3000.0f);
	glm::vec2 TopBottom = glm::vec2(-3000.0f, 3000.0f);
	glm::vec2 NearFar = glm::vec2(0.1f, 3000.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f);

	PointLight();
	PointLight(PointLightBuffer light);
	~PointLight();

	void Update() override;
	void Destroy() override;

	std::shared_ptr<PerspectiveLightViewCamera> lightViewCamera;
};

