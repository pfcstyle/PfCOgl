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
#include "PfCOglBatch.h"
#include "PfCOglCamera.h"

using namespace std;
using namespace PfCOgl;

GLFWwindow *gWindow;
list<ModelInstance> mModelInstances;
Program mPgrogram;
Camera gCamera;
int nStep = 0;
ModelAsset *mModelAsset;

const M3DVector2f SCREEN_SIZE(800, 600);


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
    gWindow = glfwCreateWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "Hello World", NULL, NULL);
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
GLfloat blockSize = 0.2f;
void loadAssetAndInstances() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
    ModelInstance square;
    mModelAsset = new ModelAsset();
    mModelAsset->drawCount = 4;
    mModelAsset->drawType = GL_TRIANGLE_FAN;
    mModelAsset->drawStart = 0;
    mModelAsset->shaders = new Program();
    mModelAsset->shaders->initializeStockShaders();
    GLfloat vVerts[] = {
        -blockSize, -blockSize,//square
        0.0f,0.25f, 0.25f,//green
        0.0f,-0.75f, 0.25f,//red
        -0.75f, -0.75f, 0.0f,//blue
        0.25f, -0.75f, 0.0f,//black
        
        blockSize, -blockSize, 0.0f,
        0.75f, 0.25f, 0.0f,
        -0.25f, 0.25f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        
        blockSize,  blockSize, 0.0f,
        0.75f, 0.75f, 0.0f,
        -0.25f, 0.75f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        0.75f, -0.25f, 0.0f,
        
        -blockSize,  blockSize, 0.0f,
        //green
        0.25f, 0.75f, 0.0f,
        //red
        -0.75f, 0.75f, 0.0f,
        //blue
        -0.75f, -0.25f, 0.0f,
        //black
        0.25f, -0.25f, 0.0f,
    };
    mModelAsset->bindData(vVerts);
    square.asset = mModelAsset;
    square.begin();
    square.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 15, NULL);
    square.end();
    
    ModelInstance green;
    green.asset = mModelAsset;
    green.begin();
    green.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 15, (const GLvoid*)(sizeof(GLfloat) * 3));
    green.end();

    ModelInstance red;
    red.asset = mModelAsset;
    red.begin();
    red.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 15, (const GLvoid*)(sizeof(GLfloat) * 3 * 2));
    red.end();

    ModelInstance blue;
    blue.asset = mModelAsset;
    blue.begin();
    blue.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 15, (const GLvoid*)(sizeof(GLfloat) * 3 * 3));
    blue.end();

    ModelInstance black;
    black.asset = mModelAsset;
    black.begin();
    black.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 15, (const GLvoid*)(sizeof(GLfloat) * 3 * 4));
    black.end();
    
    mModelInstances.push_back(green);
    mModelInstances.push_back(red);
    mModelInstances.push_back(blue);
    mModelInstances.push_back(black);
    mModelInstances.push_back(square);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 0.5f };
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    std::list<ModelInstance>::const_iterator it;
    int index = 0;
    for (it = mModelInstances.begin(); it != mModelInstances.end(); it++) {
        const ModelInstance instance = *it;
        switch (index) {
            case 0:
//                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vGreen);
//                instance.draw();
////                instance.asset->shaders->stopUsing();
//                break;
//            case 1:
//                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vRed);
//                instance.draw();
//                break;
//            case 2:
//                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vBlue);
//                instance.draw();
//                break;
//            case 3:
//                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vBlack);
//                instance.draw();
//                break;
            case 4:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vRed);
                instance.draw();
                instance.asset->shaders->stopUsing();
                glDisable(GL_BLEND);
                break;
            default:
                break;
        }
//        instance.asset->shaders->stopUsing();
        index ++;
    }
    
    
    
    // Perform the buffer swap to display back buffer
    glfwSwapBuffers(gWindow);
}

void Update(float secondsElapsed) {
    //rotate the first instance in `gInstances`
//    const float moveSpeed = 4.0; //units per second
    if(glfwGetKey(gWindow, 'S')){
        
    } else if(glfwGetKey(gWindow, 'W')){
        
    }
    if(glfwGetKey(gWindow, 'A')){
        
    } else if(glfwGetKey(gWindow, 'D')){
        
    }
    if(glfwGetKey(gWindow, 'Z')){
        
    } else if(glfwGetKey(gWindow, 'X')){
        
    }
    
    if(glfwGetKey(gWindow, 'N'))
    {
        nStep++;
        
        if(nStep > 6)
            nStep = 0;
    }
    
    
}

void AppMain(){
    //创建窗口
    createWindow();
    //init glew
    initGlew();
    // init shaders and program
    loadAssetAndInstances();
    //render
    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(gWindow)) {
        glfwPollEvents();
        
        // update the scene based on the time elapsed since last update
        float thisTime = (float)glfwGetTime();
        Update(thisTime - lastTime);
        lastTime = thisTime;
        
        RenderScene();
        
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }
    
    glfwTerminate();
}
