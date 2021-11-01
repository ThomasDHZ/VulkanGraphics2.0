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
    mat4 lightSpaceMatrix[8];
} lightSamplers;

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
//		    debugPrintfEXT("Reflection00: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][0][0]);
//             debugPrintfEXT("Reflection01: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][0][1]);
//              debugPrintfEXT("Reflection02: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][0][2]);
//               debugPrintfEXT("Reflection03: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][0][3]);
//
//                debugPrintfEXT("Reflection10: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][1][0]);
//                 debugPrintfEXT("Reflection11: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][1][1]);
//                  debugPrintfEXT("Reflection12: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][1][2]);
//                   debugPrintfEXT("Reflection13: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][1][3]);
//
//                    debugPrintfEXT("Reflection20: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][2][0]);
//                     debugPrintfEXT("Reflection21: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][2][1]);
//                      debugPrintfEXT("Reflection22: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][2][2]);
//                       debugPrintfEXT("Reflection23: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][2][3]);
//
//                        debugPrintfEXT("Reflection30: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][3][0]);
//                         debugPrintfEXT("Reflection31: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][3][1]);
//                          debugPrintfEXT("Reflection32: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][3][2]);
//                           debugPrintfEXT("Reflection33: %f \n", lightSamplers.lightSpaceMatrix[gl_ViewIndex][3][3]);
//        }
//	}

    gl_Position = lightSamplers.lightSpaceMatrix[gl_ViewIndex] * meshBuffer[scene.MeshIndex].meshProperties.ModelTransform * meshBuffer[scene.MeshIndex].meshProperties.MeshTransform * vec4(inPosition, 1.0);
}