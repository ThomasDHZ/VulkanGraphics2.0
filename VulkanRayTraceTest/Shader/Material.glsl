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

	vec3 Albedo;
	float Matallic;
	float Roughness;
	float AmbientOcclusion;
	uint AlbedoMapID;
	uint MatallicMapID;
	uint RoughnessMapID;
	uint AOMapID;
};

struct PBRMaterial
{
    vec3 Albedo;
    vec3 Normal;
    vec3 Depth;
    vec3 Emission;
    vec3 Alpha;
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
};
