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

layout(binding = 0) uniform UniformBufferObject {
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

layout(binding = 1) buffer MeshProperties 
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

layout(binding = 2) buffer DirectionalLight2
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DLight[];

layout(binding = 3) buffer PointLight2
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
} PLight[];

layout(binding = 4) buffer SpotLight2
{ 
   vec3 position;
   vec3 direction;
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float cutOff;
   float outerCutOff;
   float constant;
   float linear;
   float quadratic;
} SLight[];

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap;
layout(binding = 10) uniform samplerCube irradianceMap;
layout(binding = 11) uniform samplerCube prefilterMap;
layout(binding = 12) uniform sampler2D brdfLUT;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec3 Normal;
layout(location = 4) in vec4 Tangent;
layout(location = 5) in vec4 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

const float PI = 3.14159265359;

#include "PBRFunctions.glsl"
vec3 getNormalFromMap(PBRMaterial material, vec2 uv);

void main()
{		
   vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;
   PBRMaterial material = PBRMaterialBuilder(MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material, TexCoords);
   if(material.Alpha == 0.0f)
   {
	 discard;
   }

   vec3 N = Normal;
   if(MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material.NormalMapID != 0)
   {
      N = getNormalFromMap(material, texCoords);
   }
   vec3 V = normalize(scenedata.viewPos - FragPos);
   vec3 R = reflect(-V, N); 

   vec3 F0 = vec3(0.04); 
   F0 = mix(F0, material.Albedo, material.Metallic);

    vec3 Lo = vec3(0.0);
    Lo += CalcDirectionalLight(material, F0, V, N);
    Lo += CalcPointLight(material, FragPos, F0, V, N);
    Lo += CalcSpotLight(material, FragPos, F0, V, N);

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, material.Roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - material.Metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * material.Albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  material.Roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), material.Roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * material.AmbientOcclusion;
   vec3 color = ambient + Lo;

   outColor = vec4(color, material.Alpha);
}

vec3 getNormalFromMap(PBRMaterial material, vec2 uv)
{
    vec3 T = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * vec3(Tangent));
    vec3 B = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * vec3(BiTangent));
    vec3 N = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * Normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 normal = material.Normal;
         normal = normalize(normal * 2.0 - 1.0);
    
    return TBN * normal;
}