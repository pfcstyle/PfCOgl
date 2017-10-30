//
//  PfCOPfCOglBatch.cpp
//  PfcOgl
//
//  Created by developer on 23/10/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglBatch.h"
#include "PfCOglProgram.h"
using namespace PfCOgl;

/////////////////////// OpenGL ES support on iPhone/iPad
#ifdef OPENGL_ES
#define GL_WRITE_ONLY   GL_WRITE_ONLY_OES
#define glMapBuffer     glMapBufferOES
#define glUnmapBuffer   glUnmapBufferOES
#endif

Batch::Batch(void): nNumTextureUnits(0), nNumVerts(0), pVerts(NULL), pNormals(NULL), pColors(NULL), pTexCoords(NULL), uiVertexArray(0),
uiNormalArray(0), uiColorArray(0), vertexArrayObject(0), bBatchDone(false), nVertsBuilding(0), uiTextureCoordArray(NULL)
{
}

Batch::~Batch(){
    // Vertex buffer objects
    if(uiVertexArray != 0)
        glDeleteBuffers(1, &uiVertexArray);
    
    if(uiNormalArray != 0)
        glDeleteBuffers(1, &uiNormalArray);
    
    if(uiColorArray != 0)
        glDeleteBuffers(1, &uiColorArray);
    
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
        glDeleteBuffers(1, &uiTextureCoordArray[i]);
    
#ifndef OPENGL_ES
    glDeleteVertexArrays(1, &vertexArrayObject);
#endif
    
    delete [] uiTextureCoordArray;
    delete [] pTexCoords;
}

// Start the primitive batch.
void Batch::begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits)
{
    primitiveType = primitive;
    nNumVerts = nVerts;
    
    if(nTextureUnits > 4)   // Limit to four texture units
        nTextureUnits = 4;
    
    nNumTextureUnits = nTextureUnits;
    
    if(nNumTextureUnits != 0) {
        uiTextureCoordArray = new GLuint[nNumTextureUnits];
        
        // An array of pointers to texture coordinate arrays
        pTexCoords = new M3DVector2f*[nNumTextureUnits];
        for(unsigned int i = 0; i < nNumTextureUnits; i++) {
            uiTextureCoordArray[i] = 0;
            pTexCoords[i] = NULL;
        }
    }
    
    // Vertex Array object for this Array
#ifndef OPENGL_ES
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
#endif
}

// Block Copy in vertex data
void Batch::CopyVertexData3f(M3DVector3f *vVerts)
{
    // First time, create the buffer object, allocate the space
    if(uiVertexArray == 0) {
        glGenBuffers(1, &uiVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, vVerts, GL_DYNAMIC_DRAW);
    }
    else    { // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * nNumVerts, vVerts);
        pVerts = NULL;
    }
}

// Block copy in normal data
void Batch::CopyNormalDataf(M3DVector3f *vNorms)
{
    // First time, create the buffer object, allocate the space
    if(uiNormalArray == 0) {
        glGenBuffers(1, &uiNormalArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, vNorms, GL_DYNAMIC_DRAW);
    }
    else {    // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, uiNormalArray);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * nNumVerts, vNorms);
        pNormals = NULL;
    }
}

void Batch::CopyColorData4f(M3DVector4f *vColors)
{
    // First time, create the buffer object, allocate the space
    if(uiColorArray == 0) {
        glGenBuffers(1, &uiColorArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * nNumVerts, vColors, GL_DYNAMIC_DRAW);
    }
    else {    // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, uiColorArray);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * nNumVerts, vColors);
        pColors = NULL;
    }
}

void Batch::CopyTexCoordData2f(M3DVector2f *vTexCoords, GLuint uiTextureLayer)
{
    // First time, create the buffer object, allocate the space
    if(uiTextureCoordArray[uiTextureLayer] == 0) {
        glGenBuffers(1, &uiTextureCoordArray[uiTextureLayer]);
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[uiTextureLayer]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * nNumVerts, vTexCoords, GL_DYNAMIC_DRAW);
    }
    else {    // Just bind to existing object
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[uiTextureLayer]);
        
        // Copy the data in
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 2 * nNumVerts, vTexCoords);
        pTexCoords[uiTextureLayer] = NULL;
    }
}

