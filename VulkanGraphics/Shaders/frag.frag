#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "material.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform UniformBufferObject {
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    uint SphereAreaLightCount;
    uint TubeAreaLightCount;
    uint RectangleAreaLightCount;
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

layout(binding = 2) buffer DirectionalLight
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float Luminosity;
} DLight[];

layout(binding = 3) buffer PointLight
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float Luminosity;
    mat4 lightSpaceMatrix;
} PLight[];

layout(binding = 4) buffer SpotLight
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
   float Luminosity;
} SLight[];

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer Material { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap[];

layout(binding = 10) buffer SphereAreaLightBuffer {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float SphereRadius;
    float linear;
    float quadratic;
	float Luminosity;
} SphereLight[];

layout(binding = 11) buffer TubeAreaLightBuffer {
	vec3 StartPos;
	vec3 EndPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    float TubeRadius;
	float Luminosity;
} TubeLight[];

layout(binding = 12) buffer RectangleAreaLightBuffer
{
	vec3 VertexPos1;
	vec3 VertexPos2;
	vec3 VertexPos3;
	vec3 VertexPos4;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float Luminosity;
} RectangleLight[];
layout(binding = 13) uniform sampler2D ShadowMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

MaterialInfo material;

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
    vec3 lightDir = normalize(PLight[0].position - FragPos);
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

void main() 
{
    material = MaterialList[meshProperties[Mesh.MeshIndex].MaterialIndex].material;
    vec4 FragPosLightSpace = PLight[0].lightSpaceMatrix * vec4(FragPos, 1.0f);
    vec3 color = texture(TextureMap[material.DiffuseMapID], TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(PLight[0].position - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(Mesh.CameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    outColor = vec4(lighting, 1.0);
}