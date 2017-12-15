//
//  main.cpp
//  PfcOgl
//  多重纹理
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

#define NUM_STARS 1000

float gDegreesRotated = 0;
double gScrollY = 0;
GLFWwindow *gWindow;
ModelAsset starsAsset;
ModelInstance starsIns;

Texture * mTexture;

Camera gCamera;
float secondsElapsed;

//用于全局的model变换   一般update中使用
M3DMatrix44f geoTrans;
M3DMatrix44f proM;
const M3DVector2f SCREEN_SIZE(800, 600);

const char *szCubeFaces[6] = { "pos_x.tga", "neg_x.tga", "pos_y.tga", "neg_y.tga", "pos_z.tga", "neg_z.tga" };

GLenum  cube[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

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
        //设置点的原点在左下  默认是左上
        glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    }else if (key == GLFW_KEY_2 && action == GLFW_RELEASE){
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

PfCOgl::Texture* LoadTGATexture(const char* filename, GLint minFilter, GLint magFilter, GLint wrap_mode, GLint texType) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PfCOgl::Bitmap bmp = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new PfCOgl::Texture(bmp, minFilter, magFilter, wrap_mode, texType);
}

PfCOgl::Texture* LoadTexture(const char* filename, GLint minFilter, GLint magFilter, GLint wrap_mode,  GLint texType) {
    //    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PfCOgl::Bitmap bmp = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new PfCOgl::Texture(bmp, minFilter, magFilter, wrap_mode, texType);
}

static std::vector<PfCOgl::Shader> LoadShaders(const char* vertFilename, const char* fragFilename) {
    std::vector<PfCOgl::Shader> shaders;
    shaders.push_back(PfCOgl::Shader::shaderFromFile(ResourcePath(vertFilename), GL_VERTEX_SHADER));
    shaders.push_back(PfCOgl::Shader::shaderFromFile(ResourcePath(fragFilename), GL_FRAGMENT_SHADER));
    return shaders;
}

void loadAssetAndInstances() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //init asset and instances
    
    M3DMatrix44f fColors = {{ 1.0f, 1.0f, 1.0f, 1.0f}, // White
        { 0.67f, 0.68f, 0.82f, 1.0f}, // Blue Stars
        { 1.0f, 0.5f, 0.5f, 1.0f}, // Reddish
        { 1.0f, 0.82f, 0.65f, 1.0f}}; // Orange
    
    starsAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp.glsl", "fp.glsl"), 2,GLT_ATTRIBUTE_VERTEX, "vVertex",GLT_ATTRIBUTE_NORMAL, "vNormal",GLT_ATTRIBUTE_COLOR, "vColor");
    starsAsset.texture = LoadTGATexture("star.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    starsAsset.begin(GL_POINTS, NUM_STARS);
    for(int i = 0; i < NUM_STARS; i++)
    {
        int iColor = 0;        // All stars start as white
        
        // One in five will be blue
        if(rand() % 5 == 1)
            iColor = 1;
        
        // One in 50 red
        if(rand() % 50 == 1)
            iColor = 2;
        
        // One in 100 is amber
        if(rand() % 100 == 1)
            iColor = 3;
        
        
        starsAsset.Color4fv(fColors[iColor]);
        
        M3DVector3f vPosition;
        vPosition[0] = float(3000 - (rand() % 6000)) * 0.1f;
        vPosition[1] = float(3000 - (rand() % 6000)) * 0.1f;
        vPosition[2] = -float(rand() % 1000)-1.0f;  // -1 to -1000.0f
        
        starsAsset.Vertex3fv(vPosition);
    }
    starsAsset.endBatchs();
    starsIns.asset = &starsAsset;
    
    // init cameras
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
//    gCamera.offsetPosition(4.f * -gCamera.forward());
}

//M3DMatrix44f proj;
void RenderScene(void)
{
    M3DVector4f vFloorColor = { 1.0f, 1.0f, 1.0f, 0.75f};
    M3DVector4f vTorusColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    M3DVector4f vSphereColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    // Let the vertex program determine the point size
    // 让顶点程序决定点的大小
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    const GLfloat degreesPerSecond = 30.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;
    
    starsAsset.shaders->use();
    starsAsset.shaders->setUniform("mvpMatrix", gCamera.matrix() * starsIns.transform);
    starsAsset.shaders->setUniform("starImage", 0);
    starsAsset.shaders->setUniform("vAngle", gDegreesRotated);
    
    float fTime = glfwGetTime() * 10.0f;
    fTime = fmod(fTime, 500.0f);
    starsAsset.shaders->setUniform("timeStamp", fTime);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, starsAsset.texture->object());
    starsIns.draw();
    glDisable(GL_CULL_FACE);
    starsAsset.shaders->stopUsing();
    
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

