#version 410

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 5) in vec2 inUV;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outWorldPos;
layout (location = 3) out vec2 outUV;

uniform mat4 a_ModelViewProjection;
uniform mat4 a_Model;
uniform mat4 a_View;
uniform mat3 a_NormalMatrix;
uniform float a_Time;

void main() {
	outColor = inColor;
	outNormal = a_NormalMatrix * inNormal;
	outColor = inColor;
	outWorldPos =  (a_Model * vec4(inPosition, 1)).xyz;
	vec3 displace = inPosition;
	
		displace.y = sin(a_Time) * displace.y;
		displace.y = max(displace.y, -4.0f);
		displace.y = min(4.0f, displace.y);
	

	gl_Position = a_ModelViewProjection * vec4(displace, 1);

	// New in tutorial 06
	outUV = inUV;
}