//
//  PfCOglProgram.h
//  PfcOgl
//
//  Created by developer on 25/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglProgram_h_
#define PfCOglProgram_h_

#include "PfCOglShader.h"
#include "PfCOglObject.h"
#include <vector>
#include <glm/glm.hpp>

#define MAX_SHADER_NAME_LENGTH 64

namespace PfCOgl {
    
    enum GLT_STOCK_SHADER { GLT_SHADER_IDENTITY = 0, GLT_SHADER_FLAT, GLT_SHADER_SHADED, GLT_SHADER_DEFAULT_LIGHT, GLT_SHADER_POINT_LIGHT_DIFF,
        GLT_SHADER_TEXTURE_REPLACE, GLT_SHADER_TEXTURE_MODULATE, GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, GLT_SHADER_TEXTURE_RECT_REPLACE,
        GLT_SHADER_LAST };
    
    enum GLT_SHADER_ATTRIBUTE { GLT_ATTRIBUTE_VERTEX = 0, GLT_ATTRIBUTE_COLOR, GLT_ATTRIBUTE_NORMAL,
        GLT_ATTRIBUTE_TEXTURE0, GLT_ATTRIBUTE_TEXTURE1, GLT_ATTRIBUTE_TEXTURE2, GLT_ATTRIBUTE_TEXTURE3,
        GLT_ATTRIBUTE_LAST};
    
    struct SHADERLOOKUPETRY {
        char szVertexShaderName[MAX_SHADER_NAME_LENGTH];
        char szFragShaderName[MAX_SHADER_NAME_LENGTH];
        GLuint    uiShaderID;
    };
    
    /**
     Represents an OpenGL program made by linking shaders.
     */
    class Program:PfCObject {
    public:
        /**
         Creates a program by linking a list of tdogl::Shader objects
         
         @param shaders  The shaders to link together to make the program
         
         @throws std::exception if an error occurs.
         
         @see tdogl::Shader
         */
        Program(const std::vector<Shader>& shaders);
        /**
         use stock shaders
         */
        Program(void);
        ~Program();
        
        /**
         init stock shaders
         */
        bool initializeStockShaders(void);
        GLint useStockShader(GLT_STOCK_SHADER shaderId);

        GLuint getProgramByShaders(const std::vector<Shader>& shaders);
        GLuint getProgramByShaderSrcWithAttributes(const char *szVertexSrc, const char *szFragmentSrc, ...);
        /**
         @result The program's object ID, as returned from glCreateProgram
         */
        GLuint object() const;

        void use() const;

        bool isInUse() const;

        void stopUsing() const;
        
        /**
         @result The attribute index for the given name, as returned from glGetAttribLocation.
         */
        GLint attrib(const GLchar* attribName) const;
        
        
        /**
         @result The uniform index for the given name, as returned from glGetUniformLocation.
         */
        GLint uniform(const GLchar* uniformName) const;

        /**
         Setters for attribute and uniform variables.

         These are convenience methods for the glVertexAttrib* and glUniform* functions.
         */
#define _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(OGL_TYPE) \
        void setAttrib(const GLchar* attribName, OGL_TYPE v0); \
        void setAttrib(const GLchar* attribName, OGL_TYPE v0, OGL_TYPE v1); \
        void setAttrib(const GLchar* attribName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2); \
        void setAttrib(const GLchar* attribName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3); \
\
        void setAttrib1v(const GLchar* attribName, const OGL_TYPE* v); \
        void setAttrib2v(const GLchar* attribName, const OGL_TYPE* v); \
        void setAttrib3v(const GLchar* attribName, const OGL_TYPE* v); \
        void setAttrib4v(const GLchar* attribName, const OGL_TYPE* v); \
\
        void setUniform(const GLchar* uniformName, OGL_TYPE v0); \
        void setUniform(const GLchar* uniformName, OGL_TYPE v0, OGL_TYPE v1); \
        void setUniform(const GLchar* uniformName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2); \
        void setUniform(const GLchar* uniformName, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3); \
\
        void setUniform1v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \
        void setUniform2v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \
        void setUniform3v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \
        void setUniform4v(const GLchar* uniformName, const OGL_TYPE* v, GLsizei count=1); \

        _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLfloat)
        _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLdouble)
        _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLint)
        _TDOGL_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLuint)

        void setUniformMatrix2(const GLchar* uniformName, const GLfloat* v, GLsizei count=1, GLboolean transpose=GL_FALSE);
        void setUniformMatrix3(const GLchar* uniformName, const GLfloat* v, GLsizei count=1, GLboolean transpose=GL_FALSE);
        void setUniformMatrix4(const GLchar* uniformName, const GLfloat* v, GLsizei count=1, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::mat2& m, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::mat3& m, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::mat4& m, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::vec3& v);
        void setUniform(const GLchar* uniformName, const glm::vec4& v);

        
    private:
        GLuint _object;
        GLuint uiStockShaders[GLT_SHADER_LAST];
        
        //copying disabled
        Program(const Program&);
        const Program& operator=(const Program&);
    };

}

#endif/* PfCOglProgram */
