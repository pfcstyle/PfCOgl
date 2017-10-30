//
//  PfCOglBatch.h
//  PfcOgl
//
//  Created by developer on 23/10/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglBatch_h_
#define PfCOglBatch_h_

#include <stdio.h>
#include <GL/glew.h>
#include "PfCOglBatchBase.h"
#include "math3d.h"

namespace PfCOgl {
    class Batch : public BatchBase{
    public:
        Batch(void);
        ~Batch(void);
        
        void begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits = 0);
        void end(void);
        
        void CopyVertexData3f(M3DVector3f *vVerts);
        void CopyNormalDataf(M3DVector3f *vNorms);
        void CopyColorData4f(M3DVector4f *vColors);
        void CopyTexCoordData2f(M3DVector2f *vTexCoords, GLuint uiTextureLayer);
        
        inline void CopyVertexData3f(GLfloat *vVerts) { CopyVertexData3f((M3DVector3f *)(vVerts)); }
        inline void CopyNormalDataf(GLfloat *vNorms) { CopyNormalDataf((M3DVector3f *)(vNorms)); }
        inline void CopyColorData4f(GLfloat *vColors) { CopyColorData4f((M3DVector4f *)(vColors)); }
        inline void CopyTexCoordData2f(GLfloat *vTex, GLuint uiTextureLayer) { CopyTexCoordData2f((M3DVector2f *)(vTex), uiTextureLayer); }
        
        void draw(void);
        
        // Immediate mode emulation
        // Slowest way to build an array on purpose... Use the above if you can instead
        void reset(void);
        
        void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
        void Vertex3fv(M3DVector3f &vVertex);
        
        void Normal3f(GLfloat x, GLfloat y, GLfloat z);
        void Normal3fv(M3DVector3f &vNormal);
        
        void Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void Color4fv(M3DVector4f &vColor);
        
        void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
        void MultiTexCoord2fv(GLuint texture, M3DVector2f &vTexCoord);
        
    protected:
        GLenum        primitiveType;        // What am I drawing....
        
        GLuint        uiVertexArray;
        GLuint      uiNormalArray;
        GLuint        uiColorArray;
        GLuint        *uiTextureCoordArray;
        GLuint        vertexArrayObject;
        
        GLuint nVertsBuilding;            // Building up vertexes counter (immediate mode emulator)
        GLuint nNumVerts;                // Number of verticies in this batch
        GLuint nNumTextureUnits;        // Number of texture coordinate sets
        
        bool    bBatchDone;                // Batch has been built
        
        
        M3DVector3f *pVerts;
        M3DVector3f *pNormals;
        M3DVector4f *pColors;
        M3DVector2f **pTexCoords;
    };
}

#endif /* PfCOglBatch_h_ */
