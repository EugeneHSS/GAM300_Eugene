#version 450
layout(location = 0) in vec2 inTexCoord;   

layout (binding = 2) uniform sampler2D colorTexture;
layout (binding = 3) uniform sampler2D conditionalGamma;

layout (push_constant) uniform FadeFactor
{
    float fadeFactor;
} fade;

layout(location = 0) out vec4 outColor;
void main()
{
    vec4 texColor = texture(colorTexture, inTexCoord);
    vec4 ToNotBeAffectedByGamma = texture(conditionalGamma, inTexCoord);

    if (ToNotBeAffectedByGamma.y == 1)
    {
        outColor = texColor;
    }
    else
    {
        outColor = mix(vec4(0.0, 0.0, 0.0, texColor.a), texColor, fade.fadeFactor);
    }

}
