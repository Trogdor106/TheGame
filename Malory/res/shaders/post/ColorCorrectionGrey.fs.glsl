#version 440

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec2 inScreenCoords;

layout (location = 0) out vec4 outColor;

uniform sampler2D xImage;

void main() {
	vec4 color = texture(xImage, inUV);	
	
	vec3 grey = color.rgb;

	float average = (grey.r + grey.b + grey.g)/3.0f;
	
	grey.r = average;
	grey.b = average;
	grey.g = average;


	outColor = vec4(grey, color.a);

}