#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 viewV;

layout (location = 20) uniform sampler2D colorTexture;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;

const vec4 lightPos = vec4(0.0f, 0.0f, 10.0f, 1.0f);
const vec3 lightColor = vec3(1, 1, 1);

void main() {
	position = fragPos;
	normal = fragNormal;
	//color = texture(colorTexture, fragTexCoord).rgb;

	 vec3 objectColor = texture(colorTexture, fragTexCoord).rgb;
	// ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    float diffStrength = 0.3;
    vec3 lightDir = normalize(lightPos.xyz - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffStrength * diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewV);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 12);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = objectColor;
  
  
  //  color = vec3(fragTexCoord,0.0);
//    FragColor = vec4(objectColor, 1.0);
}

