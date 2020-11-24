#version 450
#extension GL_ARB_separate_shader_objects : enable


#define MAXPOINTLIGHTS 128

const int UseDiffuseMapBit    = 0x00000001;
const int UseSpecularMapBit   = 0x00000002;
const int UseNormalMapBit     = 0x00000004;
const int UseDepthMapBit      = 0x00000008;
const int UseAlphaMapBit      = 0x00000016;
const int UseEmissionMapBit   = 0x00000032;
const int UseSkyBoxBit        = 0x00000064;

struct MapBits
{
     int UseDiffuseMapBit;
     int UseSpecularMapBit;
     int UseNormalMapBit;
     int UseDepthMapBit;
     int UseAlphaMapBit;
     int UseEmissionMapBit;
     int UseSkyBoxBit;
};

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
};

struct PointLightStruct {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int InUse;
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
    vec3 test;
} meshProperties;
layout(binding = 10) uniform Light
{
    DirectionalLightStruct DLight;
    PointLightStruct PLight;
    vec3 viewPos;
} light;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;

vec3 CalcDirLight(DirectionalLightStruct light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{  
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(light.viewPos - FragPos);

    vec3 result = CalcDirLight(light.DLight, norm, viewDir);
    result += CalcPointLight(light.PLight, norm, FragPos, viewDir);  

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionalLightStruct light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
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