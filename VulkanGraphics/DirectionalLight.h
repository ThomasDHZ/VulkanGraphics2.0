#pragma once
#include "DebugLightMesh.h"
class DirectionalLight
{
	DirectionalLightStruct DirectionLight;
	std::shared_ptr<PerspectiveCamera> LightCamera;
	std::shared_ptr<DebugLightMesh> DebugMesh;
};

