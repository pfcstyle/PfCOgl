#version 410

out vec4 vFragColor;
smooth in vec4 vVaryingColor;

void main(void)
{ 
   vFragColor = vVaryingColor;
}