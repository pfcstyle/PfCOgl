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
        ~ModelAsset();
    };
}

#endif /* PfCOglModelAsset_h_ */
