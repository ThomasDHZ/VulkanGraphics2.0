

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
	mat4 modelInverse;
	vec4 lightPos;
	vec4 viewPos;
	int vertexSize;
} ubo;
layout(binding = 3, set = 0) buffer Vertices { vec4 v[]; } vertices[];
layout(binding = 4, set = 0) buffer Indices { uint i[]; } indices[];
layout(binding = 5, set = 0) uniform sampler2D DiffuseMap;
layout(binding = 6, set = 0) uniform sampler2D NormalMap;

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

	vec4 d0 = vertices[2].v[m * index + 0];
	vec4 d1 = vertices[2].v[m * index + 1];
	vec4 d2 = vertices[2].v[m * index + 2];

	Vertex v;
	v.pos = d0.xyz;
	v.normal = vec3(d0.w, d1.x, d1.y);
	v.Color = vec4(d2.x, d2.y, d2.z, 1.0);
	v.uv = vec2(d0.x, d0.y);
	v.tangent = vec4(d0.w, d1.y, d1.y, 0.0f);

	return v;
}

void main()
{
	ivec3 index = ivec3(indices[2].i[3 * gl_PrimitiveID], 
						indices[2].i[3 * gl_PrimitiveID + 1], 
						indices[2].i[3 * gl_PrimitiveID + 2]);

	Vertex v0 = unpack(index.x);
	Vertex v1 = unpack(index.y);
	Vertex v2 = unpack(index.z);

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	vec3 worldPos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	vec3 normal2 = normalize(v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z);
	vec2 texCoord = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;
	vec3 tangent = v0.tangent.xyz * barycentricCoords.x + v1.tangent.xyz * barycentricCoords.y + v2.tangent.xyz * barycentricCoords.z;

	mat3 normalMatrix = transpose(inverse(mat3(ubo.modelInverse)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal2);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    vec3 TangentLightPos = TBN * ubo.lightPos.xyz;
    vec3 TangentViewPos  = TBN * ubo.viewPos.xyz;
    vec3 TangentFragPos  = TBN * worldPos;

	vec3 normal = texture(NormalMap, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);  
  

	vec3 color = texture(DiffuseMap, texCoord).rgb;
	vec3 ambient = 0.1 * color;

	vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

	//vec3 lightVector = normalize(ubo.lightPos.xyz);
	//float dot_product = max(dot(lightVector, normal), 0.2);
	hitValue = ambient + diffuse;

	// Shadow casting
	float tmin = 0.001;
	float tmax = 10000.0;
	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	shadowed = true;  
	// Trace shadow ray and offset indices to match shadow hit/miss shader group indices
	traceRayEXT(topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, lightDir, tmax, 2);
	if (shadowed) {
		hitValue *= 0.3;
	}
	else
	{
		vec3 viewDir = normalize(TangentLightPos - TangentFragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

		vec3 specular = vec3(0.2) * spec;

		hitValue += specular;
	}
}