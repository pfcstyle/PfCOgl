//
//  PfCOglModelAsset.cpp
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglModelAsset.h"
#include "PfCOglTool.h"
using namespace PfCOgl;

void ModelAsset::begin()
{
    
    // Vertex Array object for this Array
#ifndef OPENGL_ES
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
    }
    glBindVertexArray(vao);
#endif
}

// Bind everything up in a little package
void ModelAsset::end(void)
{
    
#ifndef OPENGL_ES
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

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

void ModelAsset::bindData(M3DVector3f vecData[], int length){
    int arrSize = 3 * length;
    float vertexData[arrSize];
    for (int i = 0; i < length; i++) {
        vertexData[i * 3] = vecData[i][0];
        vertexData[i * 3 + 1] = vecData[i][1];
        vertexData[i * 3 + 2] = vecData[i][2];
    }
    // First time, create the buffer object, allocate the space
    if(vbo == 0) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);
        
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
        
        
    }
}

void ModelAsset::bindData(M3DVector4f vecData[], int length){
    int arrSize = 4 * length;
    float vertexData[arrSize];
    for (int i = 0; i < length; i++) {
        vertexData[i * 4] = vecData[i][0];
        vertexData[i * 4 + 1] = vecData[i][1];
        vertexData[i * 4 + 2] = vecData[i][2];
        vertexData[i * 4 + 3] = vecData[i][3];
    }
    // First time, create the buffer object, allocate the space
    if(vbo == 0) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
    }
}

void ModelAsset::CopyData(char *varName, GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid *startPos){
    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(shaders->attrib(varName));
    glVertexAttribPointer(shaders->attrib(varName), vecNum, dataType, isNormlize, stepNum, startPos);
}

void ModelAsset::CopyData(GLT_SHADER_ATTRIBUTE varName, GLuint vecNum, GLenum dataType, GLboolean isNormlize, GLsizei stepNum, const GLvoid* startPos){
    glEnableVertexAttribArray(varName);
    glVertexAttribPointer(varName, vecNum, dataType, isNormlize, stepNum, startPos);
}

void ModelAsset::draw(void) const
{
    glBindVertexArray(vao);
    glDrawArrays(drawType, drawStart, drawCount);
    glBindVertexArray(0);
    
}
