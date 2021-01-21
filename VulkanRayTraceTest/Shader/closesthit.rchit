#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 2) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;


layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2, set = 0) uniform UBO 
{
	mat4 viewInverse;
	mat4 projInverse;
	mat4 modelInverse;
	vec4 lightPos;
	vec4 viewPos;
	int vertexSize;
} ubo;
layout(binding = 3, set = 0) buffer Vertices { vec4 v[]; } vertices[];
layout(binding = 4, set = 0) buffer Indices { uint i[]; } indices[];
layout(binding = 5, set = 0) buffer MeshInfo 
{ 
  uint VertexOffset;
  uint IndiceOffset;
} offset;
layout(binding = 6, set = 0) uniform sampler2D DiffuseMap;
layout(set = 0, binding = 7) readonly buffer _TexCoordBuf {float texcoord0[];};

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
	// Unpack the vertices from the SSBO using the glTF vertex structure
	// The multiplier is the size of the vertex divided by four float components (=16 bytes)
	const int m = ubo.vertexSize / 16;

	vec4 d0 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 0];
	vec4 d1 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 1];
	vec4 d2 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 2];

	Vertex v;
	v.pos = d0.xyz;
	v.normal = vec3(d0.w, d1.x, d1.y);
	v.Color = vec4(d2.x, d2.y, d2.z, 1.0);
	v.uv = vec2(vertices[gl_InstanceCustomIndexEXT].v[2 * index + 0].x, vertices[gl_InstanceCustomIndexEXT].v[2 * index + 1].y);
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

	vec2 uv0 = getTexCoord(index.x);
		vec2 uv1 = getTexCoord(index.y);
			vec2 uv2 = getTexCoord(index.z);

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	vec3 worldPos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	vec3 normal2 = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	vec2 texCoord = uv0 * barycentricCoords.x + uv1 * barycentricCoords.y + uv2 * barycentricCoords.z;
	vec3 tangent = v0.tangent.xyz * barycentricCoords.x + v1.tangent.xyz * barycentricCoords.y + v2.tangent.xyz * barycentricCoords.z;

	vec3 normal = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	
	vec3 lightVector = normalize(ubo.lightPos.xyz);
	float dot_product = max(dot(lightVector, normal), 0.2);

 hitValue = texture(DiffuseMap, texCoord).rgb;
// vec3 a;
// switch(gl_InstanceCustomIndexEXT)
// {
//    case 0: a = vec3(1.0f, 0.0f, 0.0f); break;
//    case 1: a = vec3(0.0f, 1.0f, 0.0f); break;
//    case 2: a = vec3(0.0f, 0.0f, 1.0f); break;
//    case 3: a = vec3(1.0f, 1.0f, 0.0f); break;
//    case 4: a = vec3(1.0f, 0.0f, 1.0f); break;
//    case 5: a = vec3(0.0f, 1.0f, 1.0f); break;
//    case 6: a = vec3(1.0f, 1.0f, 1.0f); break;
// }
// hitValue = vec3(a);

	//	hitValue = texture(DiffuseMap, texCoord).rgb;
	// Shadow casting
	float tmin = 0.001;
	float tmax = 10000.0;
	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	shadowed = true;  

	traceRayEXT(topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightVector, tmax, 2);
	if (shadowed) {
		hitValue *= 0.3;
	}
}