#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(binding = 2) uniform UniformBufferObject {
	DirectionalLight dlight;
	PointLight plight[5];
	SpotLight sLight;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
	float timer;
    int Shadowed;
    int temp;
} scenedata;

layout(binding = 8) uniform sampler2D TextureMap[];

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

vec3 CalcDirLight(vec2 uv);
vec3 CalcPointLight(PointLight light, vec2 uv);
vec3 CalcSpotLight(SpotLight light, vec2 uv);

void main() 
{
    vec3 FragPos = texture(TextureMap[0], TexCoords).rgb;
    vec3 Normal = texture(TextureMap[1], TexCoords).rgb;
    vec3 Diffuse = texture(TextureMap[2], TexCoords).rgb;
    float Specular = texture(TextureMap[2], TexCoords).a;

    const vec3 viewDir = normalize(scenedata.viewPos - FragPos.rgb);

    vec3 result = vec3(0.0f);
         result = CalcDirLight(TexCoords);
    for(int x = 0; x < 5; x++)
    {
        result += CalcPointLight(scenedata.plight[x], TexCoords);   
    }
       result +=  CalcSpotLight(scenedata.sLight, TexCoords);

    outColor = vec4(result, 1.0);
}

vec3 CalcDirLight(vec2 uv)
{
    vec3 FragPos = texture(TextureMap[0], TexCoords).rgb;
    vec3 Normal = texture(TextureMap[1], TexCoords).rgb;
    vec4 Diffuse = texture(TextureMap[2], TexCoords);

    vec3 viewDir = normalize(scenedata.viewPos - FragPos);
    vec3 lightDir = normalize(-scenedata.dlight.direction);

    float diff = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    vec3 ambient = scenedata.dlight.ambient * Diffuse.rgb;
    vec3 diffuse = scenedata.dlight.diffuse * diff * Diffuse.rgb;
    vec3 specular = scenedata.dlight.specular * spec * Diffuse.a;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec2 uv)
{
    vec3 FragPos = texture(TextureMap[0], TexCoords).rgb;
    vec3 Normal = texture(TextureMap[1], TexCoords).rgb;
    vec4 Diffuse = texture(TextureMap[2], TexCoords);

    vec3 viewDir = normalize(scenedata.viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * Diffuse.rgb;
    vec3 specular = light.specular * spec * Diffuse.a;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec2 uv)
{
    vec3 FragPos = texture(TextureMap[0], TexCoords).rgb;
    vec3 Normal = texture(TextureMap[1], TexCoords).rgb;
    vec4 Diffuse = texture(TextureMap[2], TexCoords);

    vec3 viewDir = normalize(scenedata.viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * Diffuse.rgb;
    vec3 specular = light.specular * spec * Diffuse.a;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
