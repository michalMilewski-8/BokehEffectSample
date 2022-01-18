#version 430 core

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 20) uniform sampler2D colorTexture;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 color;

void main() {
	position = fragPos;
	normal = fragNormal;
	color = texture(colorTexture, fragTexCoord).rgb;
}

