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
layout(binding = 6) uniform sampler2D DiffuseMap[];
layout(binding = 7) readonly buffer _PosBuf {float pos[];};
layout(binding = 8) readonly buffer _TexCoordBuf {float texcoord0[];};
layout(binding = 9) readonly buffer _NormalBuf {float normals[];};
//layout(binding = 11) readonly buffer MaterialLi {Material material; } MaterialList;

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


vec3 getVertex(uint index)
{
  vec3 vp;
  vp.x = pos[3 * index + 0];
  vp.y = pos[3 * index + 1];
  vp.z = pos[3 * index + 2];
  return vp;
}

vec2 getTexCoord(uint index)
{
  vec2 vp;
  vp.x = texcoord0[2 * index + 0];
  vp.y = texcoord0[2 * index + 1];
  return vp;
}

vec3 getNormal(uint index)
{
  vec3 vp;
  vp.x = normals[3 * index + 0];
  vp.y = normals[3 * index + 1];
  vp.z = normals[3 * index + 2];
  return vp;
}

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

VertexData GetVertexData()
{
	VertexData data;

	const ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID], 
						indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1], 
						indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	const Vertex v0 = unpack(index.x);
	const Vertex v1 = unpack(index.y);
	const Vertex v2 = unpack(index.z);

	const vec2 uv0 = getTexCoord(index.x);
	const vec2 uv1 = getTexCoord(index.y);
	const vec2 uv2 = getTexCoord(index.z);

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	const vec3 position = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	data.Position = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));

	const vec3 normal = v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z;
	data.Normal = normalize(vec3(normal * gl_WorldToObjectEXT));

	data.UV = uv0 * barycentricCoords.x + uv1 * barycentricCoords.y + uv2 * barycentricCoords.z;

	const vec3 tangent = v0.tangent.xyz * barycentricCoords.x + v1.tangent.xyz * barycentricCoords.y + v2.tangent.xyz * barycentricCoords.z;
	data.Tangent = normalize(vec3(tangent * gl_WorldToObjectEXT));

	return data;
}

void main()
{
	const VertexData vertexData = GetVertexData();

	vec3 color = texture(DiffuseMap[0], vertexData.UV).rgb;
	vec3 ambient = ubo.ambient * color;

	vec3 lightDir = normalize(ubo.lightPos.xyz - vertexData.Position);
	float diff =  max(dot(lightDir, vertexData.Normal), 0.0);
	vec3 diffuse = ubo.diffuse * diff * color;

	hitValue = ambient + diffuse;

//// switch(gl_InstanceCustomIndexEXT)
//// {
////    case 0: a = vec3(1.0f, 0.0f, 0.0f); break;
////    case 1: a = vec3(0.0f, 1.0f, 0.0f); break;
////    case 2: a = vec3(0.0f, 0.0f, 1.0f); break;
////    case 3: a = vec3(1.0f, 1.0f, 0.0f); break;
////    case 4: a = vec3(1.0f, 0.0f, 1.0f); break;
////    case 5: a = vec3(0.0f, 1.0f, 1.0f); break;
////    case 6: a = vec3(1.0f, 1.0f, 1.0f); break;
//// }

	// Shadow casting
	float tmin = 0.001;
	float tmax = 10000.0;
	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	shadowed = true;  

	if(dot(vertexData.Normal, lightDir) > 0)
	{
		
			vec3 viewDir = normalize(ubo.viewPos.xyz - vertexData.Position);
			vec3 reflectDir = reflect(-lightDir, vertexData.Normal);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), ubo.shininess);
			vec3 specular = ubo.specular * spec;

			hitValue += specular;
		
	}
}