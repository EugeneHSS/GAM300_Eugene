#version 450

layout(location = 0) out vec2 vUV; // Output texture coordinates

void main() {
    // Define positions for a full-screen quad using two triangles
    // Triangle 1: Bottom-Left, Bottom-Right, Top-Left
    // Triangle 2: Top-Left, Bottom-Right, Top-Right
    // The positions go from -1 to 1 in NDC (Normalized Device Coordinates)
    // The UVs go from 0 to 1
    vec2 positions[6] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0),
                               vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(1.0, 1.0));
    vec2 uvs[6] = vec2[](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0),
                         vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(1.0, 1.0));

    // Use gl_VertexIndex to select the vertex position and UV
    // This index automatically increases for each vertex processed
    vec2 position = positions[gl_VertexIndex % 6]; // Ensure the index is within bounds
    vUV = uvs[gl_VertexIndex % 6];

    // Output the final vertex position
    gl_Position = vec4(position, 0.0, 1.0);
}
