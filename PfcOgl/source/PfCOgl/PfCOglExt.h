//
//  PfCOglExt.h
//  PfcOgl
//
//  Created by developer on 25/09/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglExt_h_
#define PfCOglExt_h_
#include <GL/glxew.h>
namespace PfCOgl {
    void getProcAddress(const GLubyte * procName);
    int isExtensionSupported(const char * extname);
}

#endif /* PfCOglExt_h_ */
