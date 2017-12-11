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
ModelAsset pyramidAsset;
ModelInstance pyramidIns;

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
    return new PfCOgl::Texture(bmp, minMagFilter, GL_LINEAR, wrap_mode);
}

PfCOgl::Texture* LoadTexture(const char* filename, GLint minMagFilter, GLint wrap_mode) {
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PfCOgl::Bitmap bmp = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new PfCOgl::Texture(bmp, minMagFilter, GL_LINEAR, wrap_mode);
}

void loadAssetAndInstances() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    //init asset and instances
    
//    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);
    pyramidAsset.shaders = new Program();
    pyramidAsset.shaders->initializeStockShaders();
    pyramidAsset.texture = LoadTGATexture("stone.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    pyramidAsset.begin(GL_TRIANGLES, 18);
    
    // Bottom of pyramid
    pyramidAsset.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidAsset.Vertex3f(-1.0f, -1.0f, -1.0f);
    
    pyramidAsset.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidAsset.Vertex3f(1.0f, -1.0f, -1.0f);
    
    pyramidAsset.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidAsset.Vertex3f(1.0f, -1.0f, 1.0f);
    
    pyramidAsset.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidAsset.Vertex3f(-1.0f, -1.0f, 1.0f);
    
    pyramidAsset.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidAsset.Vertex3f(-1.0f, -1.0f, -1.0f);
    
    pyramidAsset.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidAsset.Vertex3f(1.0f, -1.0f, 1.0f);
    
    
    M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
    M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
    M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
    M3DVector3f vBackLeft = { -1.0f, -1.0f, -1.0f };
    M3DVector3f vBackRight = { 1.0f, -1.0f, -1.0f };
    M3DVector3f n;
    
    // Front of Pyramid
    m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidAsset.Vertex3fv(vApex);        // Apex
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidAsset.Vertex3fv(vFrontLeft);        // Front left corner
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidAsset.Vertex3fv(vFrontRight);        // Front right corner
    
    
    m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidAsset.Vertex3fv(vApex);        // Apex
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidAsset.Vertex3fv(vBackLeft);        // Back left corner
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidAsset.Vertex3fv(vFrontLeft);        // Front left corner
    
    m3dFindNormal(n, vApex, vFrontRight, vBackRight);
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidAsset.Vertex3fv(vApex);                // Apex
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidAsset.Vertex3fv(vFrontRight);        // Front right corner
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidAsset.Vertex3fv(vBackRight);            // Back right cornder
    
    
    m3dFindNormal(n, vApex, vBackRight, vBackLeft);
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidAsset.Vertex3fv(vApex);        // Apex
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidAsset.Vertex3fv(vBackRight);       // Back right cornder
    
    pyramidAsset.Normal3fv(n);
    pyramidAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidAsset.Vertex3fv(vBackLeft);        // Back left corner
    
    pyramidAsset.endBatchs();
    pyramidIns.asset = &pyramidAsset;
    //init mode
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    
    // init cameras
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    gCamera.offsetPosition(7.f * -gCamera.forward());
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
    proM = gCamera.matrix();
    glBindTexture(GL_TEXTURE_2D, pyramidAsset.texture->object());
    pyramidAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                       &pyramidIns.transform,
                                         &proM,
                                         &vLightPos, &vWhite, 0);
    pyramidIns.draw();
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