// Bind everything up in a little package
void Batch::end(void)
{
#ifndef OPENGL_ES
    // Check to see if items have been added one at a time
    if(pVerts != NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
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
    
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
        if(pTexCoords[i] != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[i]);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            pTexCoords[i] = NULL;
        }
    
    // Set up the vertex array object
    glBindVertexArray(vertexArrayObject);
#endif
    
    if(uiVertexArray !=0) {
        glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
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
    
    // How many texture units
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
        if(uiTextureCoordArray[i] != 0) {
            glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0 + i),
            glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[i]);
            glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0 + i, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    
    bBatchDone = true;
#ifndef OPENGL_ES
    glBindVertexArray(0);
#endif
}

// Just start over. No reallocations, etc.
void Batch::reset(void)
{
    bBatchDone = false;
    nVertsBuilding = 0;
}

// Add a single vertex to the end of the array
void Batch::Vertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    // First see if the vertex array buffer has been created...
    if(uiVertexArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pVerts == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
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

void Batch::Vertex3fv(M3DVector3f &vVertex)
{
    // First see if the vertex array buffer has been created...
    if(uiVertexArray == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiVertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pVerts == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
        pVerts = (M3DVector3f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pVerts[nVertsBuilding] = vVertex;
    nVertsBuilding++;
}

// Unlike normal OpenGL immediate mode, you must specify a normal per vertex
// or you will get junk...
void Batch::Normal3f(GLfloat x, GLfloat y, GLfloat z)
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
void Batch::Normal3fv(M3DVector3f &vNormal)
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
    pNormals[nVertsBuilding] = vNormal;
}


void Batch::Color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
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

void Batch::Color4fv(M3DVector4f &vColor)
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
    pColors[nVertsBuilding] = vColor;
}

// Unlike normal OpenGL immediate mode, you must specify a texture coord
// per vertex or you will get junk...
void Batch::MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t)
{
    // First see if the vertex array buffer has been created...
    if(uiTextureCoordArray[texture] == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiTextureCoordArray[texture]);
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[texture]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pTexCoords[texture] == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[texture]);
        pTexCoords[texture] = (M3DVector2f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pTexCoords[texture][nVertsBuilding][0] = s;
    pTexCoords[texture][nVertsBuilding][1] = t;
}

// Ditto above
void Batch::MultiTexCoord2fv(GLuint texture, M3DVector2f &vTexCoord)
{
    // First see if the vertex array buffer has been created...
    if(uiTextureCoordArray[texture] == 0) {    // Nope, we need to create it
        glGenBuffers(1, &uiTextureCoordArray[texture]);
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[texture]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * nNumVerts, NULL, GL_DYNAMIC_DRAW);
    }
    
    // Now see if it's already mapped, if not, map it
    if(pTexCoords[texture] == NULL) {
        glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[texture]);
        pTexCoords[texture] = (M3DVector2f *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }
    
    // Ignore if we go past the end, keeps things from blowing up
    if(nVertsBuilding >= nNumVerts)
        return;
    
    // Copy it in...
    pTexCoords[texture] = &vTexCoord;
}


void Batch::draw(void)
{
    if(!bBatchDone)
        return;
    
#ifndef OPENGL_ES
    // Set up the vertex array object
    glBindVertexArray(vertexArrayObject);
#else
    if(uiVertexArray !=0) {
        glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER, uiVertexArray);
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
    
    // How many texture units
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
        if(uiTextureCoordArray[i] != 0) {
            glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0 + i),
            glBindBuffer(GL_ARRAY_BUFFER, uiTextureCoordArray[i]);
            glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0 + i, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
#endif
    
    
    glDrawArrays(primitiveType, 0, nNumVerts);
    
#ifndef OPENGL_ES
    glBindVertexArray(0);
#else
    glDisableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
    glDisableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
    glDisableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
    
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
        if(uiTextureCoordArray[i] != 0)
            glDisableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0 + i);
    
#endif
}
