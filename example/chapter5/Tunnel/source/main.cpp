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
#include "PfCOglTriangleBatch.h"
#include "PfCOglCamera.h"
#include "PfCOglTool.h"
#include "math3d.h"

using namespace std;
using namespace PfCOgl;

float gDegreesRotated = 0;
double gScrollY = 0;
GLFWwindow *gWindow;
ModelAsset             floorAsset;
ModelAsset             ceilingAsset;
ModelAsset             leftWallAsset;
ModelAsset             rightWallAsset;

ModelInstance floorIns;
ModelInstance ceilingIns;
ModelInstance leftWallIns;
ModelInstance rightWallIns;

Program mProgram;
Texture *mSphereTex;

Camera gCamera;
float secondsElapsed;

//用于全局的model变换   一般update中使用
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE){
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
    
    //缩小的过滤方式
    //format: GL_FILTER_MIPMAP_SELECTOR
    //filter表示要使用过滤方式   selector表示要如何选择使用mip层
    //线性效果最好  但是性能最低
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE){
        //最邻近过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }else if (key == GLFW_KEY_2 && action == GLFW_RELEASE){
        //线性过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }else if (key == GLFW_KEY_3 && action == GLFW_RELEASE){
        //最邻近过滤 最邻近选择
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }else if (key == GLFW_KEY_4 && action == GLFW_RELEASE){
        //最邻近过滤  线性选择
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }else if (key == GLFW_KEY_5 && action == GLFW_RELEASE){
        //线性过滤  最邻近选择   游戏加速
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }else if (key == GLFW_KEY_6 && action == GLFW_RELEASE){
        //三线性  精度最高   性能最差
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    
//    const float moveSpeed = 4.0; //units per second
//    if(key == GLFW_KEY_S ){
//        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
//    } else if(key == GLFW_KEY_W ){
//        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
//    }
//    if(key == GLFW_KEY_A ){
//        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
//    } else if(key == GLFW_KEY_D ){
//        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
//    }
//    if(key == GLFW_KEY_Z ){
//        gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
//    } else if(key == GLFW_KEY_X ){
//        gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
//    }
    glfwSwapBuffers(gWindow);
}

// records how far the y axis has been scrolled
void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
    gScrollY += deltaY;
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
    glfwSetKeyCallback(gWindow, key_callback);
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
    glfwSetScrollCallback(gWindow, OnScroll);
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

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x,y,z));
}


// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x,y,z));
}

// convenience function that returns a scaling matrix
glm::mat4 rotate(float rotDegrees, GLfloat x, GLfloat y, GLfloat z) {
    return glm::rotate(glm::mat4(),glm::radians(rotDegrees), glm::vec3(x,y,z));
}

PfCOgl::Texture* LoadTGATexture(const char* filename, GLint minMagFilter, GLint wrap_mode) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PfCOgl::Bitmap bmp = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new PfCOgl::Texture(bmp, minMagFilter, wrap_mode);
}

PfCOgl::Texture* LoadTexture(const char* filename, GLint minMagFilter, GLint wrap_mode) {
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PfCOgl::Bitmap bmp = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new PfCOgl::Texture(bmp, minMagFilter, wrap_mode);
}

