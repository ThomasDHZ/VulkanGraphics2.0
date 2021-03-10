#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

#include "Lighting.glsl"

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 2) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

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

struct Vertex
{
  vec3 pos;
  float padding1;
  vec3 normal;
  float padding2;
  vec2 uv;
  vec2 padding3;
  vec3 tangent;
  float padding4;
  vec3 BiTangant;
  float padding5;
  vec4 Color;
  vec4 BoneID;
  vec4 BoneWeights;
 };

layout(binding = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform UBO 
{
	mat4 viewInverse;
	mat4 projInverse;
    mat4 model;
    mat4 view;
    mat4 proj;
    DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
    float vertexSize;
	mat4 PVM;
    mat4 BoneTransform[100];
	float timer;
} ubo;
layout(binding = 3) buffer MeshProperties 
{
	mat4 model;
	mat4 BoneTransform[100];
	vec2 UVOffset;
} meshProperties;
layout(binding = 4, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 5) buffer Indices { uint i[]; } indices[];
layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D TextureMap[];
layout(binding = 9) uniform sampler3D Texture3DMap[];

Material BuildMaterial(vec2 UV)
{
	Material material;
	material.Ambient = MaterialList[gl_InstanceCustomIndexEXT].material.Ambient;
	material.Diffuse = MaterialList[gl_InstanceCustomIndexEXT].material.Diffuse;
	material.Specular = MaterialList[gl_InstanceCustomIndexEXT].material.Specular;
	material.Shininess = MaterialList[gl_InstanceCustomIndexEXT].material.Shininess;
	material.Reflectivness = MaterialList[gl_InstanceCustomIndexEXT].material.Reflectivness;

	if(MaterialList[gl_InstanceCustomIndexEXT].material.DiffuseMapID == 0)
	{
		material.DiffuseMap = MaterialList[gl_InstanceCustomIndexEXT].material.Diffuse;
	}
	else
	{
		material.DiffuseMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.DiffuseMapID], UV));
	}

	if(MaterialList[gl_InstanceCustomIndexEXT].material.SpecularMapID == 0)
	{
		material.SpecularMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.SpecularMapID], UV));
	}
	else
	{
		material.SpecularMap = MaterialList[gl_InstanceCustomIndexEXT].material.Specular;
	}

	material.SpecularMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.SpecularMapID], UV));
	material.NormalMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.NormalMapID], UV));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.AlphaMapID], UV));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.EmissionMapID], UV));
	material.ShadowMap = vec3(texture(TextureMap[MaterialList[gl_InstanceCustomIndexEXT].material.ShadowMapID], UV));
	return material;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float heightScale = 0.1;
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

void main()
{
	const mat4 rayTransform = MeshTransform[gl_InstanceCustomIndexEXT].Transform;
    const ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	const Vertex v0 = vertices[gl_InstanceCustomIndexEXT].v[index.x];
	const Vertex v1 = vertices[gl_InstanceCustomIndexEXT].v[index.y];
	const Vertex v2 = vertices[gl_InstanceCustomIndexEXT].v[index.z];

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);

	vec3 worldPos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	worldPos = vec3(ubo.model * rayTransform * vec4(worldPos, 1.0));

	vec2 UV = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;

	vec3 normal = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	normal = mat3(transpose(inverse(ubo.model * rayTransform))) * normal;  

	vec3 tangent = v0.tangent * barycentricCoords.x + v1.tangent * barycentricCoords.y + v2.tangent * barycentricCoords.z;
	vec3 bitangent = v0.BiTangant * barycentricCoords.x + v1.BiTangant * barycentricCoords.y + v2.BiTangant * barycentricCoords.z;
	vec3 color = v0.Color.xyz * barycentricCoords.x + v1.Color.xyz * barycentricCoords.y + v2.Color.xyz * barycentricCoords.z;


	vec3 T = normalize(mat3(ubo.model) * vec3(tangent));
    vec3 B = normalize(mat3(ubo.model) * vec3(bitangent));
    vec3 N = normalize(mat3(ubo.model) * normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 TangentLightPos = TBN * ubo.plight.position;
    vec3 TangentViewPos  = TBN * ubo.viewPos;
    vec3 TangentFragPos  = TBN * worldPos;
  // offset texture coordinates with Parallax Mapping
    // offset texture coordinates with Parallax Mapping
  //  vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    
   // UV = ParallaxMapping(UV,  viewDir);       
//    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//        discard;

    // obtain normal from normal map
//    vec3 normal2 = texture(TextureMap[2], UV).rgb;
//    normal2 = normalize(normal2 * 2.0 - 1.0);   
//   
//    // get diffuse color
//    vec3 color = texture(TextureMap[1], UV).rgb;
//    // ambient
//    vec3 ambient = 0.1 * color;
//    // diffuse
//    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
//    float diff = max(dot(lightDir, normal2), 0.0);
//    vec3 diffuse = diff * color;
//    // specular    
//    vec3 reflectDir = reflect(-lightDir, normal);
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    float spec = pow(max(dot(normal2, halfwayDir), 0.0), 32.0);
//
//    vec3 specular = vec3(0.2) * spec;
    hitValue = texture(TextureMap[1], UV).rgb;

//	vec3 lightDir = normalize(-ubo.dlight.direction);
//	float diff = max(dot(ubo.dlight.direction, lightDir), 0.0);
//
//	vec3 ambient = ubo.dlight.ambient *  material.DiffuseMap;
//    vec3 diffuse = ubo.dlight.diffuse * diff *   material.DiffuseMap;
// 
//	hitValue = ambient + diffuse;


//	 vec3 lightVector = normalize(ubo.dlight.direction);
//	float dot_product = max(dot(lightVector, normal), 0.2);
//	hitValue = material.DiffuseMap * dot_product;

//	float spec = 0.0f;
////  if(dot(normal, lightDir) > 0)
////  {
	// Shadow casting
//	float tmin = 0.001;
//	float tmax = 10000.0;
//	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
//	shadowed = true;  
//	// Trace shadow ray and offset indices to match shadow hit/miss shader group indices
//	traceRayEXT(topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 2);
//	if (shadowed) {
//		hitValue *= vec3(0.0f, 1.0f, 0.0f);
//	}
//	else
//	{	
		//hitValue += specular;
//	}
 // }
}