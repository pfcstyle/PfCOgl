//
//  PfCOglProgram.cpp
//  PfcOgl
//
//  Created by developer on 25/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglProgram.h"
#include "PfCOglStockShader.h"
#include "math3d.h"
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

using namespace PfCOgl;

Program::Program(void):_object(0){
    // Set stock shader handles to 0... uninitialized
    for(unsigned int i = 0; i < GLT_SHADER_LAST; i++)
        uiStockShaders[i] = 0;
}



Program::Program(const std::vector<Shader>& shaders) :
_object(0)
{
    _object = getProgramByShaders(shaders);
}

Program::~Program() {
    //might be 0 if ctor fails by throwing exception
    
    if(uiStockShaders[0] != 0) {
        unsigned int i;
        for(i = 0; i < GLT_SHADER_LAST; i++)
            glDeleteProgram(uiStockShaders[i]);
    }else{
        if(_object != 0) glDeleteProgram(_object);
    }
}

bool Program::initializeStockShaders(){
    uiStockShaders[GLT_SHADER_IDENTITY] = getProgramByShaderSrcWithAttributes(szIdentityShaderVP, szIdentityShaderFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
    uiStockShaders[GLT_SHADER_FLAT]=getProgramByShaderSrcWithAttributes(szFlatShaderVP, szFlatShaderFP, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
    uiStockShaders[GLT_SHADER_SHADED] = getProgramByShaderSrcWithAttributes(szShadedVP, szShadedFP, 2,
                                                                                       GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
    
    
    uiStockShaders[GLT_SHADER_DEFAULT_LIGHT] = getProgramByShaderSrcWithAttributes(szDefaultLightVP, szDefaultLightFP, 2,
                                                                                      GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
    
    uiStockShaders[GLT_SHADER_POINT_LIGHT_DIFF] = getProgramByShaderSrcWithAttributes(szPointLightDiffVP, szPointLightDiffFP, 2,
                                                                                      GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
    
    uiStockShaders[GLT_SHADER_TEXTURE_REPLACE]  = getProgramByShaderSrcWithAttributes(szTextureReplaceVP, szTextureReplaceFP, 2,
                                                                                      GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    
    uiStockShaders[GLT_SHADER_TEXTURE_MODULATE] = getProgramByShaderSrcWithAttributes(szTextureModulateVP, szTextureModulateFP, 2,
                                                                                      GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    
    uiStockShaders[GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF] = getProgramByShaderSrcWithAttributes(szTexturePointLightDiffVP, szTexturePointLightDiffFP, 3,
                                                                                              GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    
    
    uiStockShaders[GLT_SHADER_TEXTURE_RECT_REPLACE] = getProgramByShaderSrcWithAttributes(szTextureRectReplaceVP, szTextureRectReplaceFP, 2,
                                                                                          GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
//    _object = uiStockShaders[0];
    if(uiStockShaders[0] != 0)
        return true;
    
    return false;
    //    uiStockShaders[GLT_SHADER_IDENTITY] = Program()
}

GLuint Program::getProgramByShaders(const std::vector<Shader> &shaders){
    GLuint hReturn = 0;
    if(shaders.size() <= 0){
        throw std::runtime_error("No shaders were provided to create the program");
        return hReturn;
    }
    
    //create the program object
    hReturn = glCreateProgram();
    if(hReturn == 0){
        throw std::runtime_error("glCreateProgram failed");
        return hReturn;
    }
    
    //attach all the shaders
    for(unsigned i = 0; i < shaders.size(); ++i)
        glAttachShader(hReturn, shaders[i].object());
    
    //link the shaders together
    glLinkProgram(hReturn);
    
    //detach all the shaders
    for(unsigned i = 0; i < shaders.size(); ++i)
        glDetachShader(hReturn, shaders[i].object());
    
    //throw exception if linking failed
    //获取program的link状态信息
    //param1:handler
    /**
     param2:
     GL_DELETE_STATUS, GL_LINK_STATUS, GL_VALIDATE_STATUS, GL_INFO_LOG_LENGTH, GL_ATTACHED_SHADERS, GL_ACTIVE_ATTRIBUTES, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, GL_ACTIVE_UNIFORMS, GL_ACTIVE_UNIFORM_BLOCKS, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, GL_ACTIVE_UNIFORM_MAX_LENGTH, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, GL_TRANSFORM_FEEDBACK_VARYINGS, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, GL_GEOMETRY_VERTICES_OUT, GL_GEOMETRY_INPUT_TYPE, and GL_GEOMETRY_OUTPUT_TYPE.
     
     这里各种信息，获取的都是对应的数量或者长度或者true or false这种简略信息，需要调用
     具体的信息函数获取详细信息
     */
    //param3:存储对应的信息
    GLint status;
    glGetProgramiv(hReturn, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");
        
        GLint infoLogLength;
        glGetProgramiv(hReturn, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(hReturn, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteProgram(hReturn); hReturn = 0;
        throw std::runtime_error(msg);
    }
    return hReturn;
}

GLint Program::useStockShader(PfCOgl::GLT_STOCK_SHADER shaderId, ...){
    if(shaderId >= GLT_SHADER_LAST)
        return -1;
    
    // List of uniforms
    va_list uniformList;
    va_start(uniformList, shaderId);
    
    // Bind to the correct shader
    _object = uiStockShaders[shaderId];
    glUseProgram(_object);
    
    // Set up the uniforms
    int                iInteger;
    M3DMatrix44f* mvpMatrix;
    M3DMatrix44f*  pMatrix;
    M3DMatrix44f*  mvMatrix;
    M3DVector4f*  vColor;
    M3DVector3f*  vLightPos;
    
    switch(shaderId)
    {
        case GLT_SHADER_FLAT:            // Just the modelview projection matrix and the color
            mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvpMatrix", *mvpMatrix);
            
            vColor = va_arg(uniformList, M3DVector4f*);
            setUniform("vColor", *vColor);
            break;
            
        case GLT_SHADER_TEXTURE_RECT_REPLACE:
        case GLT_SHADER_TEXTURE_REPLACE:    // Just the texture place
            mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvpMatrix", *mvpMatrix);
            
            iInteger = va_arg(uniformList, int);
            setUniform("textureUnit0", iInteger);
            break;
            
        case GLT_SHADER_TEXTURE_MODULATE: // Multiply the texture by the geometry color
            mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvpMatrix", *mvpMatrix);
            
            vColor = va_arg(uniformList, M3DVector4f*);
            setUniform("vColor", *vColor);
            
            iInteger = va_arg(uniformList, int);
            setUniform("textureUnit0", iInteger);
            break;
            
            
        case GLT_SHADER_DEFAULT_LIGHT:
            mvMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvMatrix", *mvMatrix);
            
            pMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("pMatrix", *pMatrix);
            
            vColor = va_arg(uniformList, M3DVector4f*);
            setUniform("vColor", *vColor);
            break;
            
        case GLT_SHADER_POINT_LIGHT_DIFF:
            mvMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvMatrix", *mvMatrix);
            
            pMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("pMatrix", *pMatrix);
            
            vLightPos = va_arg(uniformList, M3DVector3f*);
            setUniform("vLightPos", *vLightPos);
            
            vColor = va_arg(uniformList, M3DVector4f*);
            setUniform("vColor", *vColor);
            break;
            
        case GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF:
            mvMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvMatrix", *mvMatrix);
            
            pMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("pMatrix", *pMatrix);
            
            vLightPos = va_arg(uniformList, M3DVector3f*);
            setUniform("vLightPos", *vLightPos);
            
            vColor = va_arg(uniformList, M3DVector4f*);
            setUniform("vColor", *vColor);
            
            iInteger = va_arg(uniformList, int);
            setUniform("textureUnit0", iInteger);
            break;
            
            
        case GLT_SHADER_SHADED:        // Just the modelview projection matrix. Color is an attribute
            mvpMatrix = va_arg(uniformList, M3DMatrix44f*);
            setUniform("mvpMatrix", *mvpMatrix);
            break;
            
        case GLT_SHADER_IDENTITY:    // Just the Color
            vColor = va_arg(uniformList, M3DVector4f*);
            setUniform("vColor", *vColor);
        default:
            break;
    }
    va_end(uniformList);
    
    return _object;
}

GLuint Program::getProgramByShaderSrcWithAttributes(const char *szVertexSrc, const char *szFragmentSrc, ...){
    GLuint hReturn = 0;
    Shader vertexShader(szVertexSrc, GL_VERTEX_SHADER);
    Shader fragShader(szFragmentSrc, GL_FRAGMENT_SHADER);
    //create the program object
    hReturn = glCreateProgram();
    if(hReturn == 0)
        throw std::runtime_error("glCreateProgram failed");
    
    //attach all the shaders
    glAttachShader(hReturn, vertexShader.object());
    glAttachShader(hReturn, fragShader.object());
    
    //bind attributes
    va_list attributeList;
    va_start(attributeList, szFragmentSrc);
    char* szNextArg;
    int iArgCount = va_arg(attributeList, int);    // Number of attributes
    for (int i=0; i<iArgCount; i++) {
        int index = va_arg(attributeList, int);
        szNextArg = va_arg(attributeList, char*);
        glBindAttribLocation(hReturn, index, szNextArg);
    }
    va_end(attributeList);
    //link the shaders together
    glLinkProgram(hReturn);
    
    //detach all the shaders
    glDetachShader(hReturn, vertexShader.object());
    glDetachShader(hReturn, fragShader.object());
    
    //throw exception if linking failed
    //获取program的link状态信息
    //param1:handler
    /**
     param2:
     GL_DELETE_STATUS, GL_LINK_STATUS, GL_VALIDATE_STATUS, GL_INFO_LOG_LENGTH, GL_ATTACHED_SHADERS, GL_ACTIVE_ATTRIBUTES, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, GL_ACTIVE_UNIFORMS, GL_ACTIVE_UNIFORM_BLOCKS, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, GL_ACTIVE_UNIFORM_MAX_LENGTH, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, GL_TRANSFORM_FEEDBACK_VARYINGS, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, GL_GEOMETRY_VERTICES_OUT, GL_GEOMETRY_INPUT_TYPE, and GL_GEOMETRY_OUTPUT_TYPE.
     
     这里各种信息，获取的都是对应的数量或者长度或者true or false这种简略信息，需要调用
     具体的信息函数获取详细信息
     */
    //param3:存储对应的信息
    GLint status;
    glGetProgramiv(hReturn, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");
        
        GLint infoLogLength;
        glGetProgramiv(hReturn, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(hReturn, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteProgram(hReturn); hReturn = 0;
        throw std::runtime_error(msg);
    }
    return hReturn;
}

GLuint Program::object() const {
    return _object;
}

void Program::use() const {
    glUseProgram(_object);
}

bool Program::isInUse() const {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    return (currentProgram == (GLint)_object);
}

void Program::stopUsing() const {
    assert(isInUse());
    glUseProgram(0);
}

GLint Program::attrib(const GLchar* attribName) const {
    if(!attribName)
        throw std::runtime_error("attribName was NULL");
    //获取GL某属性的索引  找不到返回-1
    GLint attrib = glGetAttribLocation(_object, attribName);
    if(attrib == -1)
        throw std::runtime_error(std::string("Program attribute not found: ") + attribName);
    
    return attrib;
}

GLint Program::uniform(const GLchar* uniformName) const {
    if(!uniformName)
        throw std::runtime_error("uniformName was NULL");
    
    GLint uniform = glGetUniformLocation(_object, uniformName);
    if(uniform == -1)
        throw std::runtime_error(std::string("Program uniform not found: ") + uniformName);
    
    return uniform;
}

#define ATTRIB_N_UNIFORM_SETTERS(OGL_TYPE, TYPE_PREFIX, TYPE_SUFFIX) \
\
void Program::setAttrib(const GLchar* name, OGL_TYPE v0) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX (attrib(name), v0); } \
void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX (attrib(name), v0, v1); } \
void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX (attrib(name), v0, v1, v2); } \
void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX (attrib(name), v0, v1, v2, v3); } \
\
void Program::setAttrib1v(const GLchar* name, const OGL_TYPE* v) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX ## v (attrib(name), v); } \
void Program::setAttrib2v(const GLchar* name, const OGL_TYPE* v) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX ## v (attrib(name), v); } \
void Program::setAttrib3v(const GLchar* name, const OGL_TYPE* v) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX ## v (attrib(name), v); } \
void Program::setAttrib4v(const GLchar* name, const OGL_TYPE* v) \
{ assert(isInUse()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX ## v (attrib(name), v); } \
\
void Program::setUniform(const GLchar* name, OGL_TYPE v0) \
{ assert(isInUse()); glUniform1 ## TYPE_SUFFIX (uniform(name), v0); } \
void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
{ assert(isInUse()); glUniform2 ## TYPE_SUFFIX (uniform(name), v0, v1); } \
void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
{ assert(isInUse()); glUniform3 ## TYPE_SUFFIX (uniform(name), v0, v1, v2); } \
void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
{ assert(isInUse()); glUniform4 ## TYPE_SUFFIX (uniform(name), v0, v1, v2, v3); } \
\
void Program::setUniform1v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
{ assert(isInUse()); glUniform1 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
void Program::setUniform2v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
{ assert(isInUse()); glUniform2 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
void Program::setUniform3v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
{ assert(isInUse()); glUniform3 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
void Program::setUniform4v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
{ assert(isInUse()); glUniform4 ## TYPE_SUFFIX ## v (uniform(name), count, v); }

ATTRIB_N_UNIFORM_SETTERS(GLfloat, , f);
ATTRIB_N_UNIFORM_SETTERS(GLdouble, , d);
ATTRIB_N_UNIFORM_SETTERS(GLint, I, i);
ATTRIB_N_UNIFORM_SETTERS(GLuint, I, ui);

void Program::setUniformMatrix2(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    assert(isInUse());
    glUniformMatrix2fv(uniform(name), count, transpose, v);
}

void Program::setUniformMatrix3(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    assert(isInUse());
    glUniformMatrix3fv(uniform(name), count, transpose, v);
}

void Program::setUniformMatrix4(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    assert(isInUse());
    glUniformMatrix4fv(uniform(name), count, transpose, v);
}

void Program::setUniform(const GLchar* name, const glm::mat2& m, GLboolean transpose) {
    assert(isInUse());
    glUniformMatrix2fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* name, const glm::mat3& m, GLboolean transpose) {
    assert(isInUse());
    glUniformMatrix3fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* name, const glm::mat4& m, GLboolean transpose) {
    assert(isInUse());
    glUniformMatrix4fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* uniformName, const glm::vec3& v) {
    setUniform3v(uniformName, glm::value_ptr(v));
}

void Program::setUniform(const GLchar* uniformName, const glm::vec4& v) {
    setUniform4v(uniformName, glm::value_ptr(v));
}



