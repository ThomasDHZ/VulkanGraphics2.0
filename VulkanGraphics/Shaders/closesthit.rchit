#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) rayPayloadInEXT vec3 hitValue;
layout(location = 2) rayPayloadEXT bool shadowed;
hitAttributeEXT vec3 attribs;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2, set = 0) uniform UBO 
{
	mat4 viewInverse;
	mat4 projInverse;
	vec4 lightPos;
	int vertexSize;
} ubo;
layout(binding = 3, set = 0) buffer Vertices { vec4 v[]; } vertices;
layout(binding = 4, set = 0) buffer Indices { uint i[]; } indices;
layout(binding = 5, set = 0) uniform sampler2D DiffuseTexture;


struct Vertex
{
  vec3 Position;
  vec3 Normal;
  vec2 TexureCoord;
  vec3 Tangant;
  vec3 BiTangant;
  ivec4 BoneID;
  vec4 BoneWeights;
 };

Vertex unpack(uint index)
{
	const int m = ubo.vertexSize / 16;

	vec4 d0 = vertices.v[m * index + 0];
	vec4 d1 = vertices.v[m * index + 1];
	vec4 d2 = vertices.v[m * index + 2];

	Vertex v;
	v.Position = vec3(d0.x, d0.y, d0.z);
	v.Normal = vec3(d0.w, d1.x, d1.y);

	return v;
}

void main()
{
	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
	
	ivec3 index = ivec3(indices.i[3 * gl_PrimitiveID], indices.i[3 * gl_PrimitiveID + 1], indices.i[3 * gl_PrimitiveID + 2]);

	Vertex v0 = unpack(index.x);
	Vertex v1 = unpack(index.y);
	Vertex v2 = unpack(index.z);

	vec2 texCoord = v0.TexureCoord * barycentrics.x + v1.TexureCoord * barycentrics.y + v2.TexureCoord * barycentrics.z;

	hitValue = vec3(texCoord.x, texCoord.y, 1.0f);
}
