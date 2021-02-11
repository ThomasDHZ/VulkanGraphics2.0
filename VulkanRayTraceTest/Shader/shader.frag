#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	int MeshID;
	int MaterialID;
} Mesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
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

layout(binding = 0) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	mat4 model;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	int vertexSize;
} scenedata;
//layout(binding = 1, set = 0, rgba8) uniform image2D image;
//layout(binding = 3) buffer Vertices { vec4 v[]; } vertices[];
//layout(binding = 4) buffer Indices { uint i[]; } indices[];
//layout(binding = 5) buffer MaterialInfos { MaterialInfo materialInfo[]; } MaterialList;
layout(binding = 6) uniform sampler2D TextureMap[];
//layout(binding = 8) readonly buffer _TexCoordBuf {float texcoord0[];};


layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec4 outColor;

void main() {

//	vec3 lightDir = normalize(-scenedata.dlight.direction);
//	float diff = max(dot(scenedata.dlight.direction, lightDir), 0.0);
//
//	vec3 ambient = scenedata.dlight.ambient *  vec3(0.7f);
//    vec3 diffuse = scenedata.dlight.diffuse * diff *   vec3(0.7f);
// 
// 	vec3 halfwayDir = normalize(scenedata.dlight.direction + scenedata.viewPos);  
//    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0f);
//    vec3 specular = scenedata.dlight.specular * spec * 1.0f;
		
//	vec3 color = vec3(1.0f);
//	if(Mesh.MeshID == 0)
//	{
//		color = vec3(1.0f, 0.0f, 0.0f);
//	}
//	if(Mesh.MeshID == 1)
//	{
//		color = vec3(0.0f, 1.0f, 0.0f);
//	}
//	if(Mesh.MeshID == 2)
//	{
//		color = vec3(0.0f, 0.0f, 1.0f);
//	}
//	 vec3 lightVector = normalize(scenedata.dlight.direction);
//	float dot_product = max(dot(lightVector, Normal), 0.2);
//	outColor = vec4(color * dot_product, 1.0f);

	 outColor = texture(TextureMap[Mesh.MaterialID], UV);
}