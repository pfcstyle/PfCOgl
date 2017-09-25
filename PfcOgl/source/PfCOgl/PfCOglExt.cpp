//
//  PfCOglExt.cpp
//  PfcOgl
//
//  Created by developer on 25/09/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#include "PfCOglExt.h"
#include <string.h>
using namespace PfCOgl;
void getProcAddress(const GLubyte *procName)
{
    glXGetProcAddress(procName);
}

int isExtensionSupported(const char * extname)
{
    GLint numExtensions;
    GLint i;
    
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    
    for (i = 0; i < numExtensions; i++)
    {
        const GLubyte * e = glGetStringi(GL_EXTENSIONS, i);
        if (!strcmp((const char *)e, extname))
        {
            return 1;
        }
    }
    
    return 0;
}
