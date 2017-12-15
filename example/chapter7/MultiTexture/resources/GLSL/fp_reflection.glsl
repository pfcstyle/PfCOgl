#version 410

out vec4 vFragColor;

uniform samplerCube cubeMap;
uniform sampler2D  tarnishMap;
smooth in vec3 vVaryingTexCoord;
smooth in vec2 vTarnishCoords;
void main(void)
{
    vFragColor = texture(cubeMap, vVaryingTexCoord.stp);
    vFragColor *= texture(tarnishMap, vTarnishCoords);
}
    
