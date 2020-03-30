#version 440

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec2 inScreenCoords;

layout (location = 0) out vec4 outColor;

uniform sampler2D xImage;


void main() {
	vec4 color = texture(xImage, inUV);	
	outColor = vec4(1 - color.rgb, color.a);

	vec3 hello = outColor.rgb;

	float average = (hello.r + hello.b + hello.g)/3.0f;
	if (average < 0.2) {
		average = 0.0;
	}
	hello.r = average;
	hello.b = average;
	hello.g = average;
	//outColor = vec4(hello, color.a);

}