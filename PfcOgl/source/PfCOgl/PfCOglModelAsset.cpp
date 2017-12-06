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

void ModelAsset::begin(GLenum primitive, GLuint nVerts)
{
    drawType = primitive;
    nNumVerts = nVerts;
    drawCount = nVerts;
    drawStart = 0;
    
    // Vertex Array object for this Array
#ifndef OPENGL_ES
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#endif
}

void ModelAsset::endBatchs(void)
{
#ifndef OPENGL_ES
    // Check to see if items have been added one at a time
    if(pVerts != NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, vao);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pVerts = NULL;
    }
    
    if(pColors != NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pColors = NULL;
    }
    
    if(pNormals != NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pNormals = NULL;
    }
    
    if (pTexCoords != NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        pTexCoords = NULL;
    }
    
    // Set up the vertex array object
    glBindVertexArray(vao);
#endif
    
    if(vbo !=0) {
        glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    if(uiColorArray != 0) {
        glEnableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    if(uiNormalArray != 0) {
        glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    if (uiTextureCoordArray != 0) {
        glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0),
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
#ifndef OPENGL_ES
    glBindVertexArray(0);
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

void ModelAsset::Vertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    // First see if the vertex array buffer has been created...
    if(vbo == 0) {    // Nope, we need to create it
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pVerts == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        pVerts = (M3DVector3f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pVerts[nVertsBuilding][0] = x;
    pVerts[nVertsBuilding][1] = y;
    pVerts[nVertsBuilding][2] = z;
    nVertsBuilding++;
}

void ModelAsset::Vertex3fv(M3DVector3f vVertex)
{
    // First see if the vertex array buffer has been created...
    if(vbo == 0) {    // Nope, we need to create it
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pVerts == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        pVerts = (M3DVector3f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pVerts[nVertsBuilding] = M3DVector3f(vVertex);
//    memcpy(pVerts[nVertsBuilding], glm::value_ptr(vVertex), sizeof(M3DVector3f));
    nVertsBuilding++;
}

// Unlike normal OpenGL immediate mode, you must specify a normal per vertex
// or you will get junk...
void ModelAsset::Normal3f(GLfloat x, GLfloat y, GLfloat z)
{
    // First see if the vertex array buffer has been created...
    if(uiNormalArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiNormalArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pNormals == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        pNormals = (M3DVector3f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pNormals[nVertsBuilding][0] = x;
    pNormals[nVertsBuilding][1] = y;
    pNormals[nVertsBuilding][2] = z;
}

// Ditto above
void ModelAsset::Normal3fv(M3DVector3f vNormal)
{
    // First see if the vertex array buffer has been created...
    if(uiNormalArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiNormalArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pNormals == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        pNormals = (M3DVector3f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pNormals[nVertsBuilding] = M3DVector3f(vNormal);
//    memcpy(pNormals[nVertsBuilding], glm::value_ptr(vNormal), sizeof(M3DVector3f));
}


void ModelAsset::Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    // First see if the vertex array buffer has been created...
    if(uiColorArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiColorArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pColors == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        pColors = (M3DVector4f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pColors[nVertsBuilding][0] = r;
    pColors[nVertsBuilding][1] = g;
    pColors[nVertsBuilding][2] = b;
    pColors[nVertsBuilding][3] = a;
}

void ModelAsset::Color4fv(M3DVector4f vColor)
{
    // First see if the vertex array buffer has been created...
    if(uiColorArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiColorArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pColors == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        pColors = (M3DVector4f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pColors[nVertsBuilding] = M3DVector4f(vColor);
//    memcpy(pColors[nVertsBuilding], glm::value_ptr(vColor), sizeof(M3DVector4f));
}

// Unlike normal OpenGL immediate mode, you must specify a texture coord
// per vertex or you will get junk...
void ModelAsset::MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t)
{
    // First see if the vertex array buffer has been created...
    if(uiTextureCoordArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiTextureCoordArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pTexCoords == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        pTexCoords = (M3DVector2f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pTexCoords[nVertsBuilding][0] = s;
    pTexCoords[nVertsBuilding][1] = t;
}

// Ditto above
void ModelAsset::MultiTexCoord2fv(GLuint texture, M3DVector2f vTexCoord)
{
    // First see if the vertex array buffer has been created...
    if(uiTextureCoordArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiTextureCoordArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pTexCoords == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray);
        pTexCoords = (M3DVector2f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pTexCoords[nVertsBuilding] = M3DVector2f(vTexCoord);
//    memcpy(pTexCoords, glm::value_ptr(vTexCoord), sizeof(M3DVector2f));
}

void ModelAsset::draw(void) const
{
    glBindVertexArray(vao);
    glDrawArrays(drawType, drawStart, drawCount);
    glBindVertexArray(0);
    
}
