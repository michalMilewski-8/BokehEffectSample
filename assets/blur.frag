#version 420 core

#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D colorTexture;
layout(binding = 1) uniform sampler2D depthTexture;

layout(location = 2) uniform float A;
layout(location = 3) uniform float S1;
layout(location = 4) uniform float f;
layout(location = 5) uniform float Far;
layout(location = 6) uniform float maxCoc;
layout(location = 7) uniform float sensorHeight;

void main() {
	color = texture(colorTexture, fragTexCoord);
	float depth = texture(depthTexture, fragTexCoord).r;
	float S2 = depth* Far;
	float c = A*(abs(S2-S1)/S2) * (f/(S1-f));
	color.a = clamp(c/sensorHeight,0.0f,maxCoc);

}
