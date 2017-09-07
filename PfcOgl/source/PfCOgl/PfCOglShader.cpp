//
//  PfCOglShader.cpp
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglShader.h"
#include <stdexcept>
#include <fstream>
#include <string>
#include <cassert>
#include <sstream>

using namespace PfCOgl;

Shader::Shader(const std::string& shaderCode, GLenum shaderType) :
_object(0),
_refCount(NULL)
{
    //create the shader object
    _object = glCreateShader(shaderType);
    if(_object == 0)
        throw std::runtime_error("glCreateShader failed");
    
    //set the source code
    const char* code = shaderCode.c_str();
    //param1:shader handler; param3:string类型的一维数组，相当于二维，即可以传进来多个shader
    //param2:param3的数量   parma4:shaders的字符串长度数组，每一个值对应param3的一个shader
    glShaderSource(_object, 1, (const GLchar**)&code, NULL);
    
    //compile
    glCompileShader(_object);
    
    //throw exception if compile error occurred
    //获取shader的相关信息
    //param1: shader handler
    /**
     param2: 获取的相关信息的类型
     GL_SHADER_TYPE:shader类型：vertex\fragment\geometry,eg:GL_VERTEX_SHADER
     GL_DELETE_STATUS:是否已经被标记为删除， GL_TRUE or GL_FALSE
     GL_COMPILE_STATUS：编译的状态，GL_TRUE or GL_FALSE
     GL_INFO_LOG_LENGTH:log信息的长度，没有返回0
     GL_SHADER_SOURCE_LENGTH：shader source的长度，包含null结束符，没有0
     */
    //param3: 返回请求的信息
    //这里获取编译状态
    GLint status;
    glGetShaderiv(_object, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {//编译失败
        
        std::string msg("Compile failure in shader:\n");
        
        GLint infoLogLength;
        //获取错误日志的长度
        glGetShaderiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        //获取错误日志内容
        //param1:handler  param2:设定返回的日志最大长度
        //param3:实际能返回的日志长度(out)  param4:存储日志内容
        glGetShaderInfoLog(_object, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        
        glDeleteShader(_object); _object = 0;
        throw std::runtime_error(msg);
    }
    
    _refCount = new unsigned;
    *_refCount = 1;
}

Shader::Shader(const Shader& other) :
_object(other._object),
_refCount(other._refCount)
{
    _retain();
}

Shader::~Shader() {
    //_refCount will be NULL if constructor failed and threw an exception
    if(_refCount) _release();
}

GLuint Shader::object() const {
    return _object;
}

Shader& Shader::operator = (const Shader& other) {
    _release();
    _object = other._object;
    _refCount = other._refCount;
    _retain();
    return *this;
}

Shader Shader::shaderFromFile(const std::string& filePath, GLenum shaderType) {
    //open file
    std::ifstream f;
    f.open(filePath.c_str(), std::ios::in | std::ios::binary);
    if(!f.is_open()){
        throw std::runtime_error(std::string("Failed to open file: ") + filePath);
    }
    
    //read whole file into stringstream buffer
    std::stringstream buffer;
    buffer << f.rdbuf();
    
    //return new shader
    Shader shader(buffer.str(), shaderType);
    return shader;
}

void Shader::_retain() {
    assert(_refCount);
    *_refCount += 1;
}

void Shader::_release() {
    assert(_refCount && *_refCount > 0);
    *_refCount -= 1;
    if(*_refCount == 0){
        glDeleteShader(_object); _object = 0;
        delete _refCount; _refCount = NULL;
    }
}
