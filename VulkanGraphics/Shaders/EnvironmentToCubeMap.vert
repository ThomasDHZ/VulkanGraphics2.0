  #version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview : enable

mat4 MVP[6] = {{{0.000000, 0.000000, 1.010101, 1.000000},
                       {0.000000, -1.000000, 0.000000, 0.000000},
                       {-1.000000, 0.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, -0.101010, 0.000000}},
                      {{0.000000, 0.000000, -1.010101, -1.000000},
                       {0.000000, -1.000000, 0.000000, 0.000000},
                       {1.000000, 0.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, -0.101010, 0.000000}},
                      {{1.000000, 0.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, 1.010101, 1.000000},
                       {0.000000, 1.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, -0.101010, 0.000000}},
                      {{1.000000, 0.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, -1.010101, -1.000000},
                       {0.000000, -1.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, -0.101010, 0.000000}},
                      {{1.000000, 0.000000, 0.000000, 0.000000},
                       {0.000000, -1.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, 1.010101, 1.000000},
                       {0.000000, 0.000000, -0.101010, 0.000000}},
                      {{-1.000000, 0.000000, 0.000000, 0.000000},
                       {0.000000, -1.000000, 0.000000, 0.000000},
                       {0.000000, 0.000000, -1.010101, -1.000000},
                       {0.000000, 0.000000, -0.101010, 0.000000}}};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;
layout (location = 5) in vec4 aColor;

layout(location = 0) out vec3 WorldPos;

void main() {
	WorldPos = aPos;
	vec4 pos = MVP[gl_ViewIndex] * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
