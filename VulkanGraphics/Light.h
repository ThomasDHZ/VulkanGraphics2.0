#pragma once
#include "Vulkanengine.h"
#include "CameraManager.h"
#include "SceneData.h"
#include "ObjectViewCamera.h"

template <class T>
class Light
{
private:
protected:


public:
	UniformData<T> LightBuffer;
	Light()
	{

	}
	
	Light(std::shared_ptr<VulkanEngine> engine)
	{
		LightBuffer = UniformData<T>(engine);
		Update();
	}

	~Light()
	{

	}

	virtual void Update()
	{
		LightBuffer.Update();
	}

	virtual void Destroy()
	{
		LightBuffer.Destroy();
	}

	VkBuffer GetLightBuffer() { return LightBuffer.VulkanBufferData.Buffer; }
};
