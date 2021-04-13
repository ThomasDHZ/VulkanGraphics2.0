#version 460
#extension GL_EXT_ray_tracing : enable

struct RayHitInfo
{
	vec3 color;
	uint reflectCount;
};

layout(location = 0) rayPayloadInEXT RayHitInfo rayPayload;
layout(binding = 10, set = 0) uniform samplerCube CubeMap;
void main()
{
	rayPayload.color = texture(CubeMap, gl_WorldRayDirectionEXT).rgb;
	rayPayload.reflectCount = 0;
}
