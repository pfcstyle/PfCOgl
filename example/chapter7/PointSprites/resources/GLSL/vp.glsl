#version 410

// Incoming per vertex... position and normal
in vec4 vVertex;
in vec4 vColor;

uniform float vAngle;

uniform mat4   mvpMatrix;
uniform float  timeStamp;

out vec4 vStarColor;
//让点旋转起来
out mat2 rotationMatrix;

void main(void)
{
    float sin_theta = sin(vAngle);
    float cos_theta = cos(vAngle);
    rotationMatrix = mat2(cos_theta, sin_theta,
                          -sin_theta, cos_theta);
    vec4 vNewVertex = vVertex;
    vStarColor = vColor;
    
    // Offset by running time, makes it move closer
    vNewVertex.z += timeStamp;
    
    // If out of range, adjust
    if(vNewVertex.z > -1.0)
        vNewVertex.z -= 500.0;
    
    gl_PointSize = 30.0 + (vNewVertex.z / sqrt(-vNewVertex.z));
    
    // If they are very small, fade them up
    if(gl_PointSize < 6.0)
        vStarColor = smoothstep(0.0, 6.0, gl_PointSize) * vStarColor;
    
    
    // Don't forget to transform the geometry!
    gl_Position = mvpMatrix * vNewVertex;
}
