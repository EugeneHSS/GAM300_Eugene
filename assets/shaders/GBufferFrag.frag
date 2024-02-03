
#version 450

//Preparing for materials
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    float reflectivity;
    float opacity;
    uint diffuseID;
    
    uint specularID;
    uint normalID;
    uint emissiveID;
    uint opacityID;
 
};

layout(set = 1, binding = 4) uniform sampler2D texArraySampler[500];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosWorld;
layout(location = 3) in vec3 fragNormalWorld; 
layout(location = 4) in flat uint id;
layout(location = 5) in vec4 clipspacepos;
layout(location = 6) flat in uint texID;
layout(location = 7) in float linearDepth;


layout (location = 0) out vec4 outColor;
layout (location = 1) out uint outID;
layout (location = 2) out vec4 outPositions;
layout (location = 3) out vec4 outNormals;

void main() 
{
    
    int textureID = int(texID);
    outID = id;
    outColor = texture(texArraySampler[textureID], fragTexCoord);
    outPositions = vec4(fragPosWorld, 1.0);
    outNormals = vec4(fragNormalWorld, 1.0);
}