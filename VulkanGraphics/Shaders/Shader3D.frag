#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "Structs.glsl"

#define MAXPOINTLIGHTS 4

layout(binding = 1) uniform sampler2D DiffuseMap;
layout(binding = 2) uniform sampler2D SpecularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D depthMap;
layout(binding = 5) uniform sampler2D AlphaMap;
layout(binding = 6) uniform sampler2D EmissionMap;
layout(binding = 7) uniform sampler2D ReflectionMap;
layout(binding = 8) uniform sampler2D ShadowMap;
layout(binding = 9) uniform samplerCube SkyBox;
layout(binding = 10) uniform MeshProperties
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
} meshProperties;
layout(binding = 11) uniform Light
{
    DirectionalLightStruct dLight;
    PointLightStruct pLight[MAXPOINTLIGHTS];
    SpotLightStruct sLight;
    vec3 viewPos;
} light;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;

void RemoveAlphaPixels(vec2 UV);
vec3 DirectionalLight(DirectionalLightStruct directionalLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N);
vec3 PointLight(PointLightStruct pointLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N);
vec3 SpotLight(SpotLightStruct spotLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N);
vec3 Reflect(vec3 N, vec3 TangentViewPos);
float ShadowCalculation(vec4 fragPosLightSpace);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{           
    vec3 TangentViewPos  = TBN * light.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;
    
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords;

    RemoveAlphaPixels(texCoords);
    
    if(meshProperties.UseDepthMapBit == 1)
    {
        texCoords = ParallaxMapping(TexCoords,  viewDir);       
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
    }

    vec3 normal = texture(normalMap, texCoords).rgb;
    if(meshProperties.UseNormalMapBit == 1)
    {
        normal = normalize(normal * 2.0 - 1.0);   
    }
   
    vec3 result = DirectionalLight(light.dLight, TangentFragPos, viewDir, texCoords, normal);
    for(int x = 0; x < MAXPOINTLIGHTS; x++)
    {
       if(light.pLight[x].InUseFlag == 1)
       {
            result += PointLight(light.pLight[x], TangentFragPos, viewDir, texCoords, normal);
       }
    }
    result += SpotLight(light.sLight, TangentFragPos, viewDir, texCoords, normal);

    vec3 I = normalize(TangentFragPos - TangentViewPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 Reflected = texture(SkyBox, R).rgb;
    if(meshProperties.UseReflectionMapBit == 1)
    {
        result = mix(result, Reflected, texture(ReflectionMap, texCoords).rgb);
    }
    else
    {
        result = mix(result, Reflected, meshProperties.material.reflectivness);
    }
   

    FragColor = vec4(result, 1.0);
}


void RemoveAlphaPixels(vec2 UV)
{
    if((textureSize(AlphaMap, 0).x > 1 &&
        texture(AlphaMap, UV).r == 0) ||
        texture(DiffuseMap, UV).a == 0)
   {
        discard;
   }
}

vec3 DirectionalLight(DirectionalLightStruct directionalLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N)
{
    vec3 color = meshProperties.material.Diffuse;
    if(meshProperties.UseDiffuseMapBit  == 1)
    {
        color = texture(DiffuseMap, UV).rgb;
    }
      
    vec3 ambient = directionalLight.ambient * color;
    
    vec3 lightDir = normalize(-(TBN * directionalLight.direction));
    float diff = max(dot(lightDir, N), 0.0);
    vec3 diffuse = directionalLight.diffuse * diff * color;

    vec3 H = normalize(lightDir + viewDir);  
    vec3 specular = vec3(1.0f, 1.0f, 1.0f);
    if(meshProperties.UseSpecularMapBit  == 1)
    {
        specular = directionalLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * texture(SpecularMap, UV).rgb;
    }
    else
    {
        specular = directionalLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * meshProperties.material.specular;  
    }

    return ambient + diffuse + specular;
}

vec3 PointLight(PointLightStruct pointLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N)
{
    vec3 TangentLightPos = TBN * pointLight.position;
    float distance = length(TangentLightPos - FragPosition);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));   
           
    vec3 color = meshProperties.material.Diffuse;
    if(meshProperties.UseDiffuseMapBit  == 1)
    {
        color = texture(DiffuseMap, UV).rgb;
    }
        
    vec3 ambient = pointLight.ambient * color;
       
    vec3 lightDir = normalize(TangentLightPos - FragPosition);
    float diff = max(dot(lightDir, N), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * color;

    vec3 H = normalize(lightDir + viewDir);  
    vec3 specular = vec3(1.0f, 1.0f, 1.0f);
    if(meshProperties.UseSpecularMapBit  == 1)
    {
        specular = pointLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * texture(SpecularMap, UV).rgb;
    }
    else
    {
        specular = pointLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * meshProperties.material.specular;  
    }
  
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 SpotLight(SpotLightStruct spotLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N)
{
    vec3 color = meshProperties.material.Diffuse;
    if(meshProperties.UseDiffuseMapBit  == 1)
    {
        color = texture(DiffuseMap, UV).rgb;
    }

    vec3 TangentLightPos = TBN * light.sLight.position;
    vec3 lightDir = normalize(TangentLightPos - FragPosition);
    
    float diff = max(dot(N, lightDir), 0.0);
    
    vec3 H = normalize(lightDir + viewDir);  
    
    float distance = length(TangentLightPos - FragPosition);
    float attenuation = 1.0 / (light.sLight.constant + light.sLight.linear * distance + light.sLight.quadratic * (distance * distance));    
    
    float theta = dot(lightDir, normalize(-(TBN * light.sLight.direction)));
    float epsilon = light.sLight.cutOff - light.sLight.outerCutOff;
    float intensity = clamp((theta - light.sLight.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = light.sLight.ambient * color;
    vec3 diffuse = light.sLight.diffuse * diff * color;

    vec3 specular = vec3(1.0f, 1.0f, 1.0f);
    if(meshProperties.UseSpecularMapBit  == 1)
    {
        specular = spotLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * texture(SpecularMap, UV).rgb;
    }
    else
    {
        specular = spotLight.specular * pow(max(dot(N, H), 0.0), meshProperties.material.shininess) * meshProperties.material.specular;  
    }

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return ambient + diffuse + specular;
}

vec3 Reflect(vec3 N, vec3 TangentViewPos)
{
   vec3 I = normalize((TBN * FragPos) - TangentViewPos);
   vec3 R = reflect(I, normalize(N));
   return texture(SkyBox, R).rgb;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(ShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.dLight.direction - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    float numLayers = mix(meshProperties.maxLayers, meshProperties.minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * meshProperties.heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}