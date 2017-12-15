#version 410

out vec4 vFragColor;

uniform sampler2DArray moonImage;

smooth in vec3 vMoonCoords;

void main(void)
{
    vFragColor = texture(moonImage, vMoonCoords);
}
