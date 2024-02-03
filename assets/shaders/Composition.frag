#version 450
layout(location = 0) in vec2 inTexCoord;   

layout (binding = 2) uniform sampler2D colorTexture;

layout(location = 0) out vec4 outColor;
void main()
{
    outColor = texture(colorTexture, inTexCoord);
}
