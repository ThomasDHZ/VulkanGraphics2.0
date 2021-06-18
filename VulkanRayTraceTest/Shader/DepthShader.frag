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

layout(binding = 0) uniform UniformBufferObject 
{
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
layout(binding = 9) uniform sampler2D ShadowMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec3 Normal;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 outColor;

MaterialInfo material;
#include "BlinePhongLighting.glsl"

void main() 
{
    material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
    const vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;
    
   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
   {
	 discard;
   }
    
    vec3 result = vec3(0.0f);
    vec3 normal = Normal;
    vec3 ViewPos  = ConstMesh.CameraPos;
    vec3 FragPos2  = FragPos;
    if(material.NormalMapID != 0)
    {
        ViewPos  = TBN * ConstMesh.CameraPos;
        FragPos2  = TBN * FragPos;
    }
    const vec3 viewDir = normalize(ViewPos - FragPos2);

    if(material.NormalMapID != 0)
    {
//        if(material.DepthMapID != 0)
//        {
//            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
//            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//            {
//              discard;
//            }
//        }
        normal = texture(TextureMap[material.NormalMapID], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
     }
     for(int x = 0; x < scenedata.DirectionalLightCount; x++)
     {
        result += CalcNormalDirLight(FragPos2, normal, texCoords, x);
     }
     for(int x = 0; x < scenedata.PointLightCount; x++)
     {
        result += CalcNormalPointLight(FragPos2, normal, texCoords, x);   
     }
     //result +=  CalcNormalSpotLight(FragPos, scenedata.sLight, normal, texCoords);


    outColor = vec4(result, 1.0f);
}
