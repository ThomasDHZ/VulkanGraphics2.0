#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 0) uniform sampler2D GPositionTexture;
layout(binding = 1) uniform sampler2D GAlebdoTexture;
layout(binding = 2) uniform sampler2D GNormalTexture;
layout(binding = 3) uniform sampler2D GShadowTexture;
layout(binding = 4) uniform sampler2D GReflectionTexture;
layout(binding = 5) uniform sampler2D GSSA0Texture;
layout(binding = 6) uniform sampler2D GSkyBoxTexture;
layout(binding = 7) uniform sampler2D GBloomTexture;
layout(binding = 8) uniform UniformBufferObject {
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

layout(binding = 9) buffer DirectionalLight2
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DLight[];

layout(binding = 10) buffer PointLight2
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
} PLight[];

layout(binding = 11) buffer SpotLight2
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
void main() 
{
    vec3 Position = texture(GPositionTexture, TexCoords).rgb;
    vec3 Alebdo = texture(GAlebdoTexture, TexCoords).rgb;
    float Specular = texture(GAlebdoTexture, TexCoords).a;
    vec3 Normal = texture(GNormalTexture, TexCoords).rgb;
    vec3 Shadow = texture(GShadowTexture, TexCoords).rgb;
    vec3 Reflection = texture(GReflectionTexture, TexCoords).rgb;
    float SSAO = texture(GSSA0Texture, TexCoords).r;
    vec3 SkyBox = texture(GSkyBoxTexture, TexCoords).rgb;
    vec3 Bloom = texture(GBloomTexture, TexCoords).rgb;

    const vec3 viewDir = normalize(scenedata.viewPos - Position);

    const vec3 lightDir = normalize(-DLight[0].direction);
    const vec3 lightDir2 = normalize(-DLight[1].direction);
    const float diff = max(dot(Normal, lightDir), 0.0);

    vec3 ambient = DLight[0].ambient * Alebdo;
    ambient += DLight[1].ambient * Alebdo;
    if(scenedata.temp == 1)
    {
      ambient *= SSAO;
    }
    vec3 diffuse = DLight[0].diffuse * diff *  Alebdo * SSAO;
 diffuse += DLight[1].diffuse * diff *  Alebdo * SSAO;

    vec3 result = ambient + diffuse;

    if(Shadow == vec3(1.0f, 0.0f, 0.0f))
    {
        result *= vec3(0.3f);
    }
    else
    {
        const vec3 halfwayDir = normalize(lightDir + viewDir);  
        const float spec = pow(max(dot(Normal, halfwayDir), 0.0), Specular);
        vec3 specular = DLight[0].specular * spec * Specular;

        const vec3 halfwayDir2 = normalize(lightDir2 + viewDir);  
        const float spec2 = pow(max(dot(Normal, halfwayDir2), 0.0), Specular);
        specular += DLight[1].specular * spec2 * Specular;
       result += specular;
    }
//
//      if(Alebdo.r >= 0.8f)
//    {
//    result = Alebdo;
//    }
//    result += Bloom;


    if(SkyBox != vec3(0.0f))
    {
        result = SkyBox;
    }

    vec3 finalResult = vec3(1.0) - exp(-result * Exposure);
    finalResult = pow(result, vec3(1.0 / Gamma));
    outColor = vec4(finalResult, 1.0f);
}
