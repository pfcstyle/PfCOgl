//
//  PfCOglModelInstance.cpp
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglModelInstance.h"
#include "PfCOglTool.h"
using namespace PfCOgl;


// Start the primitive batch.
void ModelInstance::begin()
{
    
    // Vertex Array object for this Array
#ifndef OPENGL_ES
    if (asset->vao == 0) {
        glGenVertexArrays(1, &asset->vao);
    }
    glBindVertexArray(asset->vao);
#endif
}

// Bind everything up in a little package
void ModelInstance::end(void)
{

#ifndef OPENGL_ES
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void ModelInstance::bindData(GLfloat *vertexData, int length){
    // First time, create the buffer object, allocate the space
    if(asset->vbo == 0) {
        glGenBuffers(1, &asset->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * length, vertexData, GL_DYNAMIC_DRAW);
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * length, vertexData);
    }
}

void ModelInstance::bindData(M3DVector3f vecData[], int length){
    int arrSize = 3 * length;
    float vertexData[arrSize];
    for (int i = 0; i < length; i++) {
        vertexData[i * 3] = vecData[i][0];
        vertexData[i * 3 + 1] = vecData[i][1];
        vertexData[i * 3 + 2] = vecData[i][2];
    }
    // First time, create the buffer object, allocate the space
    if(asset->vbo == 0) {
        glGenBuffers(1, &asset->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
    }
}

void ModelInstance::bindData(M3DVector4f vecData[], int length){
    int arrSize = 4 * length;
    float vertexData[arrSize];
    for (int i = 0; i < length; i++) {
        vertexData[i * 4] = vecData[i][0];
        vertexData[i * 4 + 1] = vecData[i][1];
        vertexData[i * 4 + 2] = vecData[i][2];
        vertexData[i * 4 + 3] = vecData[i][3];
    }
    // First time, create the buffer object, allocate the space
    if(asset->vbo == 0) {
        glGenBuffers(1, &asset->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
    }
}

void ModelInstance::CopyData(char *varName, GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid *startPos){
    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(asset->shaders->attrib(varName));
    glVertexAttribPointer(asset->shaders->attrib(varName), vecNum, dataType, isNormlize, stepNum, startPos);
}

void ModelInstance::CopyData(GLT_SHADER_ATTRIBUTE varName, GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos){
    glEnableVertexAttribArray(varName);
    glVertexAttribPointer(varName, vecNum, dataType, isNormlize, stepNum, startPos);
}


void ModelInstance::draw(void) const
{
    asset->draw();
    
}
