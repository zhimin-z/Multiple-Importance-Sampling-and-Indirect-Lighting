#include <raytracing/integrator.h>
#include <raytracing/directlightingintegrator.h>
#include <raytracing/globallightingintegrator.h>

Integrator::Integrator():
    max_depth(5){

    scene = NULL;
    intersection_engine = NULL;
}

Integrator::Integrator( Scene *s ):
    Integrator(){

    scene = s;
}

glm::vec3 ComponentMult( const glm::vec3 &a, const glm::vec3 &b ){
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

void Integrator::SetDepth( unsigned int depth ){
    max_depth = depth;
}

glm::vec3 Integrator::TraceRay( Ray r, unsigned int depth ){

//#define GLOBAL_ILLUMINATION

#ifndef GLOBAL_ILLUMINATION
    //---direct lighting---
    static DirectLightingIntegrator directLightingIntegrator( scene, intersection_engine );
    return directLightingIntegrator.TraceRay( r, depth );
#else
    //---global lighting---
    static GlobalLightingIntegrator globalLightingIntegrator( scene, intersection_engine );
    return globalLightingIntegrator.TraceRay( r, depth );
#endif
}
