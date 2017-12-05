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

namespace PfCOgl {
    class ModelAsset:PfCObject{
    public:
        Program* shaders;
        Texture* texture;
        GLuint vbo;
        GLuint vao;
        GLenum drawType;
        GLint drawStart;
        GLint drawCount;
        GLfloat shininess;
        glm::vec3 specularColor;
        
        void bindData(GLfloat vertexData[], int length);
        void bindData(M3DVector3f vecData[], int length);
        void bindData(M3DVector4f vecData[], int length);
        void begin(void);
        void end(void);
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
        
        void draw(void) const;
        
        ModelAsset() :
        shaders(NULL),
        texture(NULL),
        vbo(0),
        vao(0),
        drawType(GL_TRIANGLES),
        drawStart(0),
        drawCount(0),
        shininess(0.0f),
        specularColor(1.0f, 1.0f, 1.0f)
        {}
        ~ModelAsset(){};
    };
}

#endif /* PfCOglModelAsset_h_ */
