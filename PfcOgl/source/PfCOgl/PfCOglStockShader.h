//
//  PfCOglStockShader.h
//  PfcOgl
//
//  Created by developer on 22/10/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglStockShader_h
#define PfCOglStockShader_h
///////////////////////////////////////////////////////////////////////////////
// Identity Shader (GLT_SHADER_IDENTITY)
// This shader does no transformations at all, and uses the current
// glColor value for fragments.
// It will shade between verticies.

static const char *szIdentityShaderVP = "#version 410\n"
"in vec4 vVertex;"
"void main(void) "
"{ gl_Position = vVertex; "
"}";

static const char *szIdentityShaderFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"uniform vec4 vColor;"
"out vec4 fragColor;"
"void main(void) "
"{ fragColor = vColor;"
"}";


///////////////////////////////////////////////////////////////////////////////
// Flat Shader (GLT_SHADER_FLAT)
// This shader applies the given model view matrix to the verticies,
// and uses a uniform color value.
static const char *szFlatShaderVP = "#version 410\n"
"uniform mat4 mvpMatrix;"
"in vec4 vVertex;"
"void main(void) "
"{ gl_Position = mvpMatrix * vVertex; "
"}";

static const char *szFlatShaderFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"uniform vec4 vColor;"
"out vec4 fragColor;"
"void main(void) "
"{ fragColor = vColor; "
"}";

///////////////////////////////////////////////////////////////////////////////
// GLT_SHADER_SHADED
// Point light, diffuse lighting only
static const char *szShadedVP = "#version 410\n"
"uniform mat4 mvpMatrix;"
"in vec4 vColor;"
"in vec4 vVertex;"
"out vec4 vFragColor;"
"void main(void) {"
"vFragColor = vColor; "
" gl_Position = mvpMatrix * vVertex; "
"}";

static const char *szShadedFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"in vec4 vFragColor; "
"out vec4 fragColor;"
"void main(void) { "
" fragColor = vFragColor; "
"}";

// GLT_SHADER_DEFAULT_LIGHT
// Simple diffuse, directional, and vertex based light
static const char *szDefaultLightVP = "#version 410\n"
"uniform mat4 mvMatrix;"
"uniform mat4 pMatrix;"
"out vec4 vFragColor;"
"in vec4 vVertex;"
"in vec3 vNormal;"
"uniform vec4 vColor;"
"void main(void) { "
" mat3 mNormalMatrix;"
" mNormalMatrix[0] = mvMatrix[0].xyz;"
" mNormalMatrix[1] = mvMatrix[1].xyz;"
" mNormalMatrix[2] = mvMatrix[2].xyz;"
" vec3 vNorm = normalize(mNormalMatrix * vNormal);"
" vec3 vLightDir = vec3(0.0, 0.0, 1.0); "
" float fDot = max(0.0, dot(vNorm, vLightDir)); "
" vFragColor.rgb = vColor.rgb * fDot;"
" vFragColor.a = vColor.a;"
" mat4 mvpMatrix;"
" mvpMatrix = pMatrix * mvMatrix;"
" gl_Position = mvpMatrix * vVertex; "
"}";


static const char *szDefaultLightFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"in vec4 vFragColor; "
"out vec4 fragColor;"
"void main(void) { "
" fragColor = vFragColor; "
"}";

//GLT_SHADER_POINT_LIGHT_DIFF
// Point light, diffuse lighting only
static const char *szPointLightDiffVP = "#version 410\n"
"uniform mat4 mvMatrix;"
"uniform mat4 pMatrix;"
"uniform vec3 vLightPos;"
"uniform vec4 vColor;"
"in vec4 vVertex;"
"in vec3 vNormal;"
"out vec4 vFragColor;"
"void main(void) { "
" mat3 mNormalMatrix;"
" mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
" mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
" mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
" vec3 vNorm = normalize(mNormalMatrix * vNormal);"
" vec4 ecPosition;"
" vec3 ecPosition3;"
" ecPosition = mvMatrix * vVertex;"
" ecPosition3 = ecPosition.xyz /ecPosition.w;"
" vec3 vLightDir = normalize(vLightPos - ecPosition3);"
" float fDot = max(0.0, dot(vNorm, vLightDir)); "
" vFragColor.rgb = vColor.rgb * fDot;"
" vFragColor.a = vColor.a;"
" mat4 mvpMatrix;"
" mvpMatrix = pMatrix * mvMatrix;"
" gl_Position = mvpMatrix * vVertex; "
"}";


