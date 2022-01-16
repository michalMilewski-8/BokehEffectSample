#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec2 fragTexCoord;

layout(location = 20) uniform sampler2D positionTexture;
layout(location = 21) uniform sampler2D normalTexture;
layout(location = 22) uniform sampler2D colorTexture;
layout(location = 23) uniform sampler2D emissionTexture;

layout(location = 0) out vec4 color;

void main() {
	vec3 position = texture(positionTexture, fragTexCoord).xyz;
	vec3 normal = texture(normalTexture, fragTexCoord).xyz;
	vec3 albedo = texture(colorTexture, fragTexCoord).rgb;
	vec3 emission = texture(emissionTexture, fragTexCoord).rgb;

	normal = normalize(normal);
	vec3 toCamera = -normalize(position);
	float a = dot(normal, toCamera);
	if (isnan(a)) a = 0;
	color = vec4(a * albedo + emission, 1);
}

