#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 10) uniform mat4 model;
layout(location = 11) uniform mat4 view;
layout(location = 12) uniform mat4 perspective;
layout(location = 13) uniform mat4 invView;
layout(location = 14) uniform mat4 invModel;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 viewV;

void main() {
	gl_Position = perspective * view * model * vec4(position, 1.0);
	fragPos = vec3(view * model * vec4(position, 1));
	fragNormal = vec3(invModel* vec4(normal, 0));
	fragTexCoord = texCoord;

	vec3 worldPos =  (model * vec4(position, 1.0)).xyz;
	vec3 camPos = (invView* vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	viewV = camPos - worldPos;
}
