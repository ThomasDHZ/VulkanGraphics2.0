#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_multiview : enable

#include "SceneProperties.glsl"
#include "material.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 1) buffer MeshProperties 
{
	mat4 ModelTransform;
	vec2 UVOffset;
    vec2 UVScale;
    vec2 UVFlip;
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer Material { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap;

layout(binding = 14) uniform CubeSampler 
{
    mat4 lightSpaceMatrix[6];
} ReflectionSamples;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 Tangent;
layout(location = 4) out vec3 BiTangent;
layout(location = 5) out vec4 LightSpace;

    const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() 
{
//    if(gl_VertexIndex == 0)
//	{
//        if(gl_ViewIndex == 0)
//        {
//		    debugPrintfEXT("Reflection00: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][0][0]);
//             debugPrintfEXT("Reflection01: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][0][1]);
//              debugPrintfEXT("Reflection02: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][0][2]);
//               debugPrintfEXT("Reflection03: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][0][3]);
//
//                debugPrintfEXT("Reflection10: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][1][0]);
//                 debugPrintfEXT("Reflection11: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][1][1]);
//                  debugPrintfEXT("Reflection12: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][1][2]);
//                   debugPrintfEXT("Reflection13: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][1][3]);
//
//                    debugPrintfEXT("Reflection20: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][2][0]);
//                     debugPrintfEXT("Reflection21: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][2][1]);
//                      debugPrintfEXT("Reflection22: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][2][2]);
//                       debugPrintfEXT("Reflection23: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][2][3]);
//
//                        debugPrintfEXT("Reflection30: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][3][0]);
//                         debugPrintfEXT("Reflection31: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][3][1]);
//                          debugPrintfEXT("Reflection32: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][3][2]);
//                           debugPrintfEXT("Reflection33: %f \n", ReflectionSamples.lightSpaceMatrix[gl_ViewIndex][3][3]);
//        }
//	}

    FragPos = vec3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(inPosition, 1.0));    
    TexCoords = aTexCoords;
    Normal = aNormal;
	Tangent = aTangent.rgb;
	BiTangent = aBitangent.rgb;
    LightSpace = (biasMat * sceneBuffer.sceneData.lightSpaceMatrix * meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec4(FragPos, 1.0);
    gl_Position =  ReflectionSamples.lightSpaceMatrix[gl_ViewIndex] * meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(inPosition, 1.0);

}