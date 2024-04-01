#version 450 core

const vec2 OFFSET[6] = vec2[](
    vec2(1.0, 1.0),//btm right
    vec2(1.0, -1.0), //top right
    vec2(-1.0, -1.0),// top left
    vec2(-1.0, 1.0),  //btm left
    vec2(1.0,1.0),//btm right
    vec2(-1.0,-1.0)); //top left

struct Particle {
    //transform params
    vec4 Size;
    vec4 Rotation;
    vec4 CurrentPosition;
    
    vec4 Velocity;
    vec4 Acceleration;
    vec4 Color;
    float Age;
    uint Active;
};

layout (std140, binding = 31) buffer Particles {
    Particle List[];
}v_Particles;

layout (std140 ,binding = 5) buffer Camera{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
}Cam;

/*
layout (std140, binding = 34) buffer TransformMatrix{
    mat4 List[];
}v_TransformMatrix;
*/

//output to fragment shader
layout (location = 0) out vec3 out_Color;
layout (location = 1) out vec2 fragoffset;

void main(){
    int index = gl_InstanceIndex;
    fragoffset = OFFSET[gl_VertexIndex];
    //mat4 currentParticlexform = v_TransformMatrix.List[index];
    Particle currentParticle = v_Particles.List[index];
    //if the particle is not active, don't draw it
    if(v_Particles.List[index].Active == 0){
        //gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
        out_Color = vec3(0.0, 0.0, 0.0);
        return;
    }

    //transform the vertex position
    vec3 WorldCameraRight = vec3(Cam.ViewMatrix[0][0], Cam.ViewMatrix[1][0], Cam.ViewMatrix[2][0]);
    vec3 WorldCameraUp = vec3(Cam.ViewMatrix[0][1], Cam.ViewMatrix[1][1], Cam.ViewMatrix[2][1]);

    vec3 worldPosition = currentParticle.CurrentPosition.xyz + 
    (currentParticle.Size.x * fragoffset.x * WorldCameraRight) + 
    (currentParticle.Size.y * -fragoffset.y * WorldCameraUp);
    gl_Position = Cam.ProjectionMatrix * Cam.ViewMatrix * vec4(worldPosition, 1.0);
    
    //output the color
    out_Color = v_Particles.List[index].Color.rgb;

    
    
}