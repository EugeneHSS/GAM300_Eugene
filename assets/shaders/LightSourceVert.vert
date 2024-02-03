#version 450

const vec2 OFFSET[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0), 
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0), 
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0)); 


layout (location = 0) out vec2 fragoffset;
layout (location = 1) out vec4 fragcolor;
layout(set = 0, binding = 5) uniform SceneUBO
{
    mat4 proj;
    mat4 view;
}sceneubo;

struct LightSourceProperties
{
    vec4 Position;
    vec4 Color;
    float Radius;
    float padding[3];
};

layout(std140, binding = 11) readonly buffer PointLightBuffer
{   
	LightSourceProperties lightSource[];
};


void main() 
{
    const LightSourceProperties lightData = lightSource[gl_InstanceIndex];
    fragoffset = OFFSET[gl_VertexIndex];
    vec3 WorldCameraRight = {sceneubo.view[0][0], sceneubo.view[1][0], sceneubo.view[2][0]};
    vec3 WorldCameraUp = {sceneubo.view[0][1], sceneubo.view[1][1], sceneubo.view[2][1]};

    vec3 worldPos = lightData.Position.xyz + 
    lightData.Radius * fragoffset.x * WorldCameraRight+ 
    lightData.Radius* fragoffset.y * WorldCameraUp;
    fragcolor = lightData.Color;
    gl_Position = sceneubo.proj * sceneubo.view* vec4(worldPos, 1.0);
}
