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
    int DepthSampler;
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

#define MAX_STEPS 100
#define MAX_DIST 100.0f
#define SURF_DIST 0.01

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

//	vec2 direction = viewDir.xy * 0.21f;
//	for(int x = 0; x < 4; x++)
//	{
//		float parallax = texture(TextureMap[3], texCoords).r;
//		texCoords += direction * parallax;
//	}
//	return texCoords;
    float heightScale = 0.05f;
    // number of depth layers
    const float minLayers = 20;
    const float maxLayers = 40;
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
 //debugPrintfEXT("Depth: %i \n",  scenedata.DepthSampler);
    vec3 TangentLightPos = TBN * scenedata.plight.position;
    vec3 TangentViewPos  = TBN * scenedata.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;
  // offset texture coordinates with Parallax Mapping
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords;
    
//    texCoords = ParallaxMapping(TexCoords,  viewDir);       
//    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//        discard;
//
    // obtain normal from normal map
//    vec3 normal = texture(TextureMap[2], texCoords.xy).rgb;
//    normal = normalize(normal * 2.0 - 1.0);   
   
    // get diffuse color
    vec3 color = texture(Texture3DMap[0], vec3(texCoords, 15)).rrr;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;

    outColor = vec4(ambient + diffuse + specular, 1.0f);
//
//
//    	const int search_steps = 128;
//	
//	vec3 p = vec3(texCoords,0);
//	
//	vec3 o = TangentViewPos + p;
//	o.z = texture(TextureMap[3],o.xy).r;
//	
//	vec3 v = o - p;
//	v /= v.z;
//	v *= 1.0-o.z;
//	v /= search_steps;
//
//	p = o;
//
//	for( int i=0;i<search_steps;i++ )
//	{
//		float d = texture(TextureMap[3],p.xy).r;
//
//		if ( d <= p.z )
//			p += v;
//	}
//	float d = texture(TextureMap[3],texCoords).r;
//	
//	float r = length(p.xy-texCoords);
//	
//	r = (p.z >= d) ? 1.0 : r / (d - p.z);		
//
//	vec2 temp = texCoords;
//	temp.x = 1 - texCoords.x;
//	temp.y = 1 - texCoords.y;
//	float best_r = texture(TextureMap[3],temp).x;
//	if ( r > best_r )
//		r = best_r;
//		saturate



//	vec3 p;
//	p.xy = texCoords + TangentViewPos.xy;
//	p.z = texture(TextureMap[3],p.xy).r;
//	
//	float d = texture(TextureMap[3], texCoords).r;
//
//	float r = length(TangentViewPos.xy);
//	r = (p.z >= d) ? 1.0 : r / (d - p.z);
//		
//	vec2 temp = texCoords;
//	temp.y = 1 - texCoords.y;
//
//	float best_r = texture(TextureMap[3], temp).x;
//	if ( r > best_r )
//		r = best_r;
//		
//
//    outColor = vec4(r,r,r,r);
}