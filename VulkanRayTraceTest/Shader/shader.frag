#version 450
#extension GL_ARB_separate_shader_objects : enable



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

//layout(binding = 0) uniform accelerationStructureEXT topLevelAS;
//layout(binding = 1, set = 0, rgba8) uniform image2D image;
layout(binding = 2) uniform UBO 
{
	mat4 viewInverse;
	mat4 projInverse;
	mat4 modelInverse;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	int vertexSize;
} ubo;
//layout(binding = 3) buffer Vertices { vec4 v[]; } vertices[];
//layout(binding = 4) buffer Indices { uint i[]; } indices[];
//layout(binding = 5) buffer MaterialInfos { MaterialInfo materialInfo[]; } MaterialList;
layout(binding = 6) uniform sampler2D TextureMap[];
//layout(binding = 8) readonly buffer _TexCoordBuf {float texcoord0[];};


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(TextureMap[0], fragTexCoord);
}