#version 450

layout (binding = 1) uniform samplerCube samplerCubeMap;

layout (location = 0) in vec3 inUVW;

layout (location = 0) out vec4 outFragColor;
layout (location = 5) out vec4 isLighted;

void main() 
{
	
	outFragColor = texture(samplerCubeMap, inUVW);
	isLighted = vec4(1.0, 0.0, 0.0, 0.0);
}