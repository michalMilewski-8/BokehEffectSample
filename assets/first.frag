#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 newPosition;
layout(location = 4) in vec4 oldPosition;

layout (location = 20) uniform sampler2D colorTexture;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;
layout (location = 3) out vec3 emission;
layout (location = 4) out vec2 velocity;

void main() {
	position = fragPos;
	normal = fragNormal;
	color = texture(colorTexture, fragTexCoord).rgb;
	emission = vec3(0, 0, 0);

	vec2 oldPos = oldPosition.xy / oldPosition.w;
	vec2 newPos = newPosition.xy / newPosition.w;

	velocity = newPos - oldPos;
}

