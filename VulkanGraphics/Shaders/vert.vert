#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "material.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 proj;
    mat4 view;
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    uint SphereAreaLightCount;
    uint TubeAreaLightCount;
    uint RectangleAreaLightCount;
	float timer;
    int Shadowed;
    int temp;
} ubo;

layout(binding = 1) buffer MeshProperties 
{
	mat4 ModelTransform;
	vec2 UVOffset;
    vec2 UVScale;
    vec2 UVFlip;
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer Material { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 Tangent;
layout(location = 4) out vec3 BiTangent;
layout(location = 5) out vec4 LightSpace;

void main() 
{
    FragPos = vec3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(inPosition, 1.0));    
    TexCoords = aTexCoords;
    Normal = aNormal;
	Tangent = aTangent.rgb;
	BiTangent = aBitangent.rgb;
    LightSpace = ubo.proj * ubo.view * vec4(FragPos, 1.0);
    gl_Position = Mesh.proj * Mesh.view * meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(inPosition, 1.0);

}