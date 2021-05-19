#pragma once
#include "VulkanEngine.h"
#include "SceneData.h"

template <class T>
class Light
{
private:
protected:
	UniformData<T> LightBuffer;

public:
	Light()
	{

	}
	
	Light(VulkanEngine& engine)
	{
		LightBuffer = UniformData<T>(engine);
		Update(engine);
	}

	~Light()
	{

	}

	void Update(VulkanEngine& engine)
	{
		LightBuffer.Update(engine);
	}

	void Destroy(VulkanEngine& engine)
	{
		LightBuffer.Destroy(engine);
	}
};
