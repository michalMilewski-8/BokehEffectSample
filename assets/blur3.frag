#version 420 core

#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_separate_shader_objects : require
const int N = 144;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D colorTexture;
layout(binding = 1) uniform sampler2D depthTexture;
layout(binding = 2) uniform sampler2D colorTextureB;

layout(location = 1) uniform vec2 offset[N] ;

const float bleedingBias = 0.02f;
const float bleedingMult = 30.0f;

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
	
	vec4 centerPixel = texture(colorTexture, fragTexCoord);
	float centerDepth = texture(depthTexture, fragTexCoord).r;

	vec4 finalColor = vec4(0.0f,0.0f,0.0f,0.0f);
	float totalWeight = 0.0f;

	for(int i =0 ;i<N;i++) {
		vec2 offset = offset[i];
		vec2 sampleCoord = fragTexCoord + offset * centerPixel.a;
		vec4 samplePixel = texture(colorTexture, sampleCoord);
		float sampleDepth = texture(depthTexture, sampleCoord).r;
		
		float weight = sampleDepth < centerDepth ? samplePixel.a * bleedingMult : 1.0f;
		weight = (centerPixel.a > samplePixel.a + bleedingBias) ? weight : 1.0f;
		weight = clamp(weight,0.0f,1.0f);

		finalColor+=samplePixel*weight;
		totalWeight += weight;
	}

	vec4 rescolor = finalColor/totalWeight;

	color = min(rescolor,texture(colorTextureB, fragTexCoord));

	color = vec4(lin2srgb(color.rgb),1.0f);
//	color = rescolor;
//	color = vec4(centerDepth,centerDepth,centerDepth,1.0f);
}
