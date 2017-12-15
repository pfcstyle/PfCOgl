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
#include <string>
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

#define SMALL_STARS     100
#define MEDIUM_STARS     40
#define LARGE_STARS      15

float gDegreesRotated = 0;
double gScrollY = 0;
GLFWwindow *gWindow;
ModelAsset smallStarAsset;
ModelAsset mediumStarAsset;
ModelAsset largeStarAsset;
ModelAsset mountainRangeAsset;
ModelAsset moonAsset;

ModelInstance smallStarIns;
ModelInstance mediumStarIns;
ModelInstance largeStarIns;
ModelInstance mountainRangeIns;
ModelInstance moonIns;

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
    M3DVector3f vVerts[SMALL_STARS];
    
    smallStarAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp.glsl", "fp.glsl"), 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
    smallStarAsset.drawCount = SMALL_STARS;
    smallStarAsset.drawStart = 0;
    smallStarAsset.drawType = GL_POINTS;
    for(int i = 0; i < SMALL_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % (int)SCREEN_SIZE.x);
        vVerts[i][1] = (GLfloat)(rand() % (int)(SCREEN_SIZE.y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    smallStarAsset.bindData(vVerts, SMALL_STARS * 3);
    smallStarAsset.begin();
    smallStarAsset.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    smallStarAsset.end();
    smallStarAsset.texture = LoadTGATexture("star.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    smallStarIns.asset = &smallStarAsset;
    
    mediumStarAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp.glsl", "fp.glsl"), 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
    mediumStarAsset.drawCount = MEDIUM_STARS;
    mediumStarAsset.drawStart = 0;
    mediumStarAsset.drawType = GL_POINTS;
    for(int i = 0; i < MEDIUM_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % (int)SCREEN_SIZE.x);
        vVerts[i][1] = (GLfloat)(rand() % (int)(SCREEN_SIZE.y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    mediumStarAsset.bindData(vVerts, MEDIUM_STARS * 3);
    mediumStarAsset.begin();
    mediumStarAsset.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    mediumStarAsset.end();
    mediumStarAsset.texture = LoadTGATexture("star.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    mediumStarIns.asset = &mediumStarAsset;
    
    largeStarAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp.glsl", "fp.glsl"), 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
    largeStarAsset.drawCount = LARGE_STARS;
    largeStarAsset.drawStart = 0;
    largeStarAsset.drawType = GL_POINTS;
    for(int i = 0; i < LARGE_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % (int)SCREEN_SIZE.x);
        vVerts[i][1] = (GLfloat)(rand() % (int)(SCREEN_SIZE.y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    largeStarAsset.bindData(vVerts, LARGE_STARS * 3);
    largeStarAsset.begin();
    largeStarAsset.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    largeStarAsset.end();
    largeStarAsset.texture = LoadTGATexture("star.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D);
    largeStarIns.asset = & largeStarAsset;
    
    mountainRangeAsset.drawCount = 12;
    mountainRangeAsset.drawType = GL_LINE_STRIP;
    mountainRangeAsset.drawStart = 0;
    mountainRangeAsset.shaders = new Program();
    mountainRangeAsset.shaders->initializeStockShaders();
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
    mountainRangeAsset.bindData(vMountains, 36);
    mountainRangeAsset.begin();
    mountainRangeAsset.CopyVertexData3f(3, GL_FLOAT, GL_FALSE, 0, NULL);
    mountainRangeAsset.end();
    mountainRangeIns.asset = &mountainRangeAsset;
    
    GLfloat x = 700.0f;     // Location and radius of moon
    GLfloat y = 500.0f;
    GLfloat r = 50.0f;
    GLfloat angle = 0.0f;   // Another looping variable
    moonAsset.shaders = Program::getProgramByShadersWithAttr( LoadShaders("vp_moon.glsl", "fp_moon.glsl"), 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");
    char **files = new char*[29];
    for(int i=0; i<29; i++){
        files[i] = new char[32];
        std::sprintf(files[i], "moon%02d.tga", i);
    }
    moonAsset.texture = Texture::generateTextureArray(files, 29, 64, 64, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, 1);
    delete[] files;
    
    moonAsset.begin(GL_TRIANGLE_FAN, 4);
    
    moonAsset.MultiTexCoord2f(0, 0.0f, 0.0f);
    moonAsset.Vertex3f(x - r, y - r, 0.0f);
    
    moonAsset.MultiTexCoord2f(0, 1.0f, 0.0f);
    moonAsset.Vertex3f(x + r, y - r, 0.0f);
    
    moonAsset.MultiTexCoord2f(0, 1.0f, 1.0f);
    moonAsset.Vertex3f(x + r, y + r, 0.0f);
    
    moonAsset.MultiTexCoord2f(0, 0.0f, 1.0f);
    moonAsset.Vertex3f(x - r, y + r, 0.0f);
    moonAsset.endBatchs();
    moonIns.asset = &moonAsset;
    
    //init mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    // init cameras
//    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
//    gCamera.offsetPosition(4.f * -gCamera.forward());
}

//M3DMatrix44f proj;
void RenderScene(void)
{
    M3DVector4f vFloorColor = { 1.0f, 1.0f, 1.0f, 0.75f};
    M3DVector4f vTorusColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    M3DVector4f vSphereColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    M3DVector4f vWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    proM = gltOrthoMatrix(0.0f, SCREEN_SIZE.x, 0.0f, SCREEN_SIZE.y, -1.0f, 1.0f);
    
    glBindTexture(GL_TEXTURE_2D, smallStarAsset.texture->object());
    smallStarAsset.shaders->use();
    smallStarAsset.shaders->setUniform("mvpMatrix", proM * smallStarIns.transform);
    smallStarAsset.shaders->setUniform("starImage", 0);
    glPointSize(4.0f);
    smallStarIns.draw();
    
    glBindTexture(GL_TEXTURE_2D, mediumStarAsset.texture->object());
    mediumStarAsset.shaders->use();
    mediumStarAsset.shaders->setUniform("mvpMatrix", proM * mediumStarIns.transform);
    mediumStarAsset.shaders->setUniform("starImage", 0);
    glPointSize(8.0f);
    mediumStarIns.draw();
    
    glBindTexture(GL_TEXTURE_2D, largeStarAsset.texture->object());
    largeStarAsset.shaders->use();
    largeStarAsset.shaders->setUniform("mvpMatrix", proM * largeStarIns.transform);
    largeStarAsset.shaders->setUniform("starImage", 0);
    glPointSize(12.0f);
    largeStarIns.draw();
    
    mountainRangeAsset.shaders->useStockShader(GLT_SHADER_FLAT, &proM, &vWhite);
    glLineWidth(3.5);
    mountainRangeIns.draw();
    
    // Draw the "moon"
    glBindTexture(GL_TEXTURE_2D_ARRAY, moonAsset.texture->object());
    moonAsset.shaders->use();
    moonAsset.shaders->setUniform("mvpMatrix", proM);
    moonAsset.shaders->setUniform("moonImage", 0);
    
    float fTime = fmod(glfwGetTime(), 28.0f);
    moonAsset.shaders->setUniform("fTime", fTime);
    moonIns.draw();
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