void loadAssetAndInstances() {
    glClearColor(0.0f, 0.0f, 0.0f,1.0f);
    //init asset and instances
    
//    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);
    
    GLfloat z;
    //floor
    floorAsset.shaders = new Program();
    floorAsset.shaders->initializeStockShaders();
    floorAsset.texture = LoadTGATexture("floor.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    floorAsset.begin(GL_TRIANGLE_STRIP, 28);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        floorAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
        floorAsset.Vertex3f(-10.0f, -10.0f, z);
        
        floorAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
        floorAsset.Vertex3f(10.0f, -10.0f, z);
        
        floorAsset.MultiTexCoord2f(0, 0.0f, 1.0f);
        floorAsset.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        floorAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
        floorAsset.Vertex3f(10.0f, -10.0f, z - 10.0f);
    }
    floorAsset.endBatchs();
    floorIns.asset = &floorAsset;
    
    //ceil
    ceilingAsset.shaders = new Program();
    ceilingAsset.shaders->initializeStockShaders();
    ceilingAsset.texture = LoadTGATexture("ceiling.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    ceilingAsset.begin(GL_TRIANGLE_STRIP, 28);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        ceilingAsset.MultiTexCoord2f(0, 0.0f, 1.0f);
        ceilingAsset.Vertex3f(-10.0f, 10.0f, z - 10.0f);
        
        ceilingAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
        ceilingAsset.Vertex3f(10.0f, 10.0f, z - 10.0f);
        
        ceilingAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
        ceilingAsset.Vertex3f(-10.0f, 10.0f, z);
        
        ceilingAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
        ceilingAsset.Vertex3f(10.0f, 10.0f, z);
    }
    ceilingAsset.endBatchs();
    ceilingIns.asset = &ceilingAsset;
    
    //leftWall
    leftWallAsset.shaders = new Program();
    leftWallAsset.shaders->initializeStockShaders();
    leftWallAsset.texture = LoadTGATexture("brick.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    leftWallAsset.begin(GL_TRIANGLE_STRIP, 28);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        leftWallAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
        leftWallAsset.Vertex3f(-10.0f, -10.0f, z);
        
        leftWallAsset.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftWallAsset.Vertex3f(-10.0f, 10.0f, z);
        
        leftWallAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
        leftWallAsset.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        leftWallAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
        leftWallAsset.Vertex3f(-10.0f, 10.0f, z - 10.0f);
    }
    leftWallAsset.endBatchs();
    leftWallIns.asset = &leftWallAsset;
    
    //rightWall
    rightWallAsset.shaders = new Program();
    rightWallAsset.shaders->initializeStockShaders();
    rightWallAsset.texture = LoadTGATexture("brick.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    rightWallAsset.begin(GL_TRIANGLE_STRIP, 28);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        rightWallAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
        rightWallAsset.Vertex3f(10.0f, -10.0f, z);
        
        rightWallAsset.MultiTexCoord2f(0, 0.0f, 1.0f);
        rightWallAsset.Vertex3f(10.0f, 10.0f, z);
        
        rightWallAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
        rightWallAsset.Vertex3f(10.0f, -10.0f, z - 10.0f);
        
        rightWallAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
        rightWallAsset.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    rightWallAsset.endBatchs();
    rightWallIns.asset = &rightWallAsset;
    
    
    //init mode
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    
    // init cameras
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    gCamera.offsetPosition(70.f * -gCamera.forward());
}

//M3DMatrix44f proj;
void RenderScene(void)
{
    M3DVector3f vLightPos = { 1.0f, 1.0f, 0.0f };
    M3DVector4f vWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Pass this completed matrix to the shader, and render the torus
    // render floor
    proM = gCamera.matrix() * geoTrans;
    
    floorAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_REPLACE, &proM, 0);
    glBindTexture(GL_TEXTURE_2D, floorAsset.texture->object());
    floorIns.draw();
    
    glBindTexture(GL_TEXTURE_2D, ceilingAsset.texture->object());
    ceilingAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_REPLACE, &proM, 0);
    ceilingIns.draw();
    
    glBindTexture(GL_TEXTURE_2D, leftWallAsset.texture->object());
    leftWallAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_REPLACE, &proM, 0);
    leftWallIns.draw();
    
    glBindTexture(GL_TEXTURE_2D, rightWallAsset.texture->object());
    rightWallAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_REPLACE, &proM, 0);
    rightWallIns.draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glfwSwapBuffers(gWindow);
}


void Update(float secondsElapsed) {
//    const GLfloat degreesPerSecond = 180.0f;
//    gDegreesRotated += secondsElapsed * degreesPerSecond;
//    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;
//    geoTrans = glm::rotate(glm::mat4(), glm::radians(gDegreesRotated), glm::vec3(0,1,0));
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
    
    //increase or decrease field of view based on mouse wheel
    const float zoomSensitivity = -0.2f;
    float fieldOfView = gCamera.fieldOfView() + zoomSensitivity * (float)gScrollY;
    if(fieldOfView < 5.0f) fieldOfView = 5.0f;
    if(fieldOfView > 130.0f) fieldOfView = 130.0f;
    gCamera.setFieldOfView(fieldOfView);
    gScrollY = 0;
    
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

