#version 410

// Incoming per vertex... just the position
in vec4 vVertex;

uniform mat4   mvpMatrix;  // Transformation matrix

// Texture Coordinate to fragment program
out vec3 vVaryingTexCoord;


void main(void)
{
    // Pass on the texture coordinates
    vVaryingTexCoord = normalize(vVertex.xyz);
    
    // Don't forget to transform the geometry!
    gl_Position = mvpMatrix * vVertex;
}
