#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshID;
	uint ModelID;
	uint MaterialID;
} Mesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

struct MaterialInfo
{
	vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;    
    float Shininess;
    float Reflectivness;

	uint DiffuseMapID;
	uint SpecularMapID;
	uint NormalMapID;
	uint DepthMapID;
	uint AlphaMapID;
	uint EmissionMapID;
		uint ShadowMapID;
};

struct Material
{
	vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;    
    float Shininess;
    float Reflectivness;

	vec3 DiffuseMap;
	vec3 SpecularMap;
	vec3 NormalMap;
	vec3 DepthMap;
	vec3 AlphaMap;
	vec3 EmissionMap;
		vec3 ShadowMap;
};

layout(binding = 2) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	mat4 model;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	float vertexSize;	
	mat4 PVM;
	mat4 BoneTransform[100];
    float DepthSampler;
} scenedata;
layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];


layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outColor;


Material BuildMaterial()
{
	Material material;
	material.Ambient = MaterialList[Mesh.MeshID].material.Ambient;
	material.Diffuse = MaterialList[Mesh.MeshID].material.Diffuse;
	material.Specular = MaterialList[Mesh.MeshID].material.Specular;
	material.Shininess = MaterialList[Mesh.MeshID].material.Shininess;
	material.Reflectivness = MaterialList[Mesh.MeshID].material.Reflectivness;

	if(MaterialList[Mesh.MeshID].material.DiffuseMapID == 0)
	{
		material.DiffuseMap = MaterialList[Mesh.MeshID].material.Diffuse;
	}
	else
	{
		material.DiffuseMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.DiffuseMapID], TexCoords));
	}

	if(MaterialList[Mesh.MeshID].material.SpecularMapID == 0)
	{
		material.SpecularMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.SpecularMapID], TexCoords));
	}
	else
	{
		material.SpecularMap = MaterialList[Mesh.MeshID].material.Specular;
	}

	material.SpecularMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.SpecularMapID], TexCoords));
	material.NormalMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.NormalMapID], TexCoords));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.AlphaMapID], TexCoords));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.EmissionMapID], TexCoords));
	material.ShadowMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.ShadowMapID], TexCoords));
	return material;
}

#define NUM_ITERATIONS 16
#define MAX_STEPS 100
#define MAX_DIST 100.0f
#define SURF_DIST 0.01

bool DEPTH_BIAS = false;
bool BORDER_CLAMP = false;

vec3 ambientLightColor = {0.15, 0.15, 0.15};
vec3 lightPos;
vec3 lightColor = {1.0f, 1.0f, 1.0f};

vec3 Kd = {0.8, 0.8, 0.8};
vec3 Ks = {0.3, 0.3, 0.3};
float specularPower = 64.0;
float tile = 1.0;

float depth = 0.1;
//float GetDistance(vec3 pointPixel, vec3 RayDirection)
//{
// return length(pointPixel - texture(TextureMap[2], RayDirection.xy).rgb) - texture(RayDirection[3], RayDirection.xy).r;
//}
//
//float RayMarch(vec3 cameraView, vec3 RayDirection)
//{
//    float d0 = 0;
//    for(int x = 0; x < MAX_STEPS; x++)
//    {
//        vec3 point = cameraView + d0 * RayDirection;
//        float ds = GetDistance(point);
//        d0 += ds;
//        if(ds < SURF_DIST || d0 > MAX_DIST)
//        {
//            break;
//        }
//    }
//    return d0;
//}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float heightScale = scenedata.DepthSampler;
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(TextureMap[3], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(TextureMap[3], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[3], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}


void SphereTrace()
{
    vec3 p,v;
    p = vec3(TexCoords, 0);
    v = scenedata.viewPos;
    v.xy *= 16 / 256;

    for (int i = 0; i < NUM_ITERATIONS; i++)
	{
		float stepDist = texture(Texture3DMap[0], p).x;
		p += v * stepDist;
	}

}

vec4 phongShading(vec3 normal, vec3 lightVec, vec3 eyeVec, vec3 materialColor)
{
	vec3 halfVec = normalize(lightVec + eyeVec);

	float diffuseInt = clamp(dot(lightVec, normal), 0.0f, 1.0f);
	float specularInt = clamp(dot(halfVec, normal), 0.0f, 1.0f);
	specularInt = pow(specularInt, specularPower);

	vec4 finalColor;
	finalColor.xyz = materialColor * (ambientLightColor + lightColor * diffuseInt * Kd) +
		specularInt * Ks;
	finalColor.w = 1.0f;

	return finalColor;
}

vec4 bump_ps(vec2 uv0, vec3 eyeVec, vec3 lightVec)
{
//	if (BORDER_CLAMP)
//	{
//		if (uv0.x < 0.0) discard;
//		if (uv0.x > 1.0f) discard;
//		if (uv0.y < 0.0) discard;
//		if (uv0.y > 1.0f) discard;
//	}

// View and light vectors
vec3 v = normalize(eyeVec);
vec3 l1 = normalize(lightVec);

// Diffuse texture color
vec3 color = texture(TextureMap[1], uv0).rgb;

// Bump map
//vec3 n = texture(TextureMap[2], uv0).rgb;
//n.xy = n.xy * 2.0 - 1.0;
//n.y = -n.y;
//n.z = sqrt(1.0 - dot(n.xy, n.xy));

vec3 n = texture(TextureMap[2], uv0.xy).rgb;
    n = normalize(n * 2.0 - 1.0);  

//return vec4(n, 1.0f);
return phongShading(n, l1, -v, color);
}


vec4 sphere_ps()
{
    vec3 TangentLightPos = TBN * scenedata.plight.position;
    vec3 TangentViewPos  = TBN * scenedata.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;

	vec3 p,v;

	p = vec3(TexCoords, 0);

	// Texture width MUST be equal to texture height
	TangentViewPos *= -scenedata.DepthSampler * 15.0f / 256.0f *
		256;

	v = normalize(TangentViewPos);
	v.z = abs(v.z);

//	if (DEPTH_BIAS)
//	{
//		float db = 1.0 - v.z;
//		db *= db;
//		db = 1.0 - db * db;
//		v.xy *= db;
//	}

	v.xy *= 16 / 256.0f;

	for (int i = 0; i < NUM_ITERATIONS; i++)
	{
		float stepDist = texture(Texture3DMap[0], p).x;
		p += v * stepDist;
	}

	return bump_ps(p.xy, TangentViewPos, TangentLightPos);
}

void main() 
{
    vec3 TangentLightPos = TBN * scenedata.plight.position;
    vec3 TangentViewPos  = TBN * scenedata.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;
  // offset texture coordinates with Parallax Mapping
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords;
    
		 outColor = vec4(texture(TextureMap[1], texCoords).rgb, 1.0f);
//    texCoords = ParallaxMapping(TexCoords,  viewDir);       
////    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
////        discard;
//
//    // obtain normal from normal map
//    vec3 normal = texture(TextureMap[2], texCoords).rgb;
//    normal = normalize(normal * 2.0 - 1.0);   
//   
//    // get diffuse color
//    vec3 color = texture(TextureMap[1], texCoords).rgb;
//    // ambient
//    vec3 ambient = 0.1 * color;
//    // diffuse
//    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * color;
//    // specular    
//    vec3 reflectDir = reflect(-lightDir, normal);
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//
//    vec3 specular = vec3(0.2) * spec;
//    outColor = vec4(ambient + diffuse + specular, 1.0);
}