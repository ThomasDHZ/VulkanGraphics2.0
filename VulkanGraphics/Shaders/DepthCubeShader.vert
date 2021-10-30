#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_multiview : enable

#include "SceneProperties.glsl"
#include "MeshProperties.glsl"

layout(push_constant) uniform LightSceneInfo
{
	uint MeshIndex;
    mat4 lightSpaceMatrix;
} scene;

layout(binding = 0) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 1) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];
layout(binding = 2) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 3) uniform sampler2D TextureMap[];
layout(binding = 4) uniform CubeSampler 
{
    mat4 lightSpaceMatrix[6];
} SkyboxSamples;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

void main() 
{
//    if(gl_VertexIndex == 0)
//	{
//        if(gl_ViewIndex == 0)
//        {
//		    debugPrintfEXT("CubeDepth00: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][0][0]);
//             debugPrintfEXT("CubeDepth01: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][0][1]);
//              debugPrintfEXT("CubeDepth02: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][0][2]);
//               debugPrintfEXT("CubeDepth03: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][0][3]);
//
//                debugPrintfEXT("CubeDepth10: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][1][0]);
//                 debugPrintfEXT("CubeDepth11: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][1][1]);
//                  debugPrintfEXT("CubeDepth12: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][1][2]);
//                   debugPrintfEXT("CubeDepth13: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][1][3]);
//
//                    debugPrintfEXT("CubeDepth20: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][2][0]);
//                     debugPrintfEXT("CubeDepth21: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][2][1]);
//                      debugPrintfEXT("CubeDepth22: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][2][2]);
//                       debugPrintfEXT("CubeDepth23: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][2][3]);
//
//                        debugPrintfEXT("CubeDepth30: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][3][0]);
//                         debugPrintfEXT("CubeDepth31: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][3][1]);
//                          debugPrintfEXT("CubeDepth32: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][3][2]);
//                           debugPrintfEXT("CubeDepth33: %f \n", SkyboxSamples.lightSpaceMatrix[gl_ViewIndex][3][3]);
//        }
//	}
    gl_Position = SkyboxSamples.lightSpaceMatrix[gl_ViewIndex] * meshBuffer[scene.MeshIndex].meshProperties.ModelTransform * meshBuffer[scene.MeshIndex].meshProperties.MeshTransform * vec4(inPosition, 1.0);
}