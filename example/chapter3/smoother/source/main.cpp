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

using namespace std;
using namespace PfCOgl;

GLFWwindow *gWindow;
list<ModelInstance> mModelInstances;
ModelInstance square;
Program mProgram;
Camera gCamera;
ModelInstance smallStarBatch;
ModelInstance mediumStarBatch;
ModelInstance largeStarBatch;
ModelInstance mountainRangeBatch;
ModelInstance moonBatch;
float secondsElapsed;

#define SMALL_STARS     100
#define MEDIUM_STARS     40
#define LARGE_STARS      15

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE){
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
    
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE){
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    }else if (key == GLFW_KEY_2 && action == GLFW_RELEASE){
        glDisable(GL_BLEND);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POINT_SMOOTH);
    }
    
    const float moveSpeed = 4.0; //units per second
    if(key == GLFW_KEY_S && action == GLFW_PRESS){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    } else if(key == GLFW_KEY_W && action == GLFW_PRESS){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    }
    if(key == GLFW_KEY_A && action == GLFW_PRESS){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    } else if(key == GLFW_KEY_D && action == GLFW_PRESS){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    }
    if(key == GLFW_KEY_Z && action == GLFW_PRESS){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
    } else if(key == GLFW_KEY_X && action == GLFW_PRESS){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
    }
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
    glfwSetKeyCallback(gWindow, key_callback);
    if (!gWindow) {
        throw runtime_error("window init error");
    }
//    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    glfwSetCursorPos(gWindow, 0, 0);
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
    gCamera.setPosition(glm::vec3(0,0,7));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    glClearColor(1, 1, 1, 1); // white
    GLfloat vVerts[SMALL_STARS * 3];       // SMALL_STARS is the largest batch we are going to need
    int i;

    smallStarBatch.asset = new ModelAsset();
    smallStarBatch.asset->drawCount = SMALL_STARS;
    smallStarBatch.asset->drawType = GL_POINTS;
    smallStarBatch.asset->drawStart = 0;
    smallStarBatch.asset->shaders = new Program();
    smallStarBatch.asset->shaders->initializeStockShaders();
    for(i = 0; i < SMALL_STARS; i++)
    {
        vVerts[i*3+0] = (GLfloat)(rand() % (int)SCREEN_SIZE.x);
        vVerts[i*3+1] = (GLfloat)(rand() % (int)(SCREEN_SIZE.y - 100)) + 100.0f;
        vVerts[i*3+2] = 0.0f;
    }
    smallStarBatch.asset->bindData(vVerts, SMALL_STARS * 3);
    smallStarBatch.begin();
    smallStarBatch.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    smallStarBatch.end();
    
    mediumStarBatch.asset = new ModelAsset();
    mediumStarBatch.asset->drawCount = MEDIUM_STARS;
    mediumStarBatch.asset->drawType = GL_POINTS;
    mediumStarBatch.asset->drawStart = 0;
    mediumStarBatch.asset->shaders = new Program();
    mediumStarBatch.asset->shaders->initializeStockShaders();
    for(i = 0; i < MEDIUM_STARS; i++)
    {
        vVerts[i*3+0] = (GLfloat)(rand() % (int)SCREEN_SIZE.x);
        vVerts[i*3+1] = (GLfloat)(rand() % (int)(SCREEN_SIZE.y - 100)) + 100.0f;
        vVerts[i*3+2] = 0.0f;
    }
    mediumStarBatch.asset->bindData(vVerts, 3 * MEDIUM_STARS);
    mediumStarBatch.begin();
    mediumStarBatch.CopyVertexData3f(3, GL_FLOAT, GL_TRUE, 0, NULL);
    mediumStarBatch.end();

    largeStarBatch.asset = new ModelAsset();
    largeStarBatch.asset->drawCount = LARGE_STARS;
    largeStarBatch.asset->drawType = GL_POINTS;
    largeStarBatch.asset->drawStart = 0;
    largeStarBatch.asset->shaders = new Program();
    largeStarBatch.asset->shaders->initializeStockShaders();
    for(i = 0; i < LARGE_STARS; i++)
    {
        vVerts[i*3+0] = (GLfloat)(rand() % (int)SCREEN_SIZE.x);
        vVerts[i*3+1] = (GLfloat)(rand() % (int)(SCREEN_SIZE.y - 100)) + 100.0f;
        vVerts[i*3+2] = 0.0f;
    }
    largeStarBatch.asset->bindData(vVerts, 3 * LARGE_STARS);
    largeStarBatch.begin();
    largeStarBatch.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    largeStarBatch.end();
    
    mountainRangeBatch.asset = new ModelAsset();
    mountainRangeBatch.asset->drawCount = 12;
    mountainRangeBatch.asset->drawType = GL_LINE_STRIP;
    mountainRangeBatch.asset->drawStart = 0;
    mountainRangeBatch.asset->shaders = new Program();
    mountainRangeBatch.asset->shaders->initializeStockShaders();
    GLfloat vMountains[12 * 3] = {
        0.0f, 25.0f, 0.0f,
        50.0f, 100.0f, 0.0f,
        100.0f, 25.0f, 0.0f,
        225.0f, 125.0f, 0.0f,
        300.0f, 50.0f, 0.0f,
        375.0f, 100.0f, 0.0f,
        460.0f, 25.0f, 0.0f,
        525.0f, 100.0f, 0.0f,
        600.0f, 20.0f, 0.0f,
        675.0f, 70.0f, 0.0f,
        750.0f, 25.0f, 0.0f,
        800.0f, 90.0f, 0.0f };
    mountainRangeBatch.asset->bindData(vMountains, 36);
    mountainRangeBatch.begin();
    mountainRangeBatch.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    mountainRangeBatch.end();
    

    // The Moon
    GLfloat x = 700.0f;     // Location and radius of moon
    GLfloat y = 500.0f;
    GLfloat r = 50.0f;
    GLfloat angle = 0.0f;   // Another looping variable
    
    moonBatch.asset = new ModelAsset();
    moonBatch.asset->drawCount = 34;
    moonBatch.asset->drawType = GL_TRIANGLE_FAN;
    moonBatch.asset->drawStart = 0;
    moonBatch.asset->shaders = new Program();
    moonBatch.asset->shaders->initializeStockShaders();
    int nVerts = 0;
    vVerts[nVerts * 3 + 0] = x;
    vVerts[nVerts * 3 + 1] = y;
    vVerts[nVerts * 3 + 2] = 0.0f;
    for(angle = 0; angle < 2.0f * 3.141592f; angle += 0.2f) {
        nVerts++;
        vVerts[nVerts * 3 + 0] = x + float(cos(angle)) * r;
        vVerts[nVerts * 3 + 1] = y + float(sin(angle)) * r;
        vVerts[nVerts * 3 + 2] = 0.0f;
    }
    nVerts++;
    
    vVerts[nVerts * 3 + 0] = x + r;;
    vVerts[nVerts * 3 + 1] = y;
    vVerts[nVerts * 3 + 2] = 0.0f;
    moonBatch.asset->bindData(vVerts, 34);
    moonBatch.begin();
    moonBatch.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    moonBatch.end();
    
    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
}

//M3DMatrix44f proj;
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Everything is white
    GLfloat vWhite [] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    // Draw small stars
    glPointSize(1.0f);
    M3DMatrix44f proj = gltOrthoMatrix(0.0f, SCREEN_SIZE.x, 0.0f, SCREEN_SIZE.y, -1.0f, 1.0f);
    smallStarBatch.asset->shaders->useStockShader(GLT_SHADER_FLAT, &proj, vWhite);
    smallStarBatch.draw();
    smallStarBatch.asset->shaders->stopUsing();
    
    // Draw medium sized stars
    glPointSize(4.0f);
    mediumStarBatch.asset->shaders->useStockShader(GLT_SHADER_FLAT, &proj, vWhite);
    mediumStarBatch.draw();
    mediumStarBatch.asset->shaders->stopUsing();
    
    // Draw largest stars
    glPointSize(8.0f);
    largeStarBatch.asset->shaders->useStockShader(GLT_SHADER_FLAT, &proj, vWhite);
    largeStarBatch.draw();
    largeStarBatch.asset->shaders->stopUsing();
    
    // Draw the "moon"
//    moonBatch.asset->shaders->useStockShader(GLT_SHADER_FLAT, &proj, vWhite);
//    moonBatch.draw();
//    moonBatch.asset->shaders->stopUsing();
    
    // Draw distant horizon
    glLineWidth(3.5);
    mountainRangeBatch.asset->shaders->useStockShader(GLT_SHADER_FLAT, &proj, vWhite);
    mountainRangeBatch.draw();
    mountainRangeBatch.asset->shaders->stopUsing();
    
//    moonBatch.asset->shaders->useStockShader(GLT_SHADER_FLAT, &proj, vWhite);
//    moonBatch.draw();
//    moonBatch.asset->shaders->stopUsing();
    
    // Perform the buffer swap to display back buffer
    glfwSwapBuffers(gWindow);
}

void Update(float secondsElapsed) {
    
    
//    const float mouseSensitivity = 0.1f;
//    double mouseX, mouseY;
//    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
//    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
//    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window
    
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
        secondsElapsed = thisTime - lastTime;
        Update(secondsElapsed);
        lastTime = thisTime;
        
        RenderScene();
    }
    
    glfwTerminate();
}
