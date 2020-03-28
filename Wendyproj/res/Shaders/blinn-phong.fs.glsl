#version 410

//Non-Multiple Lights

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inWorldPos;
layout (location = 3) in vec2 inUV;

layout (location = 0) out vec4 outColor;



uniform vec3  a_CameraPos;

uniform vec3  a_AmbientColor;
uniform float a_AmbientPower;

uniform vec3  a_LightPos;
uniform vec3  a_LightColor;
uniform float a_LightShininess;
uniform float a_LightAttenuation;

uniform sampler2D s_Albedo;

// Toon
const int levels = 2;
const float scaleFactor = 1.0/levels;

void main() {
	
    // Re-normalize our input, so that it is always length 1
    vec3 norm = normalize(inNormal);
    // Determine the direction from the position to the light
    vec3 toLight = (a_LightPos - inWorldPos);
    // Determine the distance to the light (used for attenuation later)
    float distToLight = length(toLight);
    // Normalize our toLight vector
    toLight = normalize(toLight);

    // Determine the direction between the camera and the pixel
    vec3 viewDir = normalize(a_CameraPos - inWorldPos);

    // Calculate the halfway vector between the direction to the light and the direction to the eye
    vec3 halfDir = normalize(toLight + viewDir);

    // Our specular power is the angle between the the normal and the half vector, raised
    // to the power of the light's shininess
    float specPower = pow(max(dot(norm, halfDir), 0.0), a_LightShininess);

    // Finally, we can calculate the actual specular factor
    vec3 specOut = specPower * a_LightColor;

    // Calculate our diffuse factor, this is essentially the angle between
    // the surface and the light
    float diffuseFactor = max(dot(norm, toLight), 0);
    // Calculate our diffuse output
    vec3  diffuseOut = diffuseFactor * a_LightColor;
	
	//Toon Shading
	//diffuseOut = floor(diffuseOut * levels) * scaleFactor;

    // Our ambient is simply the color times the ambient power
    vec3 ambientOut = a_AmbientColor * a_AmbientPower;

    // We will use a modified form of distance squared attenuation, which will avoid divide
    // by zero errors and allow us to control the light's attenuation via a uniform
    float attenuation = 1.0 / (1.0 + a_LightAttenuation * pow(distToLight, 2));
    

	 // Below is modified for tutorial 08    
	 vec4 albedo = texture(s_Albedo, inUV);

	// Our result is our lighting multiplied by our object's color    
	vec3 result = (ambientOut + attenuation * (diffuseOut + specOut)) * albedo.xyz * inColor.xyz;

	// TODO: gamma correction
	// Write the output   
	outColor = vec4(result, inColor.a * albedo.a); // * a_ColorMultiplier;
	
	vec3 hello = outColor.rgb;
	float average = (hello.r + hello.b + hello.g)/3.0f;
	if (average < 0.2) {
		average = 0.0;
	}
	hello.r = average;
	hello.b = average;
	hello.g = average;
	//outColor = vec4(hello, inColor.a * albedo.a);

	//0, 0, 1
	//if ((inNormal.x * 0 + inNormal.y * 1 + inNormal.z * 1) <= 0.1 && (inNormal.x * 0 + inNormal.y * 1 + inNormal.z * 1) >= -0.1) {
	//		outColor.r = 1.0f;
	//		outColor[1] = 1.0f;
	//		//inColor = 0.0f;
	//}
}


//#version 410
//
////Multiple lights
//
//layout (location = 0) in vec4 inColor;
//layout (location = 1) in vec3 inNormal;
//layout (location = 2) in vec3 inWorldPos;
//layout (location = 3) in vec2 inUV;
//
//layout (location = 0) out vec4 outColor;
//
//
//
//uniform vec3  a_CameraPos;
//
//uniform vec3  a_AmbientColor;
//uniform float a_AmbientPower;
//
//uniform vec3  a_LightPos;
//uniform vec3  a_LightColor;
//uniform float a_LightShininess;
//uniform float a_LightAttenuation;
//
//const int MAX_LIGHTS = 16;
//struct Light{
//	vec3  Pos;
//	vec3  Color;
//	float Attenuation;
//};
//uniform Light a_Lights[MAX_LIGHTS];
//uniform int a_EnabledLights;
//
//uniform sampler2D s_Albedo;
//
//vec3 calculateLights(Light light, vec3 norm) {
//	 // Determine the direction from the position to the light
//    vec3 toLight = light.Pos - inWorldPos;
//
//    // Determine the distance to the light (used for attenuation later)
//    float distToLight = length(toLight);
//    // Normalize our toLight vector
//    toLight = normalize(toLight);
//
//    // Determine the direction between the camera and the pixel
//    vec3 viewDir = normalize(a_CameraPos - inWorldPos);
//
//    // Calculate the halfway vector between the direction to the light and the direction to the eye
//    vec3 halfDir = normalize(toLight + viewDir);
//
//    // Our specular power is the angle between the the normal and the half vector, raised
//    // to the power of the light's shininess
//    float specPower = pow(max(dot(norm, halfDir), 0.0), a_LightShininess);
//
//    // Finally, we can calculate the actual specular factor
//    vec3 specOut = specPower * a_LightColor;
//
//	// Calculate our diffuse factor, this is essentially the angle between
//    // the surface and the light
//    float diffuseFactor = max(dot(norm, toLight), 0);
//    // Calculate our diffuse output
//    vec3  diffuseOut = diffuseFactor * light.Color;
//
//	// We will use a modified form of distance squared attenuation, which will avoid divide
//    // by zero errors and allow us to control the light's attenuation via a uniform
//    float attenuation = 1.0 / (1.0 + light.Attenuation * pow(distToLight, 2));
//    
//	return attenuation * (diffuseOut + specOut);
//}
//
//void main() {
//	
//    // Re-normalize our input, so that it is always length 1
//    vec3 norm = normalize(inNormal);
//       
//    // Our ambient is simply the color times the ambient power
//    vec3 result = a_AmbientColor * a_AmbientPower;
//
//    for (int i = 0; (i < a_EnabledLights) && (i < MAX_LIGHTS); i++) {
//		result += ResolvePointLight(a_Lights[i], norm);
//	}
//
//	// Below is modified for tutorial 08    
//	vec4 albedo = texture(s_Albedo, inUV);
//
//	// Our result is our lighting multiplied by our object's color    
//	vec3 result = result * albedo.xyz; //* inColor.xyz;
//
//	// TODO: gamma correction
//	// Write the output   
//	
//	outColor = vec4(result, inColor.a * albedo.a); // * a_ColorMultiplier;
//}