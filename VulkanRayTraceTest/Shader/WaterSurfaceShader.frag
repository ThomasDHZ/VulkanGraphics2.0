#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} ConstMesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

layout(binding = 2) uniform UniformBufferObject {
	DirectionalLight dlight;
	PointLight plight[5];
	SpotLight sLight;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
	float timer;
    int Shadowed;
    int temp;
} scenedata;

layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
    vec2 UVFlip;
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];
layout(binding = 4) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 5) buffer Indices { uint i[]; } indices[];
layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D ReflectMap;
layout(binding = 9) uniform sampler2D RefractMap;
layout(binding = 10) uniform sampler2D DUDVMap;
layout(binding = 11) uniform sampler2D NormalMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec4 ClipSpace;
layout(location = 3) in vec3 Normal;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 outColor;

void main() 
{
    const MaterialInfo material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
	const float tile = 6.0f;
	const float waveStrength = 0.02f;
	const float waveSpeed = 0.03f;

	float waveMovement = 0.0f;
	waveMovement += waveSpeed * scenedata.timer;

	vec2 uv = ((ClipSpace.xy/ClipSpace.w)/2.0f)+0.5f;
	vec2 reflectUV = vec2(uv.x, -uv.y);
	vec2 refractUV = vec2(uv);

	vec2 distortion = (texture(DUDVMap, vec2(TexCoords.x + waveMovement, TexCoords.y)).rg * 2.0f - 1.0f) * waveStrength; 
	vec2 distortion2 = (texture(DUDVMap, vec2(TexCoords.x + waveMovement, TexCoords.y + waveMovement)).rg * 2.0f - 1.0f) * waveStrength; 
	vec2 totalDistorion = distortion + distortion2;

	reflectUV += totalDistorion;
	reflectUV.x = clamp(reflectUV.x, 0.001f, 0.999f);
	reflectUV.y = clamp(reflectUV.y, -0.001f, -0.999f);

	refractUV += distortion;
	refractUV = clamp(refractUV, 0.001f, 0.999f);
	
	vec4 reflectColor = texture(ReflectMap, reflectUV);
	vec4 refractColor = texture(RefractMap, refractUV);

	vec3 finalColor = mix(reflectColor, refractColor, 0.5f).rgb;
	finalColor = mix(finalColor, vec3(0.0f, 0.3f, 0.6f), 0.3f);

    outColor = vec4(finalColor, 1.0);
}
