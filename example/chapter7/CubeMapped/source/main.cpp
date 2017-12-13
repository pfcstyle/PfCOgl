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
ModelAsset logoAsset;
ModelInstance floorIns;
ModelInstance torusIns;
ModelInstance sphereIns;
ModelInstance logoIns;


Program mProgram;
Texture *mSphereTex;

Camera gCamera;
Camera sphereCameras[NUM_SPHERES];
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
    
    //    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);
    torusAsset.shaders = new Program();
    torusAsset.shaders->initializeStockShaders();
    torusAsset.texture = LoadTGATexture("Marslike.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    gltMakeTorusAsset(torusAsset, 0.4f, 0.15f, 30, 30);
    torusIns.asset = &torusAsset;
    
    sphereAsset.shaders = new Program();
    sphereAsset.shaders->initializeStockShaders();
    sphereAsset.texture = LoadTGATexture("Marslike.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    gltMakeSphereAsset(sphereAsset, 0.1f, 26, 13);
    sphereIns.asset = &sphereAsset;
    
    //floor
    floorAsset.shaders = new Program();
    floorAsset.shaders->initializeStockShaders();
    floorAsset.texture = LoadTGATexture("Marble.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_TEXTURE_2D);
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
    
    logoAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp.glsl", "fp.glsl"), 2,GLT_ATTRIBUTE_VERTEX, "vVertex",GLT_ATTRIBUTE_TEXTURE0, "vTexCoord");
    //top left coord
    int logox = 500;
    int logoy = 155;
    int width = 300;
    int height = 155;
    logoAsset.begin(GL_TRIANGLE_FAN, 4);
    //这里注意  默认是逆时针绘画方向
    //所以 添加的点的顺序是不可以反或者乱的
    // top left corner
    logoAsset.MultiTexCoord2f(0, 0.f, height);
    logoAsset.Vertex3f(logox, logoy, 0.f);
    
    // bottom left corner
    logoAsset.MultiTexCoord2f(0, 0.f, 0.f);
    logoAsset.Vertex3f(logox, logoy - height, 0.f);
    
    // bottom right corner
    logoAsset.MultiTexCoord2f(0, width, 0.f);
    logoAsset.Vertex3f(logox + width, logoy - height, 0.f);
    
    // top right corner
    logoAsset.MultiTexCoord2f(0, width, height);
    logoAsset.Vertex3f(logox + width, logoy, 0.f);
    
    logoAsset.endBatchs();

    logoAsset.texture = LoadTGATexture("OpenGL-Logo.tga", GL_NEAREST, GL_NEAREST,GL_CLAMP_TO_EDGE, GL_TEXTURE_RECTANGLE);
    logoIns.asset = &logoAsset;
    
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

void DrawSongAndDance(GLfloat yRot)        // Called to draw dancing objects
{
    static M3DVector4f vWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
    static M3DVector4f vLightPos = { 0.0f, 3.0f, 0.0f, 1.0f };
    
    // Get the light position in eye space
    M3DVector4f    vLightTransformed;
    m3dTransformVector4(vLightTransformed, vLightPos, gCamera.matrix());
    // Draw the light source
//    modelViewMatrix.Translatev(vLightPos);
    sphereIns.transform = translate(0.f, 3.f, 0.f);
    proM = gCamera.matrix() * geoTrans * sphereIns.transform;
    sphereAsset.shaders->useStockShader(GLT_SHADER_FLAT,
                                &proM,
                                 &vWhite);
    sphereIns.draw();
    proM = gCamera.matrix();
    sphereIns.transform = M3DMatrix44f();
    glBindTexture(GL_TEXTURE_2D, sphereAsset.texture->object());
    for(int i = 0; i < NUM_SPHERES; i++) {
        M3DMatrix44f geoTransTemp = M3DMatrix44f(geoTrans * sphereIns.transform);
        geoTransTemp = sphereCameras[i].view() * geoTransTemp;
        sphereAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, &geoTransTemp,
                                            &proM, &vLightTransformed, &vWhite, 0);
        sphereIns.draw();
        sphereAsset.shaders->stopUsing();
    }
    
    // Song and dance
    torusIns.transform = geoTrans * translate(0.0f, 0.4f, -2.5f) * rotate(yRot,0.0f, 1.0f, 0.0f);
    
    // Draw stuff relative to the camera
    glBindTexture(GL_TEXTURE_2D, torusAsset.texture->object());
    torusAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                 &torusIns.transform,
                                 &proM,
                                 &vLightTransformed,
                                 &vWhite,
                                 0);
    torusIns.draw();
    
    sphereIns.transform = geoTrans * translate(0.0f, 0.4f, -2.5f) * rotate(yRot * -2,0.0f, 1.0f, 0.0f) * translate(0.8f, 0.0f, 0.0f);
    
    glBindTexture(GL_TEXTURE_2D, sphereAsset.texture->object());
    sphereAsset.shaders->useStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                 &sphereIns.transform,
                                 &proM,
                                 &vLightTransformed,
                                 &vWhite,
                                 0);
    sphereIns.draw();
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
    
    //全局model变换
    geoTrans = scale(1.f, -1.f, 1.f) * translate(0.f, 0.8f, 0.f);
    glFrontFace(GL_CW);
    DrawSongAndDance(gDegreesRotated);
    glFrontFace(GL_CCW);
    
    // Pass this completed matrix to the shader, and render the torus
    // render floor
    proM = gCamera.matrix();
    glEnable(GL_BLEND);//开启混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorAsset.texture->object());
    floorIns.asset->shaders->useStockShader(GLT_SHADER_TEXTURE_MODULATE, &proM, &vFloorColor, 0);
    floorIns.draw();
    floorIns.asset->shaders->stopUsing();
    glDisable(GL_BLEND);
    
    geoTrans = M3DMatrix44f();
    DrawSongAndDance(gDegreesRotated);
    
    M3DMatrix44f mScreenSpace;
    m3dMakeOrthographicMatrix(mScreenSpace, 0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    
    // Turn blending on, and depth testing off
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    
    logoAsset.shaders->use();
    logoAsset.shaders->setUniform("rectangleImage", 0);
    logoAsset.shaders->setUniform("mvpMatrix", mScreenSpace);
    glBindTexture(GL_TEXTURE_RECTANGLE, logoAsset.texture->object());
    logoIns.draw();
    
    // Restore no blending and depth test
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
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

