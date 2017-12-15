#version 410


out vec4 vFragColor;

in vec4 vStarColor;
in mat2 rotationMatrix;
uniform sampler2D  starImage;

void main(void)
{
    vec2 p = gl_PointCoord - vec2(0.5);
//    if (dot(p,p) > 1.0 ) {//圆形
//        discard;
//    }
//
    if (dot(p,p) > sin(atan(p.y, p.x) * 5.0)) {//花朵
        discard;
    }
    vFragColor = texture(starImage, rotationMatrix * gl_PointCoord  + vec2(0.5)) * vStarColor;
}
    
