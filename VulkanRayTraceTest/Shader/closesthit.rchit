#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

#include "Vertex.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 1) rayPayloadInEXT vec3 hitValue2;
layout(location = 2) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

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
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
} meshProperties[];
layout(binding = 4, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 5) buffer Indices { uint i[]; } indices[];
layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D TextureMap[];
layout(binding = 9) uniform sampler3D Texture3DMap[];

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

Vertex BuildVertexInfo()
{
    Vertex vertex;
    const ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	const Vertex v0 = vertices[gl_InstanceCustomIndexEXT].v[index.x];
	const Vertex v1 = vertices[gl_InstanceCustomIndexEXT].v[index.y];
	const Vertex v2 = vertices[gl_InstanceCustomIndexEXT].v[index.z];

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);

	vertex.pos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	vertex.pos = vec3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform * vec4(vertex.pos, 1.0));

    vertex.normal = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	vertex.normal = mat3(transpose(inverse(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform))) * vertex.normal;  

	vertex.uv = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;
    vertex.uv += meshProperties[gl_InstanceCustomIndexEXT].UVOffset;

	vertex.tangent = v0.tangent * barycentricCoords.x + v1.tangent * barycentricCoords.y + v2.tangent * barycentricCoords.z;
	vertex.BiTangant = v0.BiTangant * barycentricCoords.x + v1.BiTangant * barycentricCoords.y + v2.BiTangant * barycentricCoords.z;

   // const vec3 color = v0.Color.xyz * barycentricCoords.x + v1.Color.xyz * barycentricCoords.y + v2.Color.xyz * barycentricCoords.z
	//vertex.Color = vec4(color, 1.0f);

    return vertex;
}

void main()
{
    Vertex vertex = BuildVertexInfo();

	vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform ) * vec3(vertex.tangent));
    vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform ) * vec3(vertex.BiTangant));
    vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform ) * vertex.normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 TangentLightPos = TBN * ubo.plight.position;
    vec3 TangentViewPos  = TBN * ubo.viewPos;
    vec3 TangentFragPos  = TBN * vertex.pos;

    vec2 d = vertex.uv * 2.0 - 1.0;

	float tMin = 0.001;
	float tMax = 10000.0;
  	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	vec4 target = ubo.projInverse * vec4(d.x, d.y, 1, 1) ;
	vec4 rayDir = ubo.viewInverse*vec4(normalize(target.xyz), 0) ;

    uint  flags     = gl_RayFlagsSkipClosestHitShaderEXT;
    hitValue2 = vec3(0.0f);
    traceRayEXT(topLevelAS,  // acceleration structure
                flags,       // rayFlags
                0xFF,        // cullMask
                0,           // sbtRecordOffset
                0,           // sbtRecordStride
                1,           // missIndex
                origin,      // ray origin
                tMin,        // ray min range
                rayDir.xyz,      // ray direction
                tMax,        // ray max range
                1            // payload (location = 1)
               );
    hitValue = texture(TextureMap[1], vertex.uv).rgb;
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