//
//  PfCOglModelInstance.h
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglModelInstance_h_
#define PfCOglModelInstance_h_

#include "PfCOglModelAsset.h"
#include "PfCOglObject.h"
#include "math3d.h"
#include "PfCOglProgram.h"

namespace PfCOgl {
    class ModelInstance:PfCObject{
    public:
        ModelAsset* asset;
        glm::mat4 transform;
        
        ModelInstance() :
        asset(NULL),
        normalMatrix(),
        transform()
        {}
        
        
        ~ModelInstance(){}
        
        inline void bindData(GLfloat *vertexData, int length);
        inline void bindData(M3DVector3f vecData[], int length);
        inline void bindData(M3DVector4f vecData[], int length);
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
        
        glm::mat3 getNormalMatrix(bool isNormaize = false);
        
        void draw(void) const;
    protected:
        glm::mat3 normalMatrix;
    };

}

#endif /* PfCOglModelInstance_h_ */
