#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAXPOINTLIGHTS 4

struct Material {
    vec3 ambient;
    vec3 Diffuse;
    vec3 specular;    
    float shininess;
    float reflectivness;
}; 

struct DirectionalLightStruct {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int InUseFlag;
};

struct PointLightStruct {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int InUseFlag;
};

struct SpotLightStruct {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

layout(binding = 1) uniform sampler2D DiffuseMap;
layout(binding = 2) uniform sampler2D SpecularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D depthMap;
layout(binding = 5) uniform sampler2D AlphaMap;
layout(binding = 6) uniform sampler2D EmissionMap;
layout(binding = 7) uniform sampler2D ReflectionMap;
layout(binding = 8) uniform samplerCube SkyBox;
layout(binding = 9) uniform MeshProperties
{
    Material material;
} meshProperties;
layout(binding = 10) uniform Light
{
    DirectionalLightStruct DLight;
    PointLightStruct PLight[MAXPOINTLIGHTS];
    SpotLightStruct SLight;
    vec3 viewPos;
} light;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

vec3 CalcDirLight(DirectionalLightStruct light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{  
    vec3 V = normalize(light.viewPos - FragPos);
    vec3 N = normalize(Normal);

    vec3 TangentLightDirection = TBN * light.DLight.direction;
    vec3 TangentViewPos  = TBN * light.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;


    vec3 result = CalcDirLight(light.DLight, N, V);
    for(int x = 0; x < MAXPOINTLIGHTS; x++)
    {
      if(light.PLight[x].InUseFlag == 1)
      {
             result += CalcPointLight(light.PLight[x], N, FragPos, V);  
      } 
    }
    result += CalcSpotLight(light.SLight, N, FragPos, V); 

    FragColor = vec4(result, 1.0);
    BloomColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

vec3 CalcDirLight(DirectionalLightStruct light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), meshProperties.material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(DiffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(DiffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(SpecularMap, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), meshProperties.material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(DiffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(DiffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(SpecularMap, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), meshProperties.material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(DiffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(DiffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(SpecularMap, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}