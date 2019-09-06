#ifndef BIDIRECTIONALPATHTRACINGHELPER_H
#define BIDIRECTIONALPATHTRACINGHELPER_H

#include <vector>
using std::vector;

#include "scene/scene.h"
#include "scene/geometry/geometry.h"
#include "raytracing/intersection.h"
#include "raytracing/intersectionengine.h"

class BidirectionalPathTracingHelper{
public:
    BidirectionalPathTracingHelper();
    BidirectionalPathTracingHelper( Scene *scene, IntersectionEngine *intersection_engine );

    void generatePath(
            vector< Intersection > &path_vertices,
            vector< glm::vec3 > &path_weights,
            Geometry *pLight, int max_depth );

    Scene *scene;
    IntersectionEngine *intersection_engine;
};

#endif // BIDIRECTIONALPATHTRACINGHELPER_H
