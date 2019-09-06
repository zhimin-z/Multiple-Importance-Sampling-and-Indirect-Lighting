#include <ctime>
#include "raytracing/directlightingintegrator.h"

float PowerHeuristic( int nf, float fPDF, int ng, float gPDF ){

    float f( nf * fPDF );
    float g( ng * gPDF );

    return f * f  / ( f * f + g * g );
}

const int DirectLightingIntegrator::N = 4;

DirectLightingIntegrator::DirectLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine ):
    Integrator(){

    this->scene = scene;
    this->intersection_engine = intersection_engine;

}

glm::vec3 DirectLightingIntegrator::TraceRay( Ray r, unsigned int depth ){

    static const glm::vec3 black( 0.f );

    //---compute intersection with scene---
    Intersection isx( intersection_engine->GetIntersection( r ) );

    //---no intersection---
    if( isx.object_hit == NULL ) return black;

    //---hit light---
    if( isx.object_hit->material->is_light_source )
        return isx.texture_color * isx.object_hit->material->base_color;

    int flag( 3 );

    //---sample a random light---
    glm::vec3 light_color( 0.f );
    float light_PDF( 0.f );

    if( flag & 1 ){
        float rand( distribution( generator ) );
        while( rand >= .99f ) rand = distribution( generator );
        int nLights( scene->lights.size() );
        int iLight( rand * nLights );

        Geometry *pLight( scene->lights[ iLight ] );

        float rand1( distribution( generator ) );
        float rand2( distribution( generator ) );
        float rand3( distribution( generator ) );

        Intersection sample( pLight->SampleLight( rand1, rand2, rand3 ) );
        glm::vec3 woW( -r.direction );
        glm::vec3 wiW( glm::normalize( sample.point - isx.point ) );
        //---ray from p to light---
        Ray ray( isx.point + isx.normal * 1e-4f, wiW );
        light_PDF = pLight->RayPDF( sample, ray );

        if( light_PDF > 1e-3 ){
            light_color = ( float )nLights
                //---BRDF---
                * isx.object_hit->material->EvaluateScatteredEnergy( isx, woW, wiW )
                //---L---
                * pLight->material->EvaluateScatteredEnergy( sample, glm::vec3( 0.f ), -wiW )
                //---shadow---
                * ShadowTest( isx.point + isx.normal * 1e-4f, sample.point, pLight )
                //---cosin term---
                * fabsf( glm::dot( isx.normal, woW ) )
                //---ray PDF---
                / light_PDF
                ;
        }
    }

    if( flag == 1 ) return light_color;

    //---sample a random BxDF---
    glm::vec3 bxdf_color( 0.f );
    float bxdf_PDF( 0.f );

    if( flag & 2 ){
        glm::vec3 wiW( 0.f );
        glm::vec3 SESE( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -r.direction, wiW, bxdf_PDF ) );
        glm::vec3 ray_o( isx.point + isx.normal * 1e-4f );
        glm::vec3 ray_d( wiW );

        Intersection hit( intersection_engine->GetIntersection( Ray( ray_o, ray_d ) ) );

        if( hit.object_hit != NULL && hit.object_hit->material->is_light_source ){

            glm::vec3 woW( glm::normalize( hit.point - isx.point ) );

            bxdf_color = 1.f
                    //---BRDF---
                    * hit.object_hit->material->EvaluateScatteredEnergy( hit, glm::vec3( 0.f ), -woW )
                    //---L---
                    * SESE
                    //---cosin term---
                    * fabsf( glm::dot( isx.normal, woW ) )
                    //---ray PDF---
                    / bxdf_PDF
                    ;
        }
    }

    if( flag == 2 ) return bxdf_color;

    //---combine color using power heuristic---
    float light_PH( PowerHeuristic( 1, light_PDF, 1, bxdf_PDF ) );
    float bxdf_PH( PowerHeuristic( 1, bxdf_PDF, 1, light_PDF ) );

    return light_PH * light_color + bxdf_PH * bxdf_color;
}

glm::vec3 DirectLightingIntegrator::ShadowTest( const glm::vec3 &o, const glm::vec3 &d, const Geometry *pLight ){

    static const glm::vec3 white( 1.f );
    static const glm::vec3 black( 0.f );
    static const glm::vec3 red( 1.f, 0.f, 0.f );
    static const glm::vec3 green( 0.f, 1.f, 0.f );

    Ray ray( o, d - o );
    QList< Intersection > isxes( intersection_engine->GetAllIntersections( ray ) );

    for( const Intersection &isx : isxes ){
        //---null intersection---
        if( isx.object_hit == NULL ) return red;
        //---non-light intersection---
        if( isx.object_hit->material->is_light_source == false ) return black;
        //---unwanted light intersection---
        if( isx.object_hit != pLight ) continue;
        //---wanted light intersection---
        else return white;
    }

    //---impossible---
    return green;
}
