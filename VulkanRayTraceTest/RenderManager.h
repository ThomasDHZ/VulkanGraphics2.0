#pragma once
class RenderManager
{
private:
	void CreateRasterDescriptorPool();
	void CreateRasterDescriptorSet();
	void CreateRasterCommandBuffer();
	void CreateRayTraceDescriptorPool();
	void CreateRayTraceDescriptorSet();
	void CreateRayTraceCommandBuffer();
public:
};

