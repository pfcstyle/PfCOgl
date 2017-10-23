//
//  PfCOglLight.h
//  PfcOgl
//
//  Created by developer on 27/07/2017.
//  Copyright © 2017 developer. All rights reserved.
//

#ifndef PfCOglLight_h_
#define PfCOglLight_h_

#include "PfCOglObject.h"
#include <glm/glm.hpp>

namespace PfCOgl {
    class Light:PfCObject{
    public:
        Light();
        ~Light();
        glm::vec4 position;
        glm::vec3 intensities; //a.k.a. the color of the light
        float attenuation;
        float ambientCoefficient;
        float coneAngle;
        glm::vec3 coneDirection;
    };
}

#endif /* PfCOglLight_h_ */
