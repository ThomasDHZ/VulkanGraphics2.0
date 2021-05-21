#version 460
#extension GL_EXT_ray_tracing : enable

struct RayHitInfo
{
	vec3 color;
	float distance;
	vec3 normal;
	float reflector;
	float materialreflection;
};

layout(location = 0) rayPayloadInEXT RayHitInfo rayPayload;
layout(binding = 13, set = 0) uniform samplerCube CubeMap;
void main()
{
	rayPayload.color = texture(CubeMap, gl_WorldRayDirectionEXT).rgb;
	rayPayload.distance = -1.0f;
	rayPayload.normal = vec3(0.0f);
	rayPayload.reflector = 0.0f;
	rayPayload.materialreflection = 0.0f;
}