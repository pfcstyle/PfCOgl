#version 330

// Incoming per vertex... position and normal
in vec4 vVertex;
in vec3 vNormal;


uniform mat4   mvpMatrix;
uniform mat4   mvMatrix;
uniform mat3   normalMatrix;
uniform vec3   vLightPosition;

// Color to fragment program
smooth out vec3 vVaryingNormal;
smooth out vec3 vVaryingLightDir;

void main(void) 
    { 
    // Get surface normal in eye coordinates
    vVaryingNormal = normalMatrix * vNormal;

    // Get vertex position in eye coordinates
    vec4 vPosition4 = mvMatrix * vVertex;
    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

    // Get vector to light source
    vVaryingLightDir = normalize(vLightPosition - vPosition3);


    // Don't forget to transform the geometry!
    gl_Position = mvpMatrix * vVertex;
    }
