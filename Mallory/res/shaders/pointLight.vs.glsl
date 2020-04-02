#version 410

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormals;
layout(location = 2) in vec2 inUV;



layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec3 outNormals;

uniform sampler2D s_Albedo;


int main(){
	outAlbedo = vec4(texture(s_Albedo, inUV).rgb * inColor.rgb, inColor.a);
	vec3 norm = normalize(inNormal);
	outNormal = (norm / 2.0) + vec3(0.5);

}
