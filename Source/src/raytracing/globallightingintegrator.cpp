#include "raytracing/directlightingintegrator.h"
#include "raytracing/globallightingintegrator.h"

GlobalLightingIntegrator::GlobalLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine ):
    Integrator(){

    this->scene = scene;
    this->intersection_engine = intersection_engine;
}

glm::vec3 GlobalLightingIntegrator::TraceRay( Ray r, unsigned int depth ){

    const glm::vec3 black( 0.f );

    Intersection isx( intersection_engine->GetIntersection( r ) );

    //---no intersection with scene---
    if( isx.object_hit == NULL ) return black;

    //---hit light--
    if( isx.object_hit->material->is_light_source )
        return isx.object_hit->material->base_color * isx.texture_color;

    //---global lighting---
    static DirectLightingIntegrator directLightingIntegrator( scene, intersection_engine );
    glm::vec3 A( 0.f ), B( 1.f );
    float throughput( 1.f );

    while( depth < max_depth ){

        glm::vec3 Li( 0.f ), Ld( 0.f );

        //---direct lighting---
        Ld = directLightingIntegrator.TraceRay( r, depth );
        A += B * Ld;

        //---indirect lighting---
        float pdf_bxdf( 0.f );
        glm::vec3 wiW_bxdf( 0.f );
        glm::vec3 bxdf( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -r.direction, wiW_bxdf, pdf_bxdf ) );

        //---Russian Roulette---
        //---meanwhile increament depth---
        if( depth++ > 2 ){

            float tmp( glm::max( bxdf.x, glm::max( bxdf.y, bxdf.z ) ) );
            float rand( distribution( generator ) );

            throughput *= tmp * fabsf( glm::dot( wiW_bxdf, isx.normal ) );

            if( throughput < rand ){
                B *= 0.f;
                break;
            }
        }

        //---pass Russian Roulette test, keep on tracing---
        Ray r_bxdf( isx.point + isx.normal * 1e-4f, wiW_bxdf );
        Intersection isx_bxdf( intersection_engine->GetIntersection( r_bxdf ) );

        if( isx_bxdf.object_hit == NULL
                || isx_bxdf.object_hit->material->is_light_source
                || pdf_bxdf < 1e-3f ){
            B *= 0.f;
            break;
        }else{
            Li = bxdf * fabsf( glm::dot( isx.normal, isx_bxdf.point - isx.point ) );
        }

        B *= Li;
        r = r_bxdf;
        isx = isx_bxdf;
    }

    return A;
}
