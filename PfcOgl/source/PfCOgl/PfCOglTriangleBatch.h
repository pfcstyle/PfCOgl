//
//  PfCOglTriangleBatch.h
//  PfcOgl
//
//  Created by developer on 23/10/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglTriangleBatch_h_
#define PfCOglTriangleBatch_h_

#include <stdio.h>
#include <GL/glew.h>
#include "PfCOglBatchBase.h"
#include "math3d.h"

namespace PfCOgl {
#define VERTEX_DATA     0
#define NORMAL_DATA     1
#define TEXTURE_DATA    2
#define INDEX_DATA      3
    class TriangleBatch : BatchBase{
    public:
        TriangleBatch(void);
        virtual ~TriangleBatch(void);
        
        // Use these three functions to add triangles
        void beginMesh(GLuint nMaxVerts);
        void addTriangle(M3DVector3f verts[3], M3DVector3f vNorms[3], M3DVector2f vTexCoords[3]);
        void end(void);
        
        // Useful for statistics
        inline GLuint GetIndexCount(void) { return nNumIndexes; }
        inline GLuint GetVertexCount(void) { return nNumVerts; }
        
        
        // Draw - make sure you call glEnableClientState for these arrays
        virtual void draw(void);
        
    protected:
        GLushort  *pIndexes;        // Array of indexes
        M3DVector3f *pVerts;        // Array of vertices
        M3DVector3f *pNorms;        // Array of normals
        M3DVector2f *pTexCoords;    // Array of texture coordinates
        
        GLuint nMaxIndexes;         // Maximum workspace
        GLuint nNumIndexes;         // Number of indexes currently used
        GLuint nNumVerts;           // Number of vertices actually used
        
        GLuint bufferObjects[4];
        GLuint vertexArrayBufferObject;
    };
}

#endif /* PfCOglTriangleBatch_h_ */
