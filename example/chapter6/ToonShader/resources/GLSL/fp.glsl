#version 410

uniform sampler1D colorTable;
out vec4 vFragColor;

smooth in float textureCoordinate;


void main(void)
   { 
   vFragColor = texture(colorTable, textureCoordinate);
   }
    
