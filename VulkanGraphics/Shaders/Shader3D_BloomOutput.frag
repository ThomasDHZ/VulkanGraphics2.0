#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAXPOINTLIGHTS 4
const float PI = 3.14159265359;

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
layout(location = 1) out vec4 BloomColor;

void RemoveAlphaPixels(vec2 UV);
vec3 DirectionalLight(DirectionalLightStruct directionalLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N);
vec3 PointLight(PointLightStruct pointLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N);
vec3 SpotLight(vec3 TangentLightPos, vec3 TangentFragPos, vec3 V, vec3 N, vec2 UV, SpotLightStruct spotLight);
vec3 Reflect(vec3 N, vec3 TangentViewPos);
float ShadowCalculation(vec4 fragPosLightSpace);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float SchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  FresnelSchlick(float cosTheta, vec3 F0);

void main()
{           
    //RemoveAlphaPixels();

    vec3 TangentViewPos  = TBN * light.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;
    
       // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords;
    
    texCoords = ParallaxMapping(TexCoords,  viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // obtain normal from normal map
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    vec3 result = DirectionalLight(light.dLight, TangentFragPos, viewDir, texCoords, normal);
    for(int x = 0; x < MAXPOINTLIGHTS; x++)
    {
       if(light.pLight[x].InUseFlag == 1)
       {
            result += PointLight(light.pLight[x], TangentFragPos, viewDir, texCoords, normal);
       }
    }

    FragColor = vec4(result, 1.0);

   if(meshProperties.UseEmissionMapBit == 1)
   {
      BloomColor = vec4(texture(EmissionMap, texCoords).rgb, 1.0f);
   }
   else
   {
      BloomColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
   }
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
      // get diffuse color
        vec3 color = texture(DiffuseMap, UV).rgb;
        // ambient
        vec3 ambient = directionalLight.ambient * color;
        // diffuse
        vec3 lightDir = normalize(-(TBN * directionalLight.direction));
        float diff = max(dot(lightDir, N), 0.0);
        vec3 diffuse = directionalLight.diffuse * diff * color;
        // specular    
        vec3 reflectDir = reflect(-lightDir, N);
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(N, halfwayDir), 0.0), 32.0);

        vec3 specular = directionalLight.specular * spec;

    return (ambient + diffuse + specular);
}

vec3 PointLight(PointLightStruct pointLight, vec3 FragPosition, vec3 viewDir, vec2 UV, vec3 N)
{
           vec3 TangentLightPos = TBN * pointLight.position;
            float distance = length(TangentLightPos - FragPosition);
            float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));   

            // get diffuse color
            vec3 color = texture(DiffuseMap, UV).rgb;
            // ambient
            vec3 ambient = pointLight.ambient * color;
            // diffuse
            vec3 lightDir = normalize(TangentLightPos - FragPosition);
            float diff = max(dot(lightDir, N), 0.0);
            vec3 diffuse = pointLight.diffuse * diff * color;
            // specular    
            vec3 reflectDir = reflect(-lightDir, N);
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(N, halfwayDir), 0.0), 32.0);

            vec3 specular = pointLight.specular * spec; 
  
            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;

            return ambient + diffuse + specular;
}

vec3 SpotLight(vec3 TangentLightPos, vec3 TangentFragPos, vec3 V, vec3 N, vec2 UV, SpotLightStruct spotLight)
{
        float distance = length(TangentLightPos - TangentFragPos);
        float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    

		vec3 L = normalize(TangentLightPos - TangentFragPos);
		vec3 R = reflect(-L, N);
		vec3 H = normalize(L + V);  

        vec3 TangentDirection = TBN * spotLight.direction;
        float theta = dot(L, normalize(-TangentDirection)); 
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 color = vec3(1.0f, 0.0f, 0.0f);
        if (meshProperties.UseDiffuseMapBit  == 1)
        {    
            color = texture(DiffuseMap, UV).rgb;
        }
        else
        {
            color = meshProperties.material.Diffuse;
        }

   		vec3 ambient = spotLight.ambient * color;
		vec3 diffuse = spotLight.diffuse * (max(dot(L, N), 0.0) * color);
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

        return (ambient + diffuse + specular);
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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nominator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0f);
    denominator = PI * denominator * denominator;

    return nominator/denominator;
}

float SchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float nominator = NdotV;
    float denominator = NdotV * (1.0f - k) + k;
    
    return nominator/denominator;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = SchlickGGX(NdotL, roughness);
    float ggx2 = SchlickGGX(NdotV, roughness);

    return ggx1 * ggx2;
}

vec3  FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}