static const char *szPointLightDiffFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"in vec4 vFragColor; "
"out vec4 fragColor;"
"void main(void) { "
" fragColor = vFragColor; "
"}";

//GLT_SHADER_TEXTURE_REPLACE
// Just put the texture on the polygons
static const char *szTextureReplaceVP = "#version 410\n"
"uniform mat4 mvpMatrix;"
"in vec4 vVertex;"
"in vec2 vTexCoord0;"
"out vec2 vTex;"
"void main(void) "
"{ vTex = vTexCoord0;"
" gl_Position = mvpMatrix * vVertex; "
"}";

static const char *szTextureReplaceFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"in vec2 vTex;"
"out vec4 fragColor;"
"uniform sampler2D textureUnit0;"
"void main(void) "
"{ fragColor = texture(textureUnit0, vTex); "
"}";


// Just put the texture on the polygons
static const char *szTextureRectReplaceVP = "#version 410\n"
"uniform mat4 mvpMatrix;"
"in vec4 vVertex;"
"in vec2 vTexCoord0;"
"out vec2 vTex;"
"void main(void) "
"{ vTex = vTexCoord0;"
" gl_Position = mvpMatrix * vVertex; "
"}";

static const char *szTextureRectReplaceFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"in vec2 vTex;"
"out vec4 fragColor;"
"uniform sampler2DRect textureUnit0;"
"void main(void) "
"{ fragColor = texture(textureUnit0, vTex); "
"}";



//GLT_SHADER_TEXTURE_MODULATE
// Just put the texture on the polygons, but multiply by the color (as a unifomr)
static const char *szTextureModulateVP = "#version 410\n"
"uniform mat4 mvpMatrix;"
"in vec4 vVertex;"
"in vec2 vTexCoord0;"
"out vec2 vTex;"
"void main(void) "
"{ vTex = vTexCoord0;"
" gl_Position = mvpMatrix * vVertex; "
"}";

static const char *szTextureModulateFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"out vec4 fragColor;"
"in vec2 vTex;"
"uniform sampler2D textureUnit0;"
"uniform vec4 vColor;"
"void main(void) "
"{ fragColor = vColor * texture(textureUnit0, vTex); "
"}";



//GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF
// Point light (Diffuse only), with texture (modulated)
static const char *szTexturePointLightDiffVP = "#version 410\n"
"uniform mat4 mvMatrix;"
"uniform mat4 pMatrix;"
"uniform vec3 vLightPos;"
"uniform vec4 vColor;"
"in vec4 vVertex;"
"in vec3 vNormal;"
"out vec4 vFragColor;"
"in vec2 vTexCoord0;"
"out vec2 vTex;"
"void main(void) { "
" mat3 mNormalMatrix;"
" mNormalMatrix[0] = normalize(mvMatrix[0].xyz);"
" mNormalMatrix[1] = normalize(mvMatrix[1].xyz);"
" mNormalMatrix[2] = normalize(mvMatrix[2].xyz);"
" vec3 vNorm = normalize(mNormalMatrix * vNormal);"
" vec4 ecPosition;"
" vec3 ecPosition3;"
" ecPosition = mvMatrix * vVertex;"
" ecPosition3 = ecPosition.xyz /ecPosition.w;"
" vec3 vLightDir = normalize(vLightPos - ecPosition3);"
" float fDot = max(0.0, dot(vNorm, vLightDir)); "
" vFragColor.rgb = vColor.rgb * fDot;"
" vFragColor.a = vColor.a;"
" vTex = vTexCoord0;"
" mat4 mvpMatrix;"
" mvpMatrix = pMatrix * mvMatrix;"
" gl_Position = mvpMatrix * vVertex; "
"}";


static const char *szTexturePointLightDiffFP = "#version 410\n"
#ifdef OPENGL_ES
"precision mediump float;"
#endif
"in vec4 vFragColor;"
"in vec2 vTex;"
"uniform sampler2D textureUnit0;"
"out vec4 fragColor;"
"void main(void) { "
" fragColor = vFragColor * texture(textureUnit0, vTex);"
"}";

#endif /* PfCOglStockShader_h */
