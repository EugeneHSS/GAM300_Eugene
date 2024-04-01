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
struct BatchData
{
    mat4 modelMatrix;
    int  matID;
    uint textureID;
    uint isRender;
    uint entityID;

    uint m_AnimOffset;
    uint m_IsAnimated;
    uint m_UseMeshMatID;
    uint m_UseMaterials;

    Material material;
};


layout(std140, binding = 15) readonly buffer BatchBuffer
{   
	BatchData batch[];
};

out gl_PerVertex 
{
    vec4 gl_Position;   
};

layout(push_constant) uniform Scene3D
{
    mat4 depthMVP;
};

layout (location = 0) out uint isRenderable;
void main() 
{
    BatchData batchData = batch[uint(meshID.x)];

    mat4 modelMatrix = batchData.modelMatrix;

    mat4 skinMat = mat4(1.0);

    mat4 accumulated = modelMatrix * skinMat;
    
    vec4 position_in_world = accumulated * vec4(vPosition, 1.0);

    vec4 clipspacepos =  depthMVP * position_in_world;
    gl_Position = clipspacepos;
    
    isRenderable = batchData.isRender;

}