#version 450 core

layout (location = 0) in vec3 color;
layout (location = 1) in vec2 fragoffset;

layout (location = 0) out vec4 fragColor;
layout (location = 5) out vec4 isLighted;

void main(){
    float dist = sqrt(dot(fragoffset, fragoffset));
    if (dist >= 1.0)
        discard;
    float cosdist = 0.5*(cos(3.14159*dist) + 1.0);
    fragColor = vec4(color.xyz + 0.5*cosdist, cosdist);
    isLighted = vec4(0.0, 1.0, 0.0, 0.0);
}