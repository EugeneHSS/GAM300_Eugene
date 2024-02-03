#version 450

layout(location = 0) in vec2 fragoffset;
layout(location = 1) in vec4 fragcolor;

layout(location = 0) out vec4 OutColor;

void main() 
{
    float dis = sqrt(dot(fragoffset, fragoffset));
    if (dis >= 1.0){
        discard;
    }
    float cosDis = 0.5* (cos(dis* 3.1415926538) +1.0);
    OutColor = vec4(fragcolor.xyz +0.5* cosDis, cosDis);
}