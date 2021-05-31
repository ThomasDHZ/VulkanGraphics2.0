#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} ConstMesh;

layout(binding = 0) uniform sampler2D GPositionTexture;
layout(binding = 1) uniform sampler2D GAlebdoTexture;
layout(binding = 2) uniform sampler2D GNormalTexture;
//layout(binding = 3) uniform sampler2D GTangentTexture;
//layout(binding = 4) uniform sampler2D GBiTangentTexture;
layout(binding = 5) uniform sampler2D GShadowTexture;
layout(binding = 6) uniform sampler2D GReflectionTexture;
layout(binding = 7) uniform sampler2D GSSA0Texture;
layout(binding = 8) uniform sampler2D GSkyBoxTexture;
layout(binding = 9) uniform sampler2D GBloomTexture;
layout(binding = 10) uniform sampler2D NormalMapTexture;
layout(binding = 11) uniform sampler2D SpecularMapTexture;
layout(binding = 12) uniform sampler2D ReflectionMapTexture;
layout(binding = 13) uniform sampler2D GTBN_TangentTexture;
layout(binding = 14) uniform sampler2D GTBN_BiTangentTexture;
layout(binding = 15) uniform sampler2D GTBN_NormalTexture;
layout(binding = 16) uniform UniformBufferObject {
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

layout(binding = 17) buffer DirectionalLight2
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DLight[];

layout(binding = 18) buffer PointLight2
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
} PLight[];

layout(binding = 19) buffer SpotLight2
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


layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

const float Gamma = 2.2f;
const float Exposure = 1.0f;
mat3 TBN;

vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, int index);
vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, int index);

void main() 
{
    vec3 Position = texture(GPositionTexture, TexCoords).rgb;
    vec3 Alebdo = texture(GAlebdoTexture, TexCoords).rgb;
   vec3 Specular = texture(SpecularMapTexture, TexCoords).rgb;
  //  vec3 Normal = texture(GNormalTexture, TexCoords).rgb;
    vec3 Shadow = texture(GShadowTexture, TexCoords).rgb;
    vec4 Reflection = texture(ReflectionMapTexture, TexCoords);
    vec4 RTReflection = texture(GReflectionTexture, TexCoords);
    float SSAO = texture(GSSA0Texture, TexCoords).r;
    vec3 SkyBox = texture(GSkyBoxTexture, TexCoords).rgb;
    vec3 Bloom = texture(GBloomTexture, TexCoords).rgb;
    vec3 normal = texture(GNormalTexture, TexCoords).rgb;
   const vec3 T = texture(GTBN_TangentTexture, TexCoords).rgb;
   const vec3 B = texture(GTBN_BiTangentTexture, TexCoords).rgb;
   const vec3 N = texture(GTBN_NormalTexture, TexCoords).rgb;
   TBN = transpose(mat3(T, B, N));

    vec3 result = vec3(0.0f);
    vec3 ViewPos  = scenedata.viewPos;
    vec3 FragPos2  = Position;
    if(texture(NormalMapTexture, TexCoords).a == 1.0f)
    {
        ViewPos  = TBN * scenedata.viewPos;
        FragPos2  = TBN * Position;
    }
    const vec3 viewDir = normalize(ViewPos - FragPos2);

    if(texture(NormalMapTexture, TexCoords).a == 1.0f)
    {
//        if(material.DepthMapID != 0)
//        {
//            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
//            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//            {
//              discard;
//            }
//        }
        normal = texture(NormalMapTexture, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
     }
     for(int x = 0; x < scenedata.DirectionalLightCount; x++)
     {
        result += CalcNormalDirLight(FragPos2, normal, x);
     }
//     for(int x = 0; x < scenedata.PointLightCount; x++)
//     {
//        result += CalcNormalPointLight(FragPos2, Normal, x);   
//     }
     //result +=  CalcNormalSpotLight(FragPos, scenedata.sLight, normal, texCoords);


//     if(SkyBox != vec3(0.0f))
//    {
//        result = SkyBox;
//    }

     result = mix(result, RTReflection.rgb, Reflection.a);
//    vec3 finalResult = vec3(1.0) - exp(-normal * Exposure);
//    finalResult = pow(finalResult, vec3(1.0 / Gamma));

    //Reflectivty is stored in alpha channel.
    outColor = vec4(result, 1.0f);
}

vec3 CalcNormalDirLight(vec3 FragPos, vec3 normal, int index)
{
   float SSAO = texture(GSSA0Texture, TexCoords).r;
   vec3 Alebdo = texture(GAlebdoTexture, TexCoords).rgb;
   if(scenedata.temp == 1)
   {
     Alebdo *= SSAO;
   }

   float Shiny = texture(GAlebdoTexture, TexCoords).a;
   vec3 Specular = texture(SpecularMapTexture, TexCoords).rgb;

    vec3 LightPos = DLight[index].direction;
    vec3 ViewPos = scenedata.viewPos;
    vec3 FragPos2 = FragPos;
    if(texture(NormalMapTexture, TexCoords).a == 1.0f)
    {
        LightPos = TBN * DLight[index].direction;
        ViewPos = TBN * scenedata.viewPos;
        FragPos2 = TBN * FragPos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    const vec3 lightDir = normalize(-LightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + ViewDir);
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

    vec3 ambient = DLight[index].ambient * Alebdo.rgb;
    vec3 diffuse = DLight[index].diffuse * diff * Alebdo.rgb ;
    vec3 specular = DLight[index].specular * spec * Specular.rgb;

    return vec3(ambient + diffuse + specular);
}

vec3 CalcNormalPointLight(vec3 FragPos, vec3 normal, int index)
{
//    vec3 LightPos = PLight[index].position;
//    vec3 ViewPos = ConstMesh.CameraPos;
//    vec3 FragPos2 = FragPos;
//    if (material.NormalMapID != 0)
//    {
//        LightPos = TBN * PLight[index].position;
//        ViewPos = TBN * ConstMesh.CameraPos;
//        FragPos2 = TBN * FragPos;
//    }
//    vec3 ViewDir = normalize(ViewPos - FragPos2);
//
//    vec3 lightDir = normalize(LightPos - FragPos2);
//    float diff = max(dot(normal, lightDir), 0.0);
//
//    vec3 halfwayDir = normalize(lightDir + ViewDir);
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);
//
//    vec3 ambient = PLight[index].ambient * material.Diffuse.rgb;
//    vec3 diffuse = PLight[index].diffuse * diff * material.Diffuse.rgb;
//    vec3 specular = PLight[index].specular * spec * material.Specular;
//    if (material.DiffuseMapID != 0)
//    {
//        ambient = PLight[index].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
//        diffuse = PLight[index].diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
//    }
//    if (material.SpecularMapID != 0)
//    {
//        specular = PLight[index].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
//    }
//
//    float distance = length(LightPos - FragPos2);
//    float attenuation = 1.0 / (PLight[index].constant + PLight[index].linear * distance + PLight[index].quadratic * (distance * distance));
//    ambient *= attenuation;
//    diffuse *= attenuation;
//    specular *= attenuation;
//
//    return (ambient + diffuse + specular);
    return vec3(0.0f);
}