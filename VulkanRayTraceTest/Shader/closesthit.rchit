#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 2) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

struct PrimMeshInfo
{
  uint VertexOffset;
  uint IndiceOffset;
};

struct Material
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
};

layout(binding = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform UBO 
{
	mat4 viewInverse;
	mat4 projInverse;
	mat4 modelInverse;
	vec3 lightPos;
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 viewPos;
	float shininess;
	int vertexSize;
} ubo;
layout(binding = 3) buffer Vertices { vec4 v[]; } vertices[];
layout(binding = 4) buffer Indices { uint i[]; } indices[];
layout(binding = 5) buffer Materials { Material material[]; } MaterialList;
layout(binding = 6) uniform sampler2D DiffuseMap[];
layout(binding = 8) readonly buffer _TexCoordBuf {float texcoord0[];};

struct Vertex
{
  vec3 pos;
  vec3 normal;
  vec2 uv;
  vec4 tangent;
  vec4 BiTangant;
  vec4 Color;
  vec4 BoneID;
  vec4 BoneWeights;
 };

Vertex unpack(uint index)
{
	const int m = ubo.vertexSize / 16;

	vec4 d0 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 0];
	vec4 d1 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 1];
	vec4 d2 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 2];

	Vertex v;
	v.pos = d0.xyz;
	v.normal = vec3(d0.w, d1.x, d1.y);
	v.Color = vec4(d2.x, d2.y, d2.z, 1.0);
	v.uv = vec2(d0.x, 
				d1.y);
	v.tangent = vec4(d0.w, d1.y, d1.y, 0.0f);

	return v;
}

vec2 getTexCoord(uint index)
{
  vec2 vp;
  vp.x = texcoord0[2 * index + 0];
  vp.y = texcoord0[2 * index + 1];
  return vp;
}

void main()
{
ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID], 
						indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1], 
						indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	Vertex v0 = unpack(index.x);
	Vertex v1 = unpack(index.y);
	Vertex v2 = unpack(index.z);

	const vec2 uv0 = getTexCoord(index.x);
	const vec2 uv1 = getTexCoord(index.y);
	const vec2 uv2 = getTexCoord(index.z);

	// Interpolate normal
	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	vec3 normal = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	vec2 UV = uv0 * barycentricCoords.x + uv1 * barycentricCoords.y + uv2 * barycentricCoords.z;

	// Basic lighting
	vec3 lightVector = normalize(ubo.lightPos.xyz);
	float dot_product = max(dot(lightVector, normal), 0.2);

 // hitValue = texture(DiffuseMap[MaterialList.material[gl_InstanceCustomIndexEXT].DiffuseMapID], UV).rgb * dot_product; 
	hitValue = vec3(0.7f) * dot_product;
 
	// Shadow casting
	float tmin = 0.001;
	float tmax = 10000.0;
	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	shadowed = true;  
	// Trace shadow ray and offset indices to match shadow hit/miss shader group indices
//	traceRayEXT(topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightVector, tmax, 2);
//	if (shadowed) {
//		hitValue = vec3(1.0f, 0.0f, 0.0f);
//	}
}