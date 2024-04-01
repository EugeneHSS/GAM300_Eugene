
#version 450

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

layout(set = 1, binding = 4) uniform sampler2D texArraySampler[1000];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosWorld;
layout(location = 3) in vec3 fragNormalWorld; 
layout(location = 4) in flat uint id;
layout(location = 5) in vec4 clipspacepos;
layout(location = 6) in flat uint UseMaterials;
layout(location = 7) flat in uint texID;
layout(location = 8) in float linearDepth;
layout(location = 9) flat in uint isRenderable;
layout(location = 10) in mat3 btn;
layout(location = 13) flat in Material vmaterial;

layout (location = 0) out vec4 outColor;
layout (location = 1) out uint outID;
layout (location = 2) out vec4 outPositions;
layout (location = 3) out vec4 outNormals;

layout (location = 4) out vec4 MaterialDataOut;

//Some boolean to see if want to light the object or not
layout (location = 5) out vec4 isLighted;



void main() 
{
    if (isRenderable == 0) discard;

    int textureID = int(texID);
    outID = id;

    if (UseMaterials == 1)
    {
        outNormals = vec4(fragNormalWorld, 1.0);
    
        if (vmaterial.diffuseTex != -1)
        {
            outColor = texture(texArraySampler[vmaterial.diffuseTex], fragTexCoord);
        }
        else
        {
            outColor = vec4(vmaterial.diffuse, 1.0);
        }

    }
    else
    {
        outColor = texture(texArraySampler[textureID], fragTexCoord);
        outNormals = vec4(fragNormalWorld, 1.0);
    }

    MaterialDataOut = vec4(float(vmaterial.MaterialID), float(UseMaterials), float(vmaterial.ShadingModel), 0.0);
            //outColor = texture(texArraySampler[textureID], fragTexCoord);
        outNormals = vec4(fragNormalWorld, 1.0);
  
    
    outPositions = vec4(fragPosWorld, 1.0);
    isLighted = vec4(1.0, 0.0, 0.0, 0.0);
    

    
}