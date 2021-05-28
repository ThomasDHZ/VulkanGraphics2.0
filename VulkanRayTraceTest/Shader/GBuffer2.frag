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
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];
layout(binding = 4) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 5) buffer Indices { uint i[]; } indices[];
layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D TextureMap[];
layout(binding = 9) uniform sampler3D Texture3DMap[];
layout(binding = 10) uniform samplerCube CubeMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec3 Normal;
layout(location = 4) in vec4 Tangent;
layout(location = 5) in vec4 BiTangent;

layout(location = 0) out vec4 GPosition;
layout(location = 1) out vec4 GAlebdo;
layout(location = 2) out vec4 GNormal;
layout(location = 3) out vec4 GTangent;
layout(location = 4) out vec4 GBiTangent;
layout(location = 5) out vec4 GBloom;
layout(location = 6) out vec4 NormalMap;
layout(location = 7) out vec4 SpecularMap;

vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

void main() 
{

   const MaterialInfo material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
   vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;

   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
   {
	 discard;
   }
    vec3 T = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * vec3(Tangent));
    vec3 B = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * vec3(BiTangent));
    vec3 N = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * Normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 ViewPos  = ConstMesh.CameraPos;
    vec3 FragPos2  = FragPos;
    if(material.NormalMapID != 0)
    {
        ViewPos  = TBN * ConstMesh.CameraPos;
        FragPos2  = TBN * FragPos;
    }
    vec3 I = normalize(FragPos2 - ViewPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 Reflection = texture(CubeMap, R).rgb;

	GPosition = vec4(FragPos, 1.0f);
	if(material.DiffuseMapID != 0)
    {
        GAlebdo.rgb = texture(TextureMap[material.DiffuseMapID], texCoords).rgb;
        GAlebdo.a = material.Shininess;
    }
    else
    {
        GAlebdo = vec4(material.Diffuse, 1.0f);
        GAlebdo.a = material.Shininess;
    }
	GNormal = vec4(Normal, material.Reflectivness);
    GTangent = vec4(Tangent.rgb, 1.0f);
    GBiTangent = vec4(BiTangent.rgb, 1.0f);
	GBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);


        NormalMap = vec4(Reflection, material.Reflectivness);


    SpecularMap = vec4(material.Specular, 1.0f);
    if (material.SpecularMapID != 0)
    {
        SpecularMap =  vec4(texture(TextureMap[material.SpecularMapID], texCoords).rgb, 1.0f);
    }
}