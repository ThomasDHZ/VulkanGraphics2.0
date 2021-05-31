#version 460
#extension GL_EXT_ray_tracing : enable

layout(push_constant) uniform RayTraceCamera
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Camera;

struct RayHitInfo
{
	vec3 color;
	float distance;
	vec3 normal;
	float reflector;
};

layout(location = 0) rayPayloadInEXT RayHitInfo rayPayload;
layout(binding = 13, set = 0) uniform samplerCube CubeMap;
void main()
{
	rayPayload.color = texture(CubeMap, gl_WorldRayDirectionEXT).rgb;
	rayPayload.distance = -1.0f;
	rayPayload.normal = vec3(0.0f);
	rayPayload.reflector = 0.0f;
}