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
ModelInstance square;
Program mProgram;
Camera gCamera;
int nStep = 0;

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
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize,  blockSize, 0.0f,
    -blockSize,  blockSize, 0.0f
};
void loadAssetAndInstances() {
    glClearColor(1, 1, 1, 1); // white

    square.asset = new ModelAsset();
    square.asset->drawCount = 4;
    square.asset->drawType = GL_TRIANGLE_FAN;
    square.asset->drawStart = 0;
    square.asset->shaders = new Program();
    square.asset->shaders->initializeStockShaders();
    square.asset->bindData(vVerts, 3 * 4);
    square.begin();
    square.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    square.end();
    
    ModelInstance green;
    green.asset = new ModelAsset();
    green.asset->drawCount = 4;
    green.asset->drawType = GL_TRIANGLE_FAN;
    green.asset->drawStart = 0;
    green.asset->shaders = new Program();
    green.asset->shaders->initializeStockShaders();
    GLfloat vBlock[] = {
        0.25f, 0.25f, 0.0f,
        0.75f, 0.25f, 0.0f,
        0.75f, 0.75f, 0.0f,
        0.25f, 0.75f, 0.0f
    };
    green.asset->bindData(vBlock, 3 * 4);
    green.begin();
    green.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    green.end();

    ModelInstance red;
    red.asset = new ModelAsset();
    red.asset->drawCount = 4;
    red.asset->drawType = GL_TRIANGLE_FAN;
    red.asset->drawStart = 0;
    red.asset->shaders = new Program();
    red.asset->shaders->initializeStockShaders();
    GLfloat vBlock2[] = {
        -0.75f, 0.25f, 0.0f,
        -0.25f, 0.25f, 0.0f,
        -0.25f, 0.75f, 0.0f,
        -0.75f, 0.75f, 0.0f
    };
    red.asset->bindData(vBlock2, 3 * 4);
    red.begin();
    red.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    red.end();

    ModelInstance blue;
    blue.asset = new ModelAsset();
    blue.asset->drawCount = 4;
    blue.asset->drawType = GL_TRIANGLE_FAN;
    blue.asset->drawStart = 0;
    blue.asset->shaders = new Program();
    blue.asset->shaders->initializeStockShaders();
    GLfloat vBlock3[] = {
        -0.75f, -0.75f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        -0.75f, -0.25f, 0.0f
    };
    blue.asset->bindData(vBlock3, 12);
    blue.begin();
    blue.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    blue.end();

    ModelInstance black;
    black.asset = new ModelAsset();
    black.asset->drawCount = 4;
    black.asset->drawType = GL_TRIANGLE_FAN;
    black.asset->drawStart = 0;
    black.asset->shaders = new Program();
    black.asset->shaders->initializeStockShaders();
    GLfloat vBlock4[] = {
        0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.75f, -0.25f, 0.0f,
        0.25f, -0.25f, 0.0f
    };
    black.asset->bindData(vBlock4, 12);
    black.begin();
    black.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    black.end();
    
    mModelInstances.push_back(green);
    mModelInstances.push_back(red);
    mModelInstances.push_back(blue);
    mModelInstances.push_back(black);
    mModelInstances.push_back(square);
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 0.5f };
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    std::list<ModelInstance>::iterator it;
    int index = 0;
    for (it = mModelInstances.begin(); it != mModelInstances.end(); it++) {
        const ModelInstance instance = *it;
        switch (index) {
            case 0:
                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vGreen);
                instance.draw();
                break;
            case 1:
                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vRed);
                instance.draw();
                break;
            case 2:
                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vBlue);
                instance.draw();
                break;
            case 3:
                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vBlack);
                instance.draw();
                break;
            case 4:
                //开启混合 Cf = (Cs*S) + (Cd * D)  还可以设置其他种类的方程式
                //其中S和D使用glBlendFunc()设置
                //或者使用glBlendFuncSeparate()更灵活的设置
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                instance.asset->shaders->useStockShader(GLT_SHADER_IDENTITY, vRed);
                instance.draw();
                glDisable(GL_BLEND);
                break;
            default:
                break;
        }
        instance.asset->shaders->stopUsing();
        index ++;
    }
    
    
    
    // Perform the buffer swap to display back buffer
    glfwSwapBuffers(gWindow);
}

void Update(float secondsElapsed) {
    //rotate the first instance in `gInstances`
//    const float moveSpeed = 4.0; //units per second
    GLfloat stepSize = 0.025f;
    
    GLfloat blockX = vVerts[0];   // Upper left X
    GLfloat blockY = vVerts[7];  // Upper left Y
    
    if(glfwGetKey(gWindow, 'S')){
        blockY -= stepSize;
    } else if(glfwGetKey(gWindow, 'W')){
        blockY += stepSize;
    }
    if(glfwGetKey(gWindow, 'A')){
        blockX -= stepSize;
    } else if(glfwGetKey(gWindow, 'D')){
        blockX += stepSize;
    }
    
    // Collision detection
    if(blockX < -1.0f) blockX = -1.0f;
    if(blockX > (1.0f - blockSize * 2)) blockX = 1.0f - blockSize * 2;;
    if(blockY < -1.0f + blockSize * 2)  blockY = -1.0f + blockSize * 2;
    if(blockY > 1.0f) blockY = 1.0f;
    
    // Recalculate vertex positions
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize*2;
    
    vVerts[3] = blockX + blockSize*2;
    vVerts[4] = blockY - blockSize*2;
    
    vVerts[6] = blockX + blockSize*2;
    vVerts[7] = blockY;
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    
    square.asset->bindData(vVerts, 12);
    
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
