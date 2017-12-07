//
//  PfCOglModelAsset.h
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglModelAsset_h_
#define PfCOglModelAsset_h_

#include "PfCOglObject.h"
#include "PfCOglProgram.h"
#include "PfCOglTexture.h"
#include "math3d.h"

typedef float    Vector2f[2];        // 3D points = 3D Vectors, but we need a
typedef double    Vector2d[2];        // 2D representations sometimes... (x,y) order
typedef float    Vector3f[3];        // Vector of three floats (x, y, z)
typedef double    Vector3d[3];        // Vector of three doubles (x, y, z)

typedef float    Vector4f[4];        // Lesser used... Do we really need these?
typedef double    Vector4d[4];        // Yes, occasionaly we do need a trailing w component

namespace PfCOgl {
    class ModelAsset:PfCObject{
    public:
        Program* shaders;
        Texture* texture;
        GLuint vbo;
        GLuint vao;
        GLuint      uiNormalArray;
        GLuint      uiColorArray;
        GLuint      uiTextureCoordArray;
        GLuint      uiIndexArray;
        GLenum drawType;
        GLint drawStart;
        GLint drawCount;
        GLfloat shininess;
        glm::vec3 specularColor;
        GLuint nVertsBuilding;            // Building up vertexes counter (immediate mode emulator)
        GLuint nNumVerts;                // Number of verticies in this batch
        GLuint nNumTextureUnits;        // Number of texture coordinate sets
        
        M3DVector3f *pVerts;
        M3DVector3f *pNormals;
        M3DVector4f *pColors;
        M3DVector2f *pTexCoords;
        
        //meshs
        GLushort  *pIndexes;        // Array of indexes
        GLuint nMaxIndexes;         // Maximum workspace
        GLuint nNumIndexes;         // Number of indexes currently used
        bool isMesh;
        
        void bindData(GLfloat vertexData[], int length);
        void bindData(M3DVector3f vecData[], int length);
        void bindData(M3DVector4f vecData[], int length);
        void begin(void);
        void end(void);
        void begin(GLenum primitive, GLuint nVerts);
        void endBatchs(void);
        void beginMesh(GLuint nMaxVerts);
        void addTriangle(M3DVector3f verts[3], M3DVector3f vNorms[3], M3DVector2f vTexCoords[3]);
        void endMesh(void);
        void CopyData(char *varName, GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos);
        void CopyData(GLT_SHADER_ATTRIBUTE varName, GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos);
        inline void CopyVertexData3f(GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos ){
            CopyData(PfCOgl::GLT_ATTRIBUTE_VERTEX, vecNum, dataType, isNormlize, stepNum, startPos);
        }
        inline void CopyNormalDataf(GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos){
            CopyData(PfCOgl::GLT_ATTRIBUTE_NORMAL, vecNum, dataType, isNormlize, stepNum, startPos);
        }
        inline void CopyColorData4f(GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos){
            CopyData(PfCOgl::GLT_ATTRIBUTE_COLOR, vecNum, dataType, isNormlize, stepNum, startPos);
        }
        inline void CopyTexCoordData2f(GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos){
            CopyData(PfCOgl::GLT_ATTRIBUTE_TEXTURE0, vecNum, dataType, isNormlize, stepNum, startPos);
        }
        void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
        void Vertex3fv(M3DVector3f vVertex);
        
        void Normal3f(GLfloat x, GLfloat y, GLfloat z);
        void Normal3fv(M3DVector3f vNormal);
        
        void Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void Color4fv(M3DVector4f vColor);
        
        void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
        void MultiTexCoord2fv(GLuint texture, M3DVector2f vTexCoord);
        
        void draw(void) const;
        
        ModelAsset() :
        shaders(NULL),
        texture(NULL),
        pVerts(NULL),
        pNormals(NULL),
        pColors(NULL),
        pTexCoords(NULL),
        pIndexes(NULL),
        nMaxIndexes(0),
        nNumIndexes(0),
        isMesh(false),
        vbo(0),
        vao(0),
        uiNormalArray(0),
        uiColorArray(0),
        uiTextureCoordArray(0),
        drawType(GL_TRIANGLES),
        drawStart(0),
        drawCount(0),
        shininess(0.0f),
        nVertsBuilding(0),
        nNumVerts(0),
        nNumTextureUnits(0),
        specularColor(1.0f, 1.0f, 1.0f)
        {}
        ~ModelAsset(){};
    };
}

#endif /* PfCOglModelAsset_h_ */
