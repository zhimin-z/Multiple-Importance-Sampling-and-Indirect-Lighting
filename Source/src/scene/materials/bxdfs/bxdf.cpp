#include <scene/materials/bxdfs/bxdf.h>

glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //TODO
    rand1 *= TWO_PI;
    rand2 *= PI * .5f;

    float x( cosf( rand1 ) * sinf( rand2 ) );
    float y( sinf( rand1 ) * sinf( rand2 ) );
    float z( cosf( rand2 ) );

    wi_ret = glm::vec3( x, y, z );
    pdf_ret = PDF( wo, wi_ret );

    return EvaluateScatteredEnergy( wo, wi_ret );
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    glm::vec3 color( 0.f );

    for( int i = 0; i < num_samples; ++i ){

        float r( sqrtf( samples[ i ].x ) );
        float theta( samples[ i ].y * TWO_PI );

        float x( r * cosf( theta ) );
        float y( r * sinf( theta ) );
        float z( sqrtf( 1.f - samples[ i ].x ) );

        glm::vec3 wi( x, y, z );

        color += EvaluateScatteredEnergy( wo, wi );
    }

    return color / ( float )num_samples;
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    return wo.z * wi.z > 0.f ? fabsf( wi.z ) * INV_PI : 0.f;
}
