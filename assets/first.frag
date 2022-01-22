#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 viewV;
layout(location = 4) in vec3 worldPos;

layout (location = 20) uniform sampler2D colorTexture;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;

const vec4 lightPos = vec4(3.0f, 0.0f, 0.0f, 1.0f);
const vec3 lightColor = vec3(1, 1, 1);

void main() {
	position = fragPos;
	normal = fragNormal;

	vec3 objectColor = texture(colorTexture, fragTexCoord).rgb;
	// ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    float diffStrength = 0.7;
    vec3 lightDir = normalize(lightPos.xyz - worldPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffStrength * diff * lightColor;
    
    // specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewV);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = result;
}

