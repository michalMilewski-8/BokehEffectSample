#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec2 fragTexCoord;

void main() {
	gl_Position = vec4(position, 1);
	fragTexCoord = texCoord;
}
