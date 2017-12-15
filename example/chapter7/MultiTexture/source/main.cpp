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

#define NUM_SPHERES 50

float gDegreesRotated = 0;
double gScrollY = 0;
GLFWwindow *gWindow;
ModelAsset sphereAsset;
ModelAsset cubeAsset;
ModelInstance sphereIns;
ModelInstance cubeIns;

Texture * mTexture;

Camera gCamera;
Camera sphereCameras[NUM_SPHERES];
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
    
    sphereAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp_reflection.glsl", "fp_reflection.glsl"), 2,GLT_ATTRIBUTE_VERTEX, "vVertex",GLT_ATTRIBUTE_NORMAL, "vNormal",GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");
    sphereAsset.texture = new Texture(szCubeFaces, cube, 6, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, 1);
    gltMakeSphereAsset(sphereAsset, 1.0f, 52, 26);
    sphereIns.asset = &sphereAsset;
    
    mTexture = LoadTGATexture("tarnish.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    
    cubeAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp_skybox.glsl", "fp_skybox.glsl"), 2,GLT_ATTRIBUTE_VERTEX, "vVertex",GLT_ATTRIBUTE_NORMAL, "vNormal");
    gltMakeCubeAsset(cubeAsset, 20.f);
    cubeIns.asset = &cubeAsset;
    
    //init mode
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // init cameras
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
    gCamera.offsetPosition(4.f * -gCamera.forward());
}

//M3DMatrix44f proj;
void RenderScene(void)
{
    M3DVector4f vFloorColor = { 1.0f, 1.0f, 1.0f, 0.75f};
    M3DVector4f vTorusColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    M3DVector4f vSphereColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    const GLfloat degreesPerSecond = 60.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;
    
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    M3DMatrix44f mInverseCamera = glm::inverse(gCamera.orientation());
    m3dInvertMatrix44(mInverseCamera, gCamera.orientation());
    
    sphereAsset.shaders->use();
    sphereAsset.shaders->setUniform("mvpMatrix", gCamera.matrix() * sphereIns.transform);
    sphereAsset.shaders->setUniform("mvMatrix", sphereIns.transform);
    sphereAsset.shaders->setUniform("normalMatrix", sphereIns.getNormalMatrix());
//    sphereAsset.shaders->setUniform("mInverseCamera", mInverseCamera);
    sphereAsset.shaders->setUniform("cubeMap", 0);
    sphereAsset.shaders->setUniform("tarnishMap", 1);
    glEnable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sphereAsset.texture->object());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture->object());
    sphereIns.draw();
    glDisable(GL_CULL_FACE);
    sphereAsset.shaders->stopUsing();
    
    cubeAsset.shaders->use();
    cubeAsset.shaders->setUniform("mvpMatrix", gCamera.matrix() * cubeIns.transform);
    cubeAsset.shaders->setUniform("cubeMap", 0);
    cubeIns.draw();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    cubeAsset.shaders->stopUsing();
    
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

