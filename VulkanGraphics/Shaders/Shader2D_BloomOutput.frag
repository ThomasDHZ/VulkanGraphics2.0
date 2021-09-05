#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "Structs.glsl"

#define MAXPOINTLIGHTS 4

layout(binding = 1) uniform sampler2D DiffuseMap;
layout(binding = 2) uniform sampler2D SpecularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D AlphaMap;
layout(binding = 5) uniform sampler2D EmissionMap;
layout(binding = 6) uniform MeshProperties
{
    Material material;
    vec2 UVOffset;
    int UseDiffuseMapBit;
    int UseSpecularMapBit;
    int UseNormalMapBit;
    int UseDepthMapBit;
    int UseAlphaMapBit;
    int UseEmissionMapBit;
    int UseReflectionMapBit;
    int UseSkyBoxBit;
    float minLayers;
    float maxLayers;
    float heightScale;
    float timer;
    int ReflectSprite;
    vec2 UVScale;
    float EmissionStrength;
} meshProperties;
layout(binding = 7) uniform Light
{
    DirectionalLightStruct DLight;
    PointLightStruct PLight[MAXPOINTLIGHTS];
    SpotLightStruct SLight;
    vec3 viewPos;
} light;
layout(binding = 8) uniform ExtendedMesh
{
    int empty;
} extended;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

void RemoveAlphaPixels(vec2 UV)
{
    if((textureSize(AlphaMap, 0).x > 1 &&
        texture(AlphaMap, UV).r == 0) ||
        texture(DiffuseMap, UV).a == 0)
   {
        discard;
   }
}

vec3 DirectionalLight(vec3 V, vec3 N, vec2 UV, DirectionalLightStruct directionalLight)
{
    vec3 L = normalize(-(TBN * directionalLight.direction));
    vec3 R = reflect(-L, N);
    vec3 H = normalize(L + V);  

    vec3 color = texture(DiffuseMap, UV).rgb;
   	vec3 ambient = directionalLight.ambient * color;
	vec3 diffuse = directionalLight.diffuse * (max(dot(L, N), 0.0) * color);
    vec3 specular = vec3(1.0f, 1.0f, 1.0f);

        specular = directionalLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * texture(SpecularMap, UV).rgb;

    return (ambient + diffuse + specular);
}

vec3 PointLight(vec3 TangentFragPos, vec3 V, vec3 N, vec2 UV, PointLightStruct pointLight)
{
        vec3 TangentLightPos = TBN * pointLight.position;
        float distance = length(TangentLightPos - TangentFragPos);
        float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));    
  
		vec3 L = normalize(TangentLightPos - TangentFragPos);
		vec3 R = reflect(-L, N);
		vec3 H = normalize(L + V);  

        vec3 color = texture(DiffuseMap, UV).rgb;
   		vec3 ambient = pointLight.ambient * color;
		vec3 diffuse = pointLight.diffuse * (max(dot(L, N), 0.0) * color);
        vec3 specular = vec3(1.0f, 1.0f, 1.0f);

            specular = pointLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * texture(SpecularMap, UV).rgb;


        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        
        return (ambient + diffuse + specular);
}

vec3 SpotLight(vec3 TangentFragPos, vec3 V, vec3 N, vec2 UV, SpotLightStruct spotLight)
{
         vec3 TangentLightPos = TBN * spotLight.position;
        float distance = length(TangentLightPos - TangentFragPos);
        float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    

		vec3 L = normalize(TangentLightPos - TangentFragPos);
		vec3 R = reflect(-L, N);
		vec3 H = normalize(L + V);  

        vec3 TangentDirection = TBN * spotLight.direction;
        float theta = dot(L, normalize(-TangentDirection)); 
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 color = texture(DiffuseMap, UV).rgb;
   		vec3 ambient = spotLight.ambient * color;
		vec3 diffuse = spotLight.diffuse * (max(dot(L, N), 0.0) * color);
        vec3 specular = vec3(1.0f, 1.0f, 1.0f);

        specular = spotLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * texture(SpecularMap, UV).rgb;


        ambient *= attenuation * intensity;
        diffuse *= attenuation * intensity;
        specular *= attenuation * intensity;

        return (ambient + diffuse + specular);
}

void main()
{           
    RemoveAlphaPixels(TexCoords);

    vec3 V = light.viewPos;
    vec3 N = Normal;

    vec3 TangentLightDirection = TBN * light.DLight.direction;
    vec3 TangentViewPos  = TBN * light.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;

        N = texture(normalMap, TexCoords).rgb;
        N = normalize(N * 2.0 - 1.0);   


   vec3 result = DirectionalLight( V,  N,  TexCoords, light.DLight);
   for(int x = 0; x < MAXPOINTLIGHTS; x++)
   {
      if(light.PLight[x].InUseFlag == 1)
      {
        result += PointLight(TangentFragPos,  V,  N,  TexCoords, light.PLight[x]);
      } 
    }
   //result = mix(result, texture(ReflectDiffuseMap, UV).rgb, 0.15f);
   FragColor = vec4(result, 1.0f);

      BloomColor = vec4(texture(EmissionMap, TexCoords).rgb, 1.0f);
}

