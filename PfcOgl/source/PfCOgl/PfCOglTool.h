//
//  PfCOglTool.h
//  PfcOgl
//
//  Created by developer on 22/10/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglTool_h_
#define PfCOglTool_h_

// There is a static block allocated for loading shaders to
// prevent heap fragmentation
#define MAX_SHADER_LENGTH   8192


// Bring in OpenGL
// Windows
#ifdef WIN32
#include <windows.h>        // Must have for Windows platform builds
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <gl\glew.h>            // OpenGL Extension "autoloader"
#include <gl\gl.h>            // Microsoft OpenGL headers (version 1.1 by themselves)
#endif

// Mac OS X
#ifdef __APPLE__
#include <stdlib.h>

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE | TARGET_IPHONE_SIMULATOR
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#define OPENGL_ES
#else
#include <GL/glew.h>
#include <OpenGL/gl.h>        // Apple OpenGL haders (version depends on OS X SDK version)
#endif
#endif

// Linux
#ifdef linux
#define GLEW_STATIC
#include <glew.h>
#endif

// Universal includes
#include <stdio.h>
#include <math.h>
#include "math3d.h"
#include "PfCOglBatch.h"
#include "PfCOglTriangleBatch.h"
namespace PfCOgl{
    ///////////////////////////////////////////////////////
    // Macros for big/little endian happiness
    // These are intentionally written to be easy to understand what they
    // are doing... no flames please on the inefficiency of these.
#ifdef __BIG_ENDIAN__
    ///////////////////////////////////////////////////////////
    // This function says, "this pointer is a little endian value"
    // If the value must be changed it is... otherwise, this
    // function is defined away below (on Intel systems for example)
    inline void LITTLE_ENDIAN_WORD(void *pWord)
    {
        unsigned char *pBytes = (unsigned char *)pWord;
        unsigned char temp;
        
        temp = pBytes[0];
        pBytes[0] = pBytes[1];
        pBytes[1] = temp;
    }
    
    ///////////////////////////////////////////////////////////
    // This function says, "this pointer is a little endian value"
    // If the value must be changed it is... otherwise, this
    // function is defined away below (on Intel systems for example)
    inline void LITTLE_ENDIAN_DWORD(void *pWord)
    {
        unsigned char *pBytes = (unsigned char *)pWord;
        unsigned char temp;
        
        // Swap outer bytes
        temp = pBytes[3];
        pBytes[3] = pBytes[0];
        pBytes[0] = temp;
        
        // Swap inner bytes
        temp = pBytes[1];
        pBytes[1] = pBytes[2];
        pBytes[2] = temp;
    }
#else
    
    // Define them away on little endian systems
#define LITTLE_ENDIAN_WORD
#define LITTLE_ENDIAN_DWORD
#endif
    
    ///////////////////////////////////////////////////////////////////////////////
    //         THE LIBRARY....
    ///////////////////////////////////////////////////////////////////////////////
    
    // Get the OpenGL version
    void gltGetOpenGLVersion(GLint &nMajor, GLint &nMinor);
    
    // Check to see if an exension is supported
    int gltIsExtSupported(const char *szExtension);
    
    // Set working directoyr to /Resources on the Mac
    void gltSetWorkingDirectory(const char *szArgv);
    
    ///////////////////////////////////////////////////////////////////////////////
    GLbyte* gltReadBMPBits(const char *szFileName, int *nWidth, int *nHeight);
    
    /////////////////////////////////////////////////////////////////////////////////////
    // Load a .TGA file
    GLbyte *gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat);
    
    // Capture the frame buffer and write it as a .tga
    // Does not work on the iPhone
#ifndef OPENGL_ES
    GLint gltGrabScreenTGA(const char *szFileName);
#endif
    
    
    // Make Objects
    void gltMakeTorus(TriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    void gltMakeSphere(TriangleBatch& sphereBatch, GLfloat fRadius, GLint iSlices, GLint iStacks);
    void gltMakeDisk(TriangleBatch& diskBatch, GLfloat innerRadius, GLfloat outerRadius, GLint nSlices, GLint nStacks);
    void gltMakeCylinder(TriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks);
    void gltMakeCube(Batch& cubeBatch, GLfloat fRadius);
    
    // Shader loading support
    void    gltLoadShaderSrc(const char *szShaderSrc, GLuint shader);
    bool    gltLoadShaderFile(const char *szFile, GLuint shader);
    
    GLuint    gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg);
    GLuint   gltLoadShaderPairWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);
    
    GLuint gltLoadShaderPairSrc(const char *szVertexSrc, const char *szFragmentSrc);
    GLuint gltLoadShaderPairSrcWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...);
    
    bool gltCheckErrors(GLuint progName = 0);
    void gltGenerateOrtho2DMat(GLuint width, GLuint height, M3DMatrix44f &orthoMatrix, Batch &screenQuad);
    
    template <class T>
    int getArrayLen(T& array) //使用模板定义一个函数getArrayLen,该函数将返回数组array的长度
    {
        return (sizeof(array) / sizeof(array[0]));
    }
}
#endif /* PfCOglTool_h_ */
