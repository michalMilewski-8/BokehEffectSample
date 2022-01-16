#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 11) uniform mat4 view;
layout(location = 12) uniform mat4 perspective;
layout(location = 13) uniform mat4 oldView;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

layout(location = 3) out vec4 newPosition;
layout(location = 4) out vec4 oldPosition;

void main() {
	mat4 tmpView = mat4(mat3(view));
	mat4 oldTmpView = mat4(mat3(oldView));

	newPosition = (perspective * tmpView * vec4(position, 1.0f)).xyww;
	oldPosition = (perspective * oldTmpView * vec4(position, 1.0f)).xyww;

	gl_Position = newPosition;

	fragPos = vec3(tmpView * vec4(position, 1));
	fragNormal = vec3(tmpView * vec4(normal, 0));
	fragTexCoord = texCoord;
}