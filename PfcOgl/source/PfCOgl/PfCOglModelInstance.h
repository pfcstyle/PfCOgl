//
//  PfCOglModelInstance.h
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef _PfCOglModelInstance_h_
#define _PfCOglModelInstance_h_

#include "PfCOglModelAsset.h"
#include "PfCOglObject.h"

namespace PfCOgl {
    class ModelInstance:PfCObject{
    public:
        ModelAsset* asset;
        glm::mat4 transform;
        
        ModelInstance() :
        asset(NULL),
        transform()
        {}
        
        ~ModelInstance(){}
    };
}

#endif /* _PfCOglModelInstance_h_ */
