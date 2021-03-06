//
//  PfCOglTexture.h
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglTexture_h_
#define PfCOglTexture_h_

#include <GL/glew.h>
#include "PfCOglBitmap.h"
#include "PfCOglObject.h"

namespace PfCOgl {
    
    /**
     Represents an OpenGL texture
     */
    class Texture:PfCObject {
    public:
        /**
         Creates a texture from a bitmap.
         
         The texture will be loaded upside down because tdogl::Bitmap pixel data
         is ordered from the top row down, but OpenGL expects the data to
         be from the bottom row up.
         
         @param bitmap  The bitmap to load the texture from
         @param minFilter  GL_NEAREST or GL_LINEAR
         @param magFilter  GL_NEAREST or GL_LINEAR
         @param wrapMode GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, or GL_CLAMP_TO_BORDER
         @param texType  GL_TEXTURE_1D  GL_TEXTURE_2D
         */
        Texture(const Bitmap& bitmap,
                GLint minFilter = GL_LINEAR,
                GLint magFilter = GL_LINEAR,
                GLint wrapMode = GL_CLAMP_TO_EDGE,
                GLint texType = GL_TEXTURE_2D);
        
        Texture(const GLvoid *textureData, GLint minFilter, GLint magFilter, GLint wrapMode, GLint texType, GLsizei width, GLsizei height, GLenum eFormat);
        Texture(const char **cubeFaces, const GLenum *cube, int length, GLint minFilter, GLint magFilter, GLint wrapMode, GLint unpackAlignment);
        
        Texture(const GLuint obj):_object(obj){};
        
        static Texture *generateTextureArray(char ** cFile, int length, GLsizei width, GLsizei height, GLint minFilter, GLint magFilter, GLint wrapMode, GLint unpackAlignment);
        
        /**
         Deletes the texture object with glDeleteTextures
         */
        ~Texture();
        
        /**
         @result The texure object, as created by glGenTextures
         */
        GLuint object() const;
        
        /**
         @result The original width (in pixels) of the bitmap this texture was made from
         */
        GLfloat originalWidth() const;
        
        /**
         @result The original height (in pixels) of the bitmap this texture was made from
         */
        GLfloat originalHeight() const;
        
    private:
        GLuint _object;
        GLfloat _originalWidth;
        GLfloat _originalHeight;
        
        //copying disabled
        Texture(const Texture&);
        const Texture& operator=(const Texture&);
    };
    
}


#endif /* PfCOglTexture_h_ */
