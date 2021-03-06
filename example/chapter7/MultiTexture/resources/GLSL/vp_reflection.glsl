#version 410

// Incoming per vertex... position and normal
in vec4 vVertex;
in vec3 vNormal;
in vec2 vTexCoords;

uniform mat4   mvpMatrix;
uniform mat4   mvMatrix;
uniform mat3   normalMatrix;
//uniform mat4   mInverseCamera;

// Texture coordinate to fragment program
smooth out vec3 vVaryingTexCoord;
smooth out vec2 vTarnishCoords;

void main(void)
{
    // Normal in Eye Space
    vec3 vEyeNormal = normalMatrix * vNormal;
    
    // Vertex position in Eye Space
    vec4 vVert4 = mvMatrix * vVertex;
    vec3 vEyeVertex = normalize(vVert4.xyz / vVert4.w);
    
    // Get reflected vector
    vec4 vCoords = vec4(vEyeVertex, 1.0);
    
    // Rotate by flipped camera
//    vCoords = mInverseCamera * vCoords;
    vVaryingTexCoord.xyz = normalize(vCoords.xyz);
    vTarnishCoords = vTexCoords.st;
    
    // Don't forget to transform the geometry!
    gl_Position = mvpMatrix * vVertex;
}
