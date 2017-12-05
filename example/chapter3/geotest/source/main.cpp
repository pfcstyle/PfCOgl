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
#include "PfCOglTool.h"
#include "PfCOglTriangleBatch.h"

using namespace std;
using namespace PfCOgl;

GLFWwindow *gWindow;
Camera gCamera;
int nStep = 0;
TriangleBatch     torusBatch;
Program mProgram;
int iCull = 0;
int iDepth = 0;
M3DMatrix44f geoTrans;
M3DMatrix44f proM;
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
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
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

void loadAssetAndInstances() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f );
    
    mProgram.initializeStockShaders();
    gCamera.setPosition(glm::vec3(0,0,7));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
//    gCamera.setNearAndFarPlanes(0.5f, 100.0f);
//    gCamera.offsetPosition(7.0f * gCamera.forward());
    
    
    // Make the torus
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    glPointSize(4.0f);
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Turn culling on if flag is set
    //是否剔除背面(默认是背面)，可以通过glCullFace(mode)修改剔除哪一面
    //mode可以是GL_FRONT、GL_BACK or GL_FRONT_AND_BACK
    if(iCull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    
    // Enable depth testing if flag is set
    if(iDepth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    geoTrans = M3DMatrix44f();
    proM = gCamera.matrix();
    mProgram.useStockShader(GLT_SHADER_DEFAULT_LIGHT, &geoTrans, &proM, vRed);
    
    torusBatch.draw();
    
    // Perform the buffer swap to display back buffer
    glfwSwapBuffers(gWindow);
}

void Update(float secondsElapsed) {
    //rotate the first instance in `gInstances`
//    const float moveSpeed = 4.0; //units per second
    
//    if(iCull)
//        glEnable(GL_CULL_FACE);
//    else
//        glDisable(GL_CULL_FACE);
//
//    // Enable depth testing if flag is set
//    if(iDepth)
//        glEnable(GL_DEPTH_TEST);
//    else
//        glDisable(GL_DEPTH_TEST);
    if (glfwGetKey(gWindow, '1')) {
        iDepth = !iDepth;
    }else if (glfwGetKey(gWindow, '2')){
        iCull = !iCull;
    }else if (glfwGetKey(gWindow, '3')){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }else if (glfwGetKey(gWindow, '4')){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else if (glfwGetKey(gWindow, '5')){
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    
    const float moveSpeed = 4.0; //units per second
    if(glfwGetKey(gWindow, 'S')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    } else if(glfwGetKey(gWindow, 'W')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    }
    if(glfwGetKey(gWindow, 'A')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    } else if(glfwGetKey(gWindow, 'D')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    }
    if(glfwGetKey(gWindow, 'Z')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
    } else if(glfwGetKey(gWindow, 'X')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
    }
    
    const float mouseSensitivity = 0.1f;
    double mouseX, mouseY;
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window
    
    
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
