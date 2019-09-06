#ifndef GLOBALLIGHTINGINTEGRATOR_H
#define GLOBALLIGHTINGINTEGRATOR_H

#include "integrator.h"

class GlobalLightingIntegrator : Integrator{
public:
    GlobalLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine );

    glm::vec3 ShadowTest( const glm::vec3 &o, const glm::vec3 &d, const Geometry *pLight );
    glm::vec3 TraceRay( Ray r, unsigned int depth );
};

#endif // GLOBALLIGHTINGINTEGRATOR_H
