//
//  main.cpp
//  PfcOgl
//
//  Created by developer on 24/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>

// Third Ref
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// self code
#include "platform.hpp"
#include "PfCOglModelAsset.h"
#include "PfCOglModelInstance.h"
#include "PfCOglProgram.h"
#include "PfCOglShader.h"

using namespace std;
using namespace PfCOgl;

ModelAsset *mModelAsset;
GLFWwindow *gWindow;
GLuint mVAO;
list<ModelInstance> mModelInstances;

void AppMain();
void onError(int errorCode, const char *errorMsg);

int main(int argc, char **argv){
    try {
        AppMain();
    } catch (const exception &e) {
        cerr << "ERROR: "<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void createWindow(){
    // set glfw error handler
    glfwSetErrorCallback(onError);
    if (!glfwInit()) {
        throw runtime_error("glfw init error");
    }
    //设置glfw支持的opengl version  相当于写死了版本号  可以不设置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    //只使用opengl的核心功能
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //向前兼容
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //禁止改变窗口大小
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //创建窗口  宽 高 标题 moniter(glfwGetPrimaryMonitor)指定了moniter就是全屏
    //glfwSetWindowMonitor可以用来切换全屏和非全屏
    //最后一个参数是用来与这个创建的window共享资源的window,没有就是null
    gWindow = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!gWindow) {
        throw runtime_error("window init error");
    }
    glfwMakeContextCurrent(gWindow);
    
}

void onError(int errorCode, const char *errorMsg){
    throw runtime_error(errorMsg);
}

void initGlew(){
    //防止glew crash on macos
    glewExperimental = GL_TRUE;
    //init glew
    if (glewInit() != GLEW_OK) {
        throw runtime_error("glew init error");
    }
    if (!GLEW_VERSION_3_2) {
        throw runtime_error("opengl version 3.2 is unavailable");
    }
    
}


Program *loadShaders(){
    vector<Shader> shaders;
    //load vertex
    shaders.push_back(Shader::shaderFromFile("vertex-shader.txt", GL_VERTEX_SHADER));
    
    //load tessellation_control
//    shaders.push_back(Shader::shaderFromFile("tessellation_control.txt", GL_TESS_CONTROL_SHADER));
//    //load tessellation_evaluation
//    shaders.push_back(Shader::shaderFromFile("tessellation_evaluation.txt", GL_TESS_EVALUATION_SHADER));
//    //geometry shader
//    shaders.push_back(Shader::shaderFromFile("geometry-shader.txt", GL_GEOMETRY_SHADER));
    
    //load fragment
    shaders.push_back(Shader::shaderFromFile("fragment-shader.txt", GL_FRAGMENT_SHADER));
    
    //compile program
    return new Program(shaders);
    
}

void createVBOAndVAO(){
    //生成buffer name(unique id)   param1:生成的数量
//    glGenBuffers(1,&mModelAsset->vbo);
//    //绑定name  param1：绑定的buffer类型
//    /**
//     GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, or GL_UNIFORM_BUFFER.
//     直白来说，影响的是绘图时所使用的绘图函数，具体参考文档
//     比如本行代码，需要使用drawArray
//     
//     */
//    glBindBuffer(GL_ARRAY_BUFFER,mModelAsset->vbo);
//    //创建定点值  范围是-1~1
//    GLfloat vertex_data[] = {
//        0.0f,0.8f,0.0f,
//        -0.8f,-0.8f,0.0f,
//        0.8f,-0.8f,0.0f,
//    };
//    //填充数据了  填充到之前绑定的VBO中
//    //param1:跟之前绑定的buffer要一致
//    //param2:绑定数据的大小    param3:要绑定的数据
//    //param4:绑定的数据如何使用
//    /**
//     GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
//     STREAM:修改一次，使用几次
//     STATIC:修改一次，使用很多次
//     DYNAMIC:修改多次，使用多次
//     DRAW:被application修改，并且作为GL绘图和图像的source
//     READ:被GL修改，当application查询时可以返回
//     COPY:被GL修改，并且作为GL绘图和图像的source
//     */
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    
    glGenVertexArrays(1,&mModelAsset->vao);
//    glBindVertexArray(mModelAsset->vao);
//    
//    //启用GL中的某个属性，传入属性对应的index
//    glEnableVertexAttribArray(mModelAsset->shaders->attrib("vert"));
//    //对属性赋值
//    /**
//     param1:属性索引
//     param2:每次读取几个数据  或者可以理解成传入的是几维向量
//     param3:数据类型
//     param4:是否单位化
//     param5:读取的步长，默认0
//     param6:从哪开始读，默认0
//     */
//    glVertexAttribPointer(mModelAsset->shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);
////
////    
////    //解绑
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
    
    //设置细分曲面控制点数目 默认是3
//    glPatchParameteri(GL_PATCH_VERTICES, 3);
    //设置绘画模式  使用线  无颜色
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //设置点大小
//    glPointSize(5.0);
}

void loadModelAsset(){
    mModelAsset = new ModelAsset();
    mModelAsset->shaders = loadShaders();
    mModelAsset->drawType = GL_TRIANGLES;
//    mModelAsset->drawType = GL_PATCHES;
    mModelAsset->drawStart = 0;
    mModelAsset->drawCount = 3;
    
    createVBOAndVAO();
}

void createInstances(){
    ModelInstance triangle;
    triangle.asset = mModelAsset;
    triangle.transform = glm::mat4();
    mModelInstances.push_back(triangle);
}

void renderInstance(const ModelInstance& inst, float currentTime) {
    ModelAsset* asset = inst.asset;
    Program* shaders = asset->shaders;
    
    //bind the shaders
    shaders->use();
    
    //bind VAO and draw
    glBindVertexArray(asset->vao);
    
    //set attributs
    GLfloat attrib[] = { (float)sin(currentTime) * 0.5f, (float)cos(currentTime) * 0.6f, 0.0f };
    shaders->setAttrib("offset", attrib[0], attrib[1], attrib[2]);
//    shaders->setAttrib("color", 0.5f, 0.2f, 0.8f, 1.0f);
    //set the shader uniforms
//    shaders->setUniform("camera", gCamera.matrix());
//    shaders->setUniform("model", inst.transform);
//    shaders->setUniform("tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0
//    shaders->setUniform("light.position", gLight.position);
//    shaders->setUniform("light.intensities", gLight.intensities);
    
    //bind the texture
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, asset->texture->object());
    
    //绘画  模式  开始索引  定点数量
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);
    
    //unbind everything
    glBindVertexArray(0);
