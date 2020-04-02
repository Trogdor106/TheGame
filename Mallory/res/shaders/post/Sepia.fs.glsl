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

	//outputRed = (inputRed * .393) + (inputGreen *.769) + (inputBlue * .189)
	//outputGreen = (inputRed * .349) + (inputGreen *.686) + (inputBlue * .168)
	//outputBlue = (inputRed * .272) + (inputGreen *.534) + (inputBlue * .131)

	grey.r = (grey.r * 0.393f) + (grey.g * 0.769f) + (grey.b * 0.189);
	grey.g = (grey.r * 0.349f) + (grey.g * 0.686f) + (grey.b * 0.168);
	grey.b = (grey.r * 0.272f) + (grey.g * 0.534f) + (grey.b * 0.131);

	if (grey.r > 255.0f)	{
		grey.r = 255.0f;
	}
	if (grey.b > 255.0f)	{
		grey.b = 255.0f;
	}
	if (grey.g > 255.0f)	{
		grey.g = 255.0f;
	}

	outColor = vec4(grey, color.a);

}