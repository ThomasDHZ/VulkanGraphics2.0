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


struct DirectionalLight
{
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float constant;
    float linear;
    float quadratic;
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
};

layout(binding = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform UBO 
{
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	mat4 model;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	int vertexSize;
} ubo;
layout(binding = 3) buffer Vertices { vec4 v[]; } vertices[];
layout(binding = 4) buffer Indices { uint i[]; } indices[];
layout(binding = 5) buffer MaterialInfos { MaterialInfo materialInfo[]; } MaterialList;
layout(binding = 6) uniform sampler2D TextureMap[];
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
	vec4 d3 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 3];
	vec4 d4 = vertices[gl_InstanceCustomIndexEXT].v[m * index + 4];

	Vertex v;
	v.pos = d0.xyz;
	v.normal = vec3(d0.w, d1.x, d1.y);
	v.uv = vec2(d1.zw);
	v.tangent = vec4(d2.x, d2.y, d2.z, d2.w);
    v.BiTangant = vec4(d3.x, d3.y, d3.z, d3.w);
	v.Color = vec4(d4.x, d4.y, d4.z, d4.z);
	return v;
}

Material BuildMaterial(vec2 UV)
{
	Material material;
	material.Ambient = MaterialList.materialInfo[gl_InstanceCustomIndexEXT].Ambient;
	material.Diffuse = MaterialList.materialInfo[gl_InstanceCustomIndexEXT].Diffuse;
	material.Specular = MaterialList.materialInfo[gl_InstanceCustomIndexEXT].Specular;
	material.Shininess = MaterialList.materialInfo[gl_InstanceCustomIndexEXT].Shininess;
	material.Reflectivness = MaterialList.materialInfo[gl_InstanceCustomIndexEXT].Reflectivness;
	material.DiffuseMap = vec3(texture(TextureMap[MaterialList.materialInfo[gl_InstanceCustomIndexEXT].DiffuseMapID], UV));
	material.SpecularMap = vec3(texture(TextureMap[MaterialList.materialInfo[gl_InstanceCustomIndexEXT].SpecularMapID], UV));
	material.NormalMap = vec3(texture(TextureMap[MaterialList.materialInfo[gl_InstanceCustomIndexEXT].NormalMapID], UV));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList.materialInfo[gl_InstanceCustomIndexEXT].AlphaMapID], UV));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList.materialInfo[gl_InstanceCustomIndexEXT].EmissionMapID], UV));
	return material;
};

void main()
{
    const ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1], 
						      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	const Vertex v0 = unpack(index.x);
	const Vertex v1 = unpack(index.y);
	const Vertex v2 = unpack(index.z);

	// Interpolate normal
	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	vec3 color = normalize(v0.Color.xyz * barycentricCoords.x + v1.Color.xyz * barycentricCoords.y + v2.Color.xyz * barycentricCoords.z);
	vec3 normal = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	 normal = vec3(ubo.model * vec4(normal, 1.0));
	vec2 UV = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;

	 vec3 worldPos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	 worldPos = vec3(ubo.model * vec4(worldPos, 1.0));

	 	const Material material = BuildMaterial(UV);

	vec3 lightDir = normalize(-ubo.dlight.direction);
	float diff = max(dot(ubo.dlight.direction, lightDir), 0.0);

	vec3 ambient = ubo.dlight.ambient *  material.DiffuseMap;
    vec3 diffuse = ubo.dlight.diffuse * diff *   material.DiffuseMap;
 
	 hitValue = ambient + diffuse;



//	 vec3 lightVector = normalize(ubo.dlight.direction);
//	float dot_product = max(dot(lightVector, normal), 0.2);
//	hitValue = vec3(0.7f) * dot_product;
//









	float spec = 0.0f;
//  if(dot(normal, L) > 0)
//  {
	// Shadow casting
	float tmin = 0.001;
	float tmax = 10000.0;
	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	shadowed = true;  
	// Trace shadow ray and offset indices to match shadow hit/miss shader group indices
	traceRayEXT(topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 2);
	if (shadowed) {
		hitValue *= 0.3f;
	}
	else
	{
		vec3 halfwayDir = normalize(ubo.dlight.direction + ubo.viewPos);  
         spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);
		vec3 specular = ubo.dlight.specular * spec * material.Specular;
		hitValue += specular;
	}


}