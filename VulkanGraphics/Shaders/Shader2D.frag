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
    vec2 UVScale;
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
    mat4 lightSpaceMatrix;
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
layout(binding = 9) uniform samplerCube CubeMap[];

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

vec3 CalcNormalDirLight(MaterialInfo material, vec3 FragPos, vec3 normal, vec2 uv, mat3 TBN)
{
    vec3 LightPos = DLight[0].direction;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[0].direction;
        ViewPos = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * FragPos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    const vec3 lightDir = normalize(-LightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + ViewDir);
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

    vec3 ambient = DLight[0].ambient * material.Diffuse.rgb;
    vec3 diffuse = DLight[0].diffuse * diff * material.Diffuse.rgb;
    vec3 specular = DLight[0].specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = DLight[0].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = DLight[0].diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = DLight[0].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    return vec3(ambient + diffuse + specular);
}

void main() 
{
   MaterialInfo material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
   vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;
   texCoords *= meshProperties[ConstMesh.MeshIndex].UVScale;
   if(meshProperties[ConstMesh.MeshIndex].UVFlip.x == 1.0f)
   {
        texCoords.x = 1.0f - texCoords.x;
   }
   if(meshProperties[ConstMesh.MeshIndex].UVFlip.y == 1.0f)
   {
        texCoords.y = 1.0f - texCoords.y;
   }
   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
   {
	 discard;
   }

   outColor = vec4(texture(TextureMap[material.DiffuseMapID], texCoords).rgb, material.Alpha);
   outBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
