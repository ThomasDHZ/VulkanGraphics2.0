#version 460
#extension GL_EXT_ray_tracing : enable

layout(push_constant) uniform RayTraceCamera
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
    uint frame;
} Camera;

struct RayHitInfo
{
	vec3 ShadowMask;
	vec3 ReflectionColor;
	vec3 SSAOColor;
	vec3 SkyboxColor;
	uint reflectCount;
};

layout(location = 0) rayPayloadInEXT RayHitInfo rayPayload;
layout(binding = 13, set = 0) uniform samplerCube CubeMap;
void main()
{
	rayPayload.SkyboxColor = texture(CubeMap, gl_WorldRayDirectionEXT).rgb;
	rayPayload.reflectCount = 0;
}
