#version 430 core

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 20) uniform sampler2D colorTexture;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 color;

vec3 lin2srgb( vec3 cl )
{
	//cl = clamp( cl, 0.0, 1.0 );
	vec3 c_lo = 12.92 * cl;
	vec3 c_hi = 1.055 * pow(cl,vec3(0.41666)) - 0.055;
	vec3 s = step( vec3(0.0031308), cl);
	return mix( c_lo, c_hi, s );
}

vec3 srgb2lin( vec3 cs )
{
	vec3 c_lo = cs / 12.92;
	vec3 c_hi = pow( (cs + 0.055) / 1.055, vec3(2.4) );
	vec3 s = step(vec3(0.04045), cs);
	return mix( c_lo, c_hi, s );
}

void main() {
	position = fragPos;
	normal = fragNormal;
	color = srgb2lin(texture(colorTexture, fragTexCoord).rgb).rgb ;
}

