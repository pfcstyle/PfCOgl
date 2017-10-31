//
//  PfCOglModelAsset.cpp
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglModelAsset.h"
using namespace PfCOgl;

void ModelAsset::bindData(GLfloat vertexData[], int length){
    // First time, create the buffer object, allocate the space
    if(vbo == 0) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * length, vertexData, GL_DYNAMIC_DRAW);
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * length, vertexData);
    }
}