//    glBindTexture(GL_TEXTURE_2D, 0);
    shaders->stopUsing();
}


void render(float currentTime){
    //清空画布为黑色rgba
//    glClearColor(0.5, 0.5, 0.5, 1);
    const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f, (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, color);
    //glClear sets the bitplane area of the window to values previously selected by glClearColor, glClearDepth, and glClearStencil
    /**
     glClear 可以清空前面通过glClearColor, glClearDepth, and glClearStencil设置的
     值   GL_STENCIL_BUFFER_BIT：模板缓冲区
     */
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // render all the instances
    std::list<ModelInstance>::const_iterator it;
    for(it = mModelInstances.begin(); it != mModelInstances.end(); ++it){
        renderInstance(*it, currentTime);
    }
    
    glfwSwapBuffers(gWindow);
    
}

void Update(float secondsElapsed) {
    //rotate the first instance in `gInstances`
    
}

void AppMain(){
    //创建窗口
    createWindow();
    //init glew
    initGlew();
    //加载模型
    loadModelAsset();
    //创建实例
    createInstances();
    //render
    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(gWindow)) {
        glfwPollEvents();
        
        // update the scene based on the time elapsed since last update
        float thisTime = (float)glfwGetTime();
        Update(thisTime - lastTime);
        lastTime = thisTime;
        
        render(thisTime);
    }
    
    glfwTerminate();
}
