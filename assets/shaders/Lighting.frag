#version 450

layout (location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outConditions;

layout (set = 0, binding = 4) uniform sampler2D vColor;
layout (set = 0, binding = 5) uniform sampler2D vPositions;
layout (set = 0, binding = 6) uniform sampler2D vNormalsMap;
layout (set = 0, binding = 8) uniform sampler2D MaterialDataIn;
layout (set = 0, binding = 9) uniform sampler2D IsLightOn;



struct PointLight 
{
    vec4 Position;
    vec4 Color;
    vec4 attenuation;
};

struct DirectionalLight {
    vec4 direction;
    vec4 Color;
};
/*
struct SpotLight 
{
    vec4 Position;//16
    vec4 direction;//32
    vec4 Color;//48
    vec4 attenuation;//64
    float cutoff;
    float outerCutoff;
    float padding[2];//80
};*/


struct SpotLight 
{
    vec4 Position; //16
    vec4 direction; //32
    vec4 Color; //48
    vec4 attenuation; //64 
    float angle;
    float padding[3];//80
};

layout(push_constant) uniform LightingPush
{
	vec4 ambientlightcolor;
    int activepointlights;  
    int activeDirLights;
    int activeSpotLights;
    int toggleLight;
};


layout(std140, binding = 21) readonly buffer PointLightBuffer
{   
	PointLight pointLights[];
}PL_Buffer;

layout(std140, binding = 22) readonly buffer DirectionalLightBuffer
{   
	DirectionalLight dirLights[];
}DL_Buffer;

layout(std140, binding = 23) readonly buffer SpotLightBuffer
{   
	SpotLight spotLights[];
}SL_Buffer;

layout(set = 0, binding = 7) uniform SceneCamera
{
    mat4 view;
    mat4 projection;
    vec4 cameraPos;
} camera;




void main()
{

    vec4 isLightOn = texture(IsLightOn, inUV);
    vec4 MaterialProps = texture(MaterialDataIn, inUV);

    uint Lighted = uint(isLightOn.x);

    outConditions = isLightOn;

    uint shadingModel = uint(MaterialProps.z);
    uint UseMaterials = uint(MaterialProps.y);


    if (toggleLight == 0 || Lighted == 0)
    {
        outColor = texture(vColor, inUV);
        return;
    }

    if (UseMaterials == 1 && shadingModel != 0)
    {
        outColor = texture(vColor, inUV);
        return;
    }

    
    vec4 fragColor = texture(vColor, inUV);

    vec4 albedo = fragColor;
    vec4 fragPosWorld = texture(vPositions, inUV);
    vec4 fragNormalWorld = texture(vNormalsMap, inUV);

    vec3 ambientlight = ambientlightcolor.xyz * ambientlightcolor.w;//intensity stored in w of vec4
    vec3 SurfaceNormal = normalize(fragNormalWorld).xyz;
    vec3 result = vec3(0.0,0.0,0.0);

    //sum of pointlight lighting;
    for (int i=0;i< activepointlights; ++i)
    {
        PointLight currentlight = PL_Buffer.pointLights[i];
        vec3 directiontolight = currentlight.Position.xyz - fragPosWorld.xyz;
        float distance = length(directiontolight);
        directiontolight = normalize(directiontolight);
        
        //attenuation
        float attenuation = 1.0/(currentlight.attenuation.x + 
                            (currentlight.attenuation.y* distance) + 
                            (currentlight.attenuation.z* distance*distance));
        
        //diffuse
        float diff = max(dot(SurfaceNormal, directiontolight),0.0);
        vec3 diffuse = (currentlight.Color.xyz * currentlight.Color.w) * (diff * attenuation);
        
        result += diffuse;
    }
    
    //sum of Directional light lighting
    for (int i=0; i< activeDirLights; ++i )
    {
        vec3 LightDirection = normalize(-DL_Buffer.dirLights[i].direction.xyz);
        
        //diffuse
        float diff = max(dot(fragNormalWorld.xyz, LightDirection), 0.0);
        vec3 diffuse = DL_Buffer.dirLights[i].Color.rgb * diff;
        
        result +=diffuse;
    }
/*
    for (int i=0; i< activeSpotLights;++i)
    {
        SpotLight currentlight = SL_Buffer.spotLights[i];
        
        vec3 LightDirection = vec3(currentlight.Position - fragPosWorld);

        //attenuation
        float distance = length(LightDirection);
        LightDirection = normalize(LightDirection);
        float attenuation = 1.0/(currentlight.attenuation.x + 
                            (currentlight.attenuation.y* distance) + 
                            (currentlight.attenuation.z* distance*distance));

        //diffuse
        float diff = max(dot(fragNormalWorld.xyz, LightDirection), 0.0);
        vec3 diffuse = (currentlight.Color.xyz * currentlight.Color.w) * (attenuation * diff);
        
        //spotlight intensity based of cutoff angle
        float theta = dot(LightDirection, normalize(-currentlight.direction.xyz));
        float epsilon = currentlight.outerCutoff - currentlight.cutoff;
        float intensity = smoothstep(epsilon, currentlight.outerCutoff, theta);

        result += diffuse * intensity;
    }
    */


    for (int i=0; i< activeSpotLights;++i)
    {
        SpotLight currentlight = SL_Buffer.spotLights[i];
        
        vec3 LightDirection = vec3(currentlight.Position - fragPosWorld);

        //attenuation
        float distance = length(LightDirection);
        LightDirection = normalize(LightDirection);
        float attenuation = 1.0/(currentlight.attenuation.x + 
                            (currentlight.attenuation.y* distance) + 
                            (currentlight.attenuation.z* distance*distance));

        //diffuse
        float diff = max(dot(fragNormalWorld.xyz, LightDirection), 0.0);
        vec3 diffuse = (currentlight.Color.xyz * currentlight.Color.w) * (attenuation * diff);
        
        //spotlight intensity based of cutoff angle
        float theta = dot(LightDirection, normalize(-currentlight.direction.xyz));
        // float epsilon = currentlight.outerCutoff - currentlight.cutoff;
        // float intensity = smoothstep(epsilon, currentlight.outerCutoff, theta);
        float intensity = smoothstep(cos(currentlight.angle), 1.0, theta);
        result += diffuse * intensity;
    }

    outColor = albedo * vec4((ambientlight + result), 1.0);

}