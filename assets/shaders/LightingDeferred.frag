#version 450

layout (location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout (set = 0, binding = 4) uniform sampler2D vColor;
layout (set = 0, binding = 5) uniform sampler2D vPositions;
layout (set = 0, binding = 6) uniform sampler2D vNormalsMap;


struct PointLight 
{
    vec4 Position;
    vec4 Color;
    vec4 attenuation;
    float radius;
    float padding[3];
};

struct DirectionalLight {
    vec4 direction;
    vec4 Color;
};

struct SpotLight 
{
    vec4 Position;//16
    vec4 direction;//32
    vec4 Color;//48
    vec4 attenuation;//64
    float cutoff;
    float outerCutoff;
    float radius;
    float padding;

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

vec3 CalcDirLight(DirectionalLight dirLight, vec3 V, vec3 N, vec3 modelPos) {
    vec3 L = normalize(vec3(-dirLight.direction));
    float NdotL = max(dot(N, L), 0.0);

    // Diffuse
    vec3 diffuse = vec3(dirLight.Color) * NdotL;

    // Specular
    vec3 R = reflect(-L, N);
    float specAngle = max(dot(R, V), 0.0);
    vec3 specular = 0.5 * pow(specAngle, 16.0) * vec3(dirLight.Color); 

    // No attenuation needed for directional lights since they are considered infinitely far away
    return diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 modelPos) {
    vec3 L = normalize(light.Position.xyz - modelPos);
    float distance = length(light.Position.xyz - modelPos);
    if(distance > light.radius) return vec3(0);

    float NdotL = max(dot(N, L), 0.0);

    // Attenuation
    float attenuation = clamp(1.0 - distance/light.radius, 0.0, 1.0);
    attenuation = clamp(attenuation, 0.0, 1.0);

    // Diffuse
    vec3 diffuse = vec3(light.Color) * NdotL * attenuation;

    // Specular
    vec3 R = reflect(-L, N);
    float specAngle = max(dot(R, V), 0.0);
    vec3 specular = 0.5 * pow(specAngle, 16.0) * light.Color.xyz * attenuation;

    return diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 modelPos)
{
    float distance = length(light.Position.xyz - modelPos);
    if(distance > light.radius) return vec3(0); // Corrected typo: raduis to radius

    vec3 L = normalize(light.Position.xyz - modelPos);
    float theta = dot(L, normalize(-light.direction.xyz));

    if(theta > light.outerCutoff) return vec3(0); // Outside of spotlight cone

    // Spotlight soft edge calculation
    float epsilon = (light.cutoff - light.outerCutoff);
    float intensity = clamp((light.cutoff - theta) / epsilon, 0.0, 1.0);
    float NdotL = max(dot(N, L), 0.0); // Lambertian reflection

    // Attenuation
    float attenuation = 1.0 - (distance / light.radius);
    attenuation = clamp(attenuation, 0.0, 1.0);
    
    vec3 diffuse = light.Color.xyz * NdotL * attenuation * intensity;

    // Simple specular component
    vec3 R = reflect(-L, N); // Reflected light vector
    float specAngle = max(dot(R, V), 0.0);
    float specularStrength = pow(specAngle, 16.0); // Shininess coefficient, adjust as needed
    vec3 specular = 0.5 * specularStrength * attenuation * intensity * light.Color.xyz; // Specular term, '0.5' as specular intensity

    return diffuse + specular; // Combined lighting contribution without PBR
}

void main()
{
    if (toggleLight == 0)
    {
        outColor = texture(vColor, inUV);
        return;
    }

    vec4 albedo = texture(vColor, inUV);
    vec3 fragPosWorld = texture(vPositions, inUV).xyz;
    vec3 fragNormalWorld = normalize(texture(vNormalsMap, inUV).xyz * 2.0 - 1.0); // Assuming normals are stored in [0,1], convert to [-1,1]

    vec3 ambientLight = ambientlightcolor.xyz * ambientlightcolor.w; // Intensity stored in w of vec4
    vec3 result = ambientLight;

    vec3 V = normalize(camera.cameraPos.xyz - fragPosWorld);

    for(int i = 0; i < activeDirLights; i++)
    {
        result += CalcDirLight(DL_Buffer.dirLights[i], V, fragNormalWorld, fragPosWorld);
    }

    for(int i = 0; i < activepointlights; i++)
    {
        result += CalcPointLight(PL_Buffer.pointLights[i], V, fragNormalWorld, fragPosWorld);
    }

    for(int i = 0; i < activeSpotLights; i++)
    {
        result += CalcSpotLight(SL_Buffer.spotLights[i], V, fragNormalWorld, fragPosWorld);
    }

    outColor = vec4(result, 1.0) * albedo;

    
}
