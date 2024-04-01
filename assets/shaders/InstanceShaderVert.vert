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


//for the scene
layout(set = 0, binding = 5) uniform SceneUBO
{
    mat4 proj;
    mat4 view;
}Scene3D;




struct Material
{
    //Phong Bling
    vec3 diffuse;
    int diffuseTex;

    vec3 specular;
    int specularTex;

    vec3 ambient;
    int ambientTex;
    
    vec3 emissive;
    int emissiveTex;
    
    
    int shininessTex;
    float shininess;
    float shininessStrength;
    float reflectivity;

    //General Material
    vec3 reflectance; 
    int reflectanceTex;

    //PBR 
   // int normalTex;
   // int RoughnessTex;
   // int MetallicTex;
    //int aoTex;

   // float metalness;
    //float roughness;
    //float emissiveFactor;
   // int  albedoTex;

    int  MaterialID;
    int  ShadingModel;
    int  UseMaterialTextures;
    int  UseMaterialNormal;
};

layout(push_constant) uniform ConstantData
{
	uint offset;
};

layout(std140, binding = 16) readonly buffer MaterialBuffer
{
    Material material[];
}materialData;


Material GetMaterial(uint MaterialID)
{
    for (uint i = 0; i < materialData.material.length(); i++)
    {
        if (materialData.material[i].MaterialID == MaterialID)
        {
            return materialData.material[i];
        }
    }
    
}

struct InstanceData
{
    mat4 modelMatrix;
    int matID;
    uint textureID;
    uint isRender;
    uint entityID;

    uint m_AnimOffset;
    uint m_IsAnimated;
    uint m_UseMeshMatID;
    uint m_UseMaterials;

    Material m_Material;
};

layout(std140, binding = 15) readonly buffer InstanceBuffer
{   
	InstanceData instances[];
};


layout(std140, binding = 19) readonly buffer boneView
{
    mat4 BoneMatrices[];
}bones;




layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPosWorld;
layout(location = 3) out vec3 fragNormalWorld;
layout(location = 4) out flat uint id;
layout(location = 5) out vec4 clipspacepos;
layout(location = 6) out flat uint UseMaterials;
layout(location = 7) out flat uint texID;
layout(location = 8) out float linearDepth;
layout(location = 9) out uint isRenderable;
layout(location = 10) out mat3 TBN;
layout(location = 13) out Material vMaterial;

void main() 
{
    //Common data in this and the batch version, but this is instancing so we are not using this

    
    InstanceData instance = instances[gl_InstanceIndex + offset + uint(meshID.x)];
    uint textureID = instance.textureID;
    mat4 modelMatrix = instance.modelMatrix;

    mat4 skinMat = mat4(1.0);

    if (instance.m_IsAnimated == 1)
    {
        skinMat = 
		Weights.x * bones.BoneMatrices[instance.m_AnimOffset + uint(BoneIDs.x)] +
		Weights.y * bones.BoneMatrices[instance.m_AnimOffset + uint(BoneIDs.y)] +
		Weights.z * bones.BoneMatrices[instance.m_AnimOffset + uint(BoneIDs.z)] +
		Weights.w * bones.BoneMatrices[instance.m_AnimOffset + uint(BoneIDs.w)];
    }


    mat4 accumulated = modelMatrix * skinMat;
    
    vec4 position_in_world = accumulated * vec4(vPosition, 1.0);
    fragTexCoord = vUV;
    
    id = instance.entityID;

    vec4 clipspacepos = Scene3D.proj * Scene3D.view * position_in_world;
    gl_Position = clipspacepos;
    
    TBN = mat3(vBiTangent, vTangent, vNormals);
    
    texID = textureID;
    fragNormalWorld = vNormals;
    fragPosWorld = position_in_world.xyz;

    fragColor = vColor;
   
    linearDepth = -(Scene3D.view * position_in_world).z;

    isRenderable = instance.isRender;

    if (instance.m_UseMaterials == 1)
    {
        if (instance.m_UseMeshMatID == 1)
        {
            int matID = int(meshID.y);
            vMaterial = GetMaterial(matID);
        }
        else
        {
            vMaterial = instance.m_Material;
        }
            
        
        UseMaterials = 1;
    }
    else
    {
        UseMaterials = 0;
    }
}