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
	float Dissolveness;

	vec3 DiffuseMap;
	vec3 SpecularMap;
	vec3 NormalMap;
	vec3 DepthMap;
	vec3 AlphaMap;
	vec3 EmissionMap;
};

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

struct hitPayload
{
	vec3 hitValue;
	uint seed;
};

struct shadowPayload
{
	bool isHit;
	uint seed;
};