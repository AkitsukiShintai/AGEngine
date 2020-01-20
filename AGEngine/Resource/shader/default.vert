#version 450
#extension GL_ARB_separate_shader_objects : enable
#include <camera.glsl>
#pragma pervertex
in vec3 worldPos;

layout(binding = 0) uniform CAMERA_BUFFER
{
    Camera camera;
};

void main(){
	gl_Position = camera.projectionMatrix * camera.viewMatrix * vec4(worldPos,1.0f);

}