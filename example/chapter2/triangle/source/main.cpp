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

using namespace std;
using namespace PfCOgl;

ModelAsset *mModelAsset;
GLFWwindow *gWindow;
GLuint mVAO;
list<ModelInstance> mModelInstances;
Batch mTriangleBatch;
Program mProgram;

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
    gWindow = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
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

void createVBOAndVAO(){
    //生成buffer name(unique id)   param1:生成的数量
    GLuint buffer[2];
    glGenBuffers(2, &buffer[0]);
    //绑定name  param1：绑定的buffer类型
    /**
     GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, or GL_UNIFORM_BUFFER.
     直白来说，影响的是绘图时所使用的绘图函数，具体参考文档
     比如本行代码，需要使用drawArray

     */
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    //创建定点值  范围是-1~1
    GLfloat vertex_data[] = {
        0.0f,0.5f,0.0f,
        -0.5f,-0.5f,0.0f,
        0.5f,-0.5f,0.0f,
        0.8f,-0.6f,0.0f,
        0.4f,-0.6f,0.0f,
        0.0f,-0.7f,0.0f,
    };
    GLfloat colors[] = {
        0.0f,0.0f,0.0f,1.0
    };
    
    //填充数据了  填充到之前绑定的VBO中
    //param1:跟之前绑定的buffer要一致
    //param2:绑定数据的大小
    //param3:要绑定的数据
    //param4:绑定的数据如何使用
    /**
     GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
     STREAM:修改一次，使用几次
     STATIC:修改一次，使用很多次
     DYNAMIC:修改多次，使用多次
     DRAW:被application修改，并且作为GL绘图和图像的source
     READ:被GL修改，当application查询时可以返回
     COPY:被GL修改，并且作为GL绘图和图像的source
     */
    //vbo填充数据1  NULL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_DYNAMIC_DRAW);
    
    /**
     *vbo填充数据2
     */
//    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
//    memcpy(ptr, vertex_data, sizeof(vertex_data));
//    glUnmapBuffer(GL_ARRAY_BUFFER);
    //vbo填充数据3
//    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_data), vertex_data);
    
    /**vbo填充数据4
     *params解释同glBufferData  version 4.5  上述对应的Named函数都是无法使用的 都是4.5才支持
     *Flags                               Description
     GL_DYNAMIC_STORAGE_BIT              缓冲的内容可以直接更新，如果不设置，OpenGL认为不会更新缓冲区内容，会将数据放在不易访问到的地方，而且无法使用glBufferSubData()之类的命令来更新缓冲的内容
     GL_MAP_READ_BIT                     缓冲的数据仓储可被映射进行读取
     GL_MAP_WRITE_BIT                    缓冲的数据仓储可被映射进行写入
     GL_MAP_PERSISTENT_BIT               缓冲的数据仓储可被持久映射，支持持久映射(persistent map)会对性能产生一些花销，所以除非我们真的需要，不然最好不要设置这个标志
     GL_MAP_COHERENT_BIT                 缓冲的映射是无缝的
     GL_CLIENT_STORAGE_BIT               如果其他所有的条件都能满足，就将存储放在本地客户端(CPU)
 
     *
     */
//    glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_MAP_READ_BIT);
//    glNamedBufferStorage(mModelAsset->vbo, sizeof(vertex_data), vertex_data, GL_MAP_READ_BIT);
    
    glGenVertexArrays(1,&mModelAsset->vao);
    glBindVertexArray(mModelAsset->vao);
    
    /**
     void glClearBufferSubData(GLenum target,
     GLenum internalformat,
     GLintptr offset,
     GLsizeiptr size,
     GLenum format,
     GLenum type,
     const void* data);
     函数接收一个指针，指针指向的变量包含有我们想要用来清除缓冲对象的值，然后依照internalformat指定的格式进行转换，转换后的数据复制到offset和size(以字节为单位)指定的缓冲数据仓储区域内。format和type告诉OpenGLdata指向的数据的信息。format可以是GL_RED、GL_RG、GL_RGB、GL_RGBA其中之一，它们分别用来指定单通道、双通道、三通道、四通道数据。同时，type表示数据的类型。比如它可以是GL_UNSIGNED_BYTE或者GL_FLOAT，分别用以指定无符号字节或者浮点数据
     *
     */
    
//    void glCopyBufferSubData(GLenum readtarget,
//                             GLenum writetarget,
//                             GLintptr readoffset,
//                             GLintptr writeoffset,
//                             GLsizeiptr size);
    
    //启用GL中的某个属性，传入属性对应的index
    glEnableVertexAttribArray(mModelAsset->shaders->attrib("vert"));
    //4.5
//    glEnableVertexArrayAttrib(mModelAsset->vao,mModelAsset->shaders->attrib("vert"));
    
    //对属性赋值
    /**
     param1:属性索引
     param2:每次读取几个数据  或者可以理解成传入的是几维向量
     param3:数据类型
     param4:是否单位化
     param5:读取的步长，默认0
     param6:从哪开始读，默认0
     */
    glVertexAttribPointer(mModelAsset->shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    //4.5 绑定vao
//    glVertexArrayAttribBinding(mModelAsset->vao, mModelAsset->shaders->attrib("vert"), 0);
    // First, bind a vertex buffer to the VAO
//    glVertexArrayVertexBuffer(vao,                  // Vertex array object
//                              0,                    // First vertex buffer binding
//                              buffer,               // Buffer object
//                              0,                    // Start from the beginning
//                              sizeof(vmath::vec4)); // Each vertex is one vec4
    
    // Now, describe the data to OpenGL, tell it where it is, and turn on automatic
    // vertex fetching for the specified attribute
//    glVertexArrayAttribFormat(vao,                  // Vertex array object
//                              0,                    // First attribute
//                              4,                    // Four components
//                              GL_FLOAT,             // Floating-point data
//                              GL_FALSE,             // Normalized - ignored
//                              0);                   // First element of the vertex


//    解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
//    设置细分曲面控制点数目 默认是3
//    glPatchParameteri(GL_PATCH_VERTICES, 3);
////    设置绘画模式  使用线  无颜色
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
////    设置点大小
//    glPointSize(5.0);
}

void SetupRC(){
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );
    
    mProgram.initializeStockShaders();
    GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f };
    
    mTriangleBatch.begin(GL_TRIANGLES,3);
    mTriangleBatch.CopyVertexData3f(vVerts);
    mTriangleBatch.end();
}

void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    mProgram.useStockShader(GLT_SHADER_IDENTITY, vRed);
    mTriangleBatch.draw();
    
    // Perform the buffer swap to display back buffer
    glfwSwapBuffers(gWindow);
}

void Update(float secondsElapsed) {
    //rotate the first instance in `gInstances`
//    const float moveSpeed = 4.0; //units per second
    if(glfwGetKey(gWindow, 'S')){
        
    } else if(glfwGetKey(gWindow, 'W')){
        
    }
    if(glfwGetKey(gWindow, 'A')){
        
    } else if(glfwGetKey(gWindow, 'D')){
        
    }
    if(glfwGetKey(gWindow, 'Z')){
        
    } else if(glfwGetKey(gWindow, 'X')){
        
    }
    
}

void AppMain(){
    //创建窗口
    createWindow();
    //init glew
    initGlew();
    // init shaders and program
    SetupRC();
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
