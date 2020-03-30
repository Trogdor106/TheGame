#version 410

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;

layout (location = 4) in vec3 inGoalPosition;
layout (location = 5) in vec4 inGoalColor;
layout (location = 6) in vec3 inGoalNormal;
layout (location = 7) in vec2 inGoalUV;

uniform float time;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outWorldPos;
layout (location = 3) out vec2 outUV;

uniform mat4 a_ModelViewProjection;
uniform mat4 a_Model;
uniform mat4 a_ModelView;
uniform mat3 a_NormalMatrix;

void main() {
	vec3 v = mix(inPosition, inGoalPosition, time);
	//if (v.x >= inGoalPosition.x) {
	//	v = inGoalPosition;
	//}
	//vec4 c = mix(inColor, inGoalColor, time);
	//vec3 n = mix(inNormal, inGoalColor, time);
	//vec2 UV = mix(inUV, inGoalUV, time);
	//
	//outColor = c;
	//outNormal = a_NormalMatrix * n;
	//outWorldPos = (a_Model * vec4(v, 1)).xyz;
	//gl_Position = a_ModelViewProjection * vec4(v, 1);
	//outUV = UV;

	outColor = inColor;
	outNormal = a_NormalMatrix * inNormal;
	outColor = inColor;
	
	//outWorldPos =  (a_Model * vec4(inPosition, 1)).xyz;
	vec3 v2 = inPosition;
	//if (mod(floor(time), 2.0) != 0.0){
	//	v2.x = inGoalPosition.x;
	//	v2.y = inGoalPosition.y;
	//	v2.z = inGoalPosition.z;
	//}
	//v2.y = v2.y * sin(time);
	
	v2.x = v2.x + (inGoalPosition.x / 30) * time; 
	v2.y = v2.y + (inGoalPosition.y / 30) * time;
	v2.z = v2.z + (inGoalPosition.z / 30) * time;

	//if (abs(v2.x) > abs(inGoalPosition.x)){
	//	v2.x = (inGoalPosition.x);
	//	v2.y = (inGoalPosition.y);
	//	v2.z = (inGoalPosition.z);
	//}
	
	outWorldPos = (a_Model * vec4(v, 1)).xyz;
	gl_Position = a_ModelViewProjection * vec4(v, 1);
	outUV = inUV;

}