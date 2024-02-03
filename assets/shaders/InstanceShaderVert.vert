#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vBiTangent;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vNormals;
layout(location = 4) in vec2 vUV;
layout(location = 5) in vec4 vColor;
layout(location = 6) in vec4 BoneIDs;
layout(location = 7) in vec4 Weights;
layout(location = 8) in vec2 meshID;

/*
    struct PointLight{
        vec4 Position;
        vec4 Color;
        vec4 pad[2];
    };
*/
//for the scene
layout(set = 0, binding = 5) uniform SceneUBO
{
    mat4 proj;
    mat4 view;
}Scene3D;

struct InstanceData
{
    uint matID;
    uint textureID;
    uint isAnimated;
    uint entityID;
    mat4 modelMatrix;
};

layout(std140, binding = 15) readonly buffer InstanceBuffer
{   
	InstanceData instances[];
};

// for phong bling bling
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

layout(push_constant) uniform ConstantData
{
	uint offset;
};

/*
    layout(std140, binding = 16) readonly buffer MaterialBuffer
    {   
        Material materials[];
    };
*/

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPosWorld;
layout(location = 3) out vec3 fragNormalWorld;
layout(location = 4) out flat uint id;
layout(location = 5) out vec4 clipspacepos;
layout(location = 6) out flat uint texID;
layout(location = 7) out float linearDepth;

void main() 
{
    //Common data in this and the batch version, but this is instancing so we are not using this

    InstanceData instance = instances[gl_InstanceIndex + offset + uint(meshID.x)];
    uint materialID = instance.matID;
    uint textureID = instance.textureID;
    uint animatedMesh = instance.isAnimated;
    mat4 modelMatrix = instance.modelMatrix;

    mat4 skinMat = mat4(1.0);

    if (animatedMesh != 0)
    {
        //Empty now, but xing xiang, put ur animation here and update the skinMat
    }

    mat4 accumulated = modelMatrix * skinMat;
    
    vec4 position_in_world = accumulated * vec4(vPosition, 1.0);
    fragTexCoord = vUV;
    
    id = instance.entityID;

    vec4 clipspacepos = Scene3D.proj * Scene3D.view * position_in_world;
    gl_Position = clipspacepos;
    
    vec3 normal = mat3(transpose(inverse(accumulated))) * vNormals;
    vec3 tangent = mat3(accumulated) * vTangent;
    vec3 bitangent = mat3(accumulated) * vBiTangent;
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    texID = textureID;
    fragNormalWorld = normal;
    fragPosWorld = position_in_world.xyz;

    fragColor = vColor;
   
    linearDepth = -(Scene3D.view * position_in_world).z;
}