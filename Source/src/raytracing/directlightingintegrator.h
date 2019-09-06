#ifndef DIRECTLIGHTINGINTEGRATOR_H
#define DIRECTLIGHTINGINTEGRATOR_H

#include "raytracing/integrator.h"
#include <random>

class DirectLightingIntegrator : Integrator{
public:
    DirectLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine );

    glm::vec3 ShadowTest( const glm::vec3 &o, const glm::vec3 &d, const Geometry *pLight );
    glm::vec3 TraceRay( Ray r, unsigned int depth );

private:
    //---number of sample per light---
    static const int N;
};

#endif // DIRECTLIGHTINGINTEGRATOR_H

/*
#include <chrono>
static std::uniform_real_distribution<float> rng(0, 1);
static std::mt19937 seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
*/
