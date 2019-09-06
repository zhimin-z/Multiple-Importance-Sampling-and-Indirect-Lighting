#include <scene/geometry/geometry.h>
#include <scene/geometry/square.h>
#include <scene/geometry/disc.h>

//---isx is on light---
//---ray is from geometry to light---
float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //TODO
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(isx.object_hit == NULL)
    {
        return 0;
    }else{
        //Add more here
        glm::vec3 A( isx.normal );
        glm::vec3 R( -ray.direction );
        float r2( glm::length2( isx.point - ray.origin ) );
        float area( isx.object_hit->area );
        float cos_theta( glm::dot( A, R ) );

        if( dynamic_cast< const SquarePlane * >( isx.object_hit ) == NULL
                && dynamic_cast< const Disc * >( isx.object_hit ) == NULL ){

            cos_theta = fabs( cos_theta );
        }

        return r2 / ( cos_theta * area );
    }
}
