#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 11) uniform mat4 view;
layout(location = 12) uniform mat4 perspective;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main() {
	mat4 tmpView = mat4(mat3(view));

	gl_Position = (perspective * tmpView * vec4(position, 1.0f)).xyww;

	fragPos = vec3(tmpView * vec4(position, 1));
	fragNormal = vec3(tmpView * vec4(normal, 0));
	fragTexCoord = texCoord;
}
