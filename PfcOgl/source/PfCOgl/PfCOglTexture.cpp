//
//  PfCOglTexture.cpp
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglTexture.h"
#include "platform.hpp"
#include <stdexcept>

using namespace PfCOgl;

static GLenum TextureFormatForBitmapFormat(Bitmap::Format format, bool srgb)
{
    switch (format) {
        case Bitmap::Format_Grayscale: return GL_LUMINANCE;
        case Bitmap::Format_GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
        case Bitmap::Format_RGB: return (srgb ? GL_SRGB : GL_RGB);
        case Bitmap::Format_RGBA: return (srgb ? GL_SRGB_ALPHA : GL_RGBA);
        default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

static GLenum TextureInFormatForBitmapFormat(Bitmap::Format format, bool srgb)
{
    switch (format) {
        case Bitmap::Format_Grayscale: return GL_LUMINANCE;
        case Bitmap::Format_GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
        case Bitmap::Format_RGB: return (srgb ? GL_SRGB : GL_RGB);
        case Bitmap::Format_RGBA: return (srgb ? GL_SRGB_ALPHA : GL_RGBA);
        default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

Texture::Texture(const GLvoid *textureData, GLint minFilter, GLint magFilter, GLint wrapMode, GLint texType, GLsizei width, GLsizei height, GLenum eFormat){
    glGenTextures(1, &_object);
    glBindTexture(texType, _object);
    //缩小过滤器  比如GL_LINEAR 线性
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    //放大过滤器  两个可以不同
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    //设置伸缩方式  比如 repeat  toedge
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrapMode);
    switch (texType) {
        case GL_TEXTURE_1D:
            glTexImage1D(texType,
                         0,
                         GL_RGB,
                         width,
                         0,
                         eFormat,
                         GL_UNSIGNED_BYTE,
                         textureData);
            break;
            case GL_TEXTURE_2D:
            case GL_TEXTURE_RECTANGLE:
                glTexImage2D(texType,
                             0,
                             GL_COMPRESSED_RGB,
                             width,
                             height,
                             0,
                             eFormat,
                             GL_UNSIGNED_BYTE,
                             textureData);
                if (texType == GL_TEXTURE_2D) {
                    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
                       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
                       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
                       minFilter == GL_NEAREST_MIPMAP_NEAREST){
                        //生成Mip层
                        glGenerateMipmap(GL_TEXTURE_2D);
                    }
                }
            break;
            
        default:
            break;
    }
    
    
    glBindTexture(texType, 0);
}

Texture::Texture(const Bitmap& bitmap, GLint minFilter, GLint magFilter, GLint wrapMode, GLint texType) :
_originalWidth((GLfloat)bitmap.width()),
_originalHeight((GLfloat)bitmap.height())
{
    glGenTextures(1, &_object);
    glBindTexture(texType, _object);
    //缩小过滤器  比如GL_LINEAR 线性
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
    //放大过滤器  两个可以不同
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, magFilter);
    //设置伸缩方式  比如 repeat  toedge
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrapMode);
    //更改像素存储方式  默认4字节对齐  下面是从数据缓冲区中如何解包
    // 如tga格式的图片  其排列方式为1字节
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //希望opengl使用哪种算法来选择压缩格式
    //    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);
    //判断是否压缩成功  还可以获取压缩格式  压缩后大小等信息
//    GLint compFlag;
//    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compFlag);
//    glGetCompressedTexImage2D 用于获取已经压缩好的图片并保存在本地
//    glCompressedTexImage2D()类似glTexImage2D  可以加载本地的已经压缩的图片
    //相当于预压缩  可以提高效率
    glTexImage2D(texType,
                 0,
                 TextureInFormatForBitmapFormat(bitmap.format(), false),
                 (GLsizei)bitmap.width(),
                 (GLsizei)bitmap.height(),
                 0,
                 TextureFormatForBitmapFormat(bitmap.format(), false),
                 GL_UNSIGNED_BYTE,
                 bitmap.pixelBuffer());
    
    glBindTexture(texType, 0);
    if (texType == GL_TEXTURE_2D) {
        if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
           minFilter == GL_LINEAR_MIPMAP_NEAREST ||
           minFilter == GL_NEAREST_MIPMAP_LINEAR ||
           minFilter == GL_NEAREST_MIPMAP_NEAREST){
            //生成Mip层
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    
}

Texture::Texture(const char **cubeFaces, const GLenum *cube, int length, GLint minFilter, GLint magFilter, GLint wrapMode, GLint unpackAlignment)
{
    glGenTextures(1, &_object);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _object);
    //缩小过滤器  比如GL_LINEAR 线性
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
    //放大过滤器  两个可以不同
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
    //设置伸缩方式  比如 repeat  toedge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapMode);
    //更改像素存储方式  默认4字节对齐  下面是从数据缓冲区中如何解包
    // 如tga格式的图片  其排列方式为1字节
    //    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //希望opengl使用哪种算法来选择压缩格式
    //    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);
    //判断是否压缩成功  还可以获取压缩格式  压缩后大小等信息
    //    GLint compFlag;
    //    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compFlag);
    //    glGetCompressedTexImage2D 用于获取已经压缩好的图片并保存在本地
    //    glCompressedTexImage2D()类似glTexImage2D  可以加载本地的已经压缩的图片
    //相当于预压缩  可以提高效率
    int i = 0;
    for (i = 0; i<length; i++) {
        if (unpackAlignment > 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
        }
        
        PfCOgl::Bitmap bitmap = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(cubeFaces[i]));
        bitmap.flipVertically();
        glTexImage2D(cube[i],
                     0,
                     TextureInFormatForBitmapFormat(bitmap.format(), false),
                     (GLsizei)bitmap.width(),
                     (GLsizei)bitmap.height(),
                     0,
                     TextureFormatForBitmapFormat(bitmap.format(), false),
                     GL_UNSIGNED_BYTE,
                     bitmap.pixelBuffer());
        
    }
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST){
        //生成Mip层
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    
}

Texture *Texture::generateTextureArray(char ** cFile, int length, GLsizei width, GLsizei height, GLint minFilter, GLint magFilter, GLint wrapMode, GLint unpackAlignment){
    GLuint obj;
    glGenTextures(1, &obj);
    glBindTexture(GL_TEXTURE_2D_ARRAY, obj);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, magFilter);
    
    if (unpackAlignment > 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
    }
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, length + 1, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    for(int i = 0; i < length; i++) {
        PfCOgl::Bitmap bitmap = PfCOgl::Bitmap::bitmapFromFile(ResourcePath(cFile[i]));
        bitmap.flipVertically();
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                        0,
                        0,
                        0,
                        i,
                        (GLsizei)bitmap.width(),
                        (GLsizei)bitmap.height(),
                        1,
                        TextureFormatForBitmapFormat(bitmap.format(), false),
                        GL_UNSIGNED_BYTE,
                        bitmap.pixelBuffer());
    }
    return new Texture(obj);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_object);
}

GLuint Texture::object() const
{
    return _object;
}

GLfloat Texture::originalWidth() const
{
    return _originalWidth;
}

GLfloat Texture::originalHeight() const
{
    return _originalHeight;
}
