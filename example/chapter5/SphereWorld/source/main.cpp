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

#define NUM_SPHERES 50

float gDegreesRotated = 0;
double gScrollY = 0;
GLFWwindow *gWindow;
ModelAsset sphereAsset;
ModelAsset torusAsset;
ModelAsset floorAsset;
ModelInstance floorIns;
ModelInstance torusIns;
ModelInstance sphereIns;

Program mProgram;
Texture *mSphereTex;

Camera gCamera;
Camera sphereCameras[NUM_SPHERES];
// The torus
TriangleBatch     torusBatch;
Batch                floorBatch;
TriangleBatch     sphereBatch;
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //init asset and instances
    
//    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);
    torusAsset.shaders = new Program();
    torusAsset.shaders->initializeStockShaders();
    torusAsset.texture = LoadTGATexture("Marslike.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    gltMakeTorus(torusAsset, 0.4f, 0.15f, 30, 30);
    torusIns.asset = &torusAsset;
    
    mProgram.initializeStockShaders();
    mSphereTex = LoadTexture("MoonLike.tga", GL_LINEAR, GL_CLAMP_TO_EDGE);
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    
    //floor
    floorAsset.shaders = new Program();
    floorAsset.shaders->initializeStockShaders();
    floorAsset.texture = LoadTGATexture("Marble.tga", GL_LINEAR, GL_REPEAT);
    floorAsset.drawType = GL_TRIANGLE_FAN;
    floorAsset.drawStart = 0;
    floorAsset.drawCount =  4;
    
    int length =  5 * 4;
    GLfloat floorData[length];
    int index = 0;
    GLfloat texSize = 10.0f;
//    for(GLfloat x = -20.0; x <= 20.0f; x+= 0.5) {
    GLfloat x = 20;
        GLfloat temp[5] = {-x, -0.55f, 20.0f, 0.f, 0.f};
        memcpy(floorData + index * 5 * 4,temp,5*sizeof(GLfloat));
        
        GLfloat temp1[5] = {x, -0.55f, 20.0f, texSize, 0.f};
        memcpy(floorData + index * 5 * 4 + 5,temp1,5*sizeof(GLfloat));
        
        GLfloat temp2[5] = {20.0f, -0.55f, -x, texSize, texSize};
        memcpy(floorData + index * 5 * 4 + 10,temp2,5*sizeof(GLfloat));
        
        GLfloat temp3[5] = {-20.0f, -0.55f, -x, 0.f, texSize};
        memcpy(floorData + index * 5 * 4 + 15,temp3,5*sizeof(GLfloat));
//        floorData[index * 4] = {x, -0.55f, 20.0f};
//        floorData[index * 4 + 1] = {x, -0.55f, -20.0f};
//
//        floorData[index * 4 + 2] = {20.0f, -0.55f, x};
//        floorData[index * 4 + 3] = {-20.0f, -0.55f, x};
        index ++;
//    }
    floorAsset.bindData(floorData, length);
    floorIns.asset = &floorAsset;
    floorIns.begin();
    floorIns.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);
    floorIns.CopyTexCoordData2f(2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
    floorIns.end();
    
    //init mode
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // init cameras
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    for(int i = 0; i < NUM_SPHERES; i++) {
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        sphereCameras[i].setPosition({x, 0.0f, z});
    }
}

//M3DMatrix44f proj;
void RenderScene(void)
{
    M3DVector4f vFloorColor = { 1.0f, 1.0f, 1.0f, 1.0f};
    M3DVector4f vTorusColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    M3DVector4f vSphereColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pass this completed matrix to the shader, and render the torus
    // render floor
    proM = gCamera.matrix();
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);//开启混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorAsset.texture->object());
    floorIns.asset->shaders->useStockShader(GLT_SHADER_TEXTURE_MODULATE, &proM, &vFloorColor, 0);
    
    floorIns.draw();
    floorIns.asset->shaders->stopUsing();
    glDisable(GL_BLEND);
    
    M3DVector4f vLightPos = { 0.0f, 10.0f, 5.0f, 1.0f };
    M3DVector4f vLightEyePos;
    m3dTransformVector4(vLightEyePos, vLightPos, gCamera.matrix());
    M3DVector3f vLightEyePos3f = M3DVector3f(vLightEyePos);
    glFrontFace(GL_CW);
    glBindTexture(GL_TEXTURE_2D, mSphereTex->object());
    for(int i = 0; i < NUM_SPHERES; i++) {
        M3DMatrix44f geoTransTemp = M3DMatrix44f(geoTrans);
        geoTransTemp = sphereCameras[i].view() * geoTransTemp;
        mProgram.useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, &geoTransTemp,
                                     &proM, &vLightEyePos3f, &vSphereColor, 0);
        sphereBatch.draw();
        mProgram.stopUsing();
    }

    const GLfloat degreesPerSecond = 60.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;

    glBindTexture(GL_TEXTURE_2D, torusAsset.texture->object());
//    torusIns.transform = rotate(gDegreesRotated,0.0f, 1.0f, 0.0f) * translate(0.0f, 0.0f, -2.5f);
    torusAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, &torusIns.transform,&proM, &vLightEyePos3f, &vTorusColor, 0);
    torusIns.draw();
    torusAsset.shaders->stopUsing();

    glBindTexture(GL_TEXTURE_2D, mSphereTex->object());
//    geoTrans = translate(0.0f, 0.0f, -2.5f) * rotate(gDegreesRotated * -2,0.0f, 1.0f, 0.0f);
    mProgram.useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, &geoTrans,
                                 &proM, &vLightEyePos3f, &vSphereColor, 0);
    sphereBatch.draw();
    mProgram.stopUsing();
    glFrontFace(GL_CCW);
    
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

