#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>

glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    float theta_wi( fabsf( wi.z ) );
    float theta_wo( fabsf( wo.z ) );

    if( theta_wi == 0.f || theta_wo == 0.f ) return glm::vec3( 0.f );

    glm::vec3 wh( glm::normalize( wo + wi ) );

    return reflection_color
            * F( wi, wh )
            * D( wh )
            * G( wo, wi, wh )
            * .25f / theta_wi / theta_wo;
}

glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const{

    float cos_theta( powf( rand1, 1.f / ( exponent + 1.f ) ) );
    float sin_theta( sqrtf( fmaxf( 0.f, 1.f - cos_theta * cos_theta ) ) );

    float phi( rand2 * TWO_PI );
    float cos_phi( cosf( phi ) );
    float sin_phi( sinf( phi ) );

    glm::vec3 wh( sin_theta * cos_phi, sin_theta * sin_phi, cos_theta );

    float woDwh( glm::dot( wo, wh ) );

    if( woDwh < 0.f ) wh = -wh;

    wi_ret = -wo + 2.f * woDwh * wh;
    pdf_ret = PDF( wo, wi_ret );

    return EvaluateScatteredEnergy( wo, wi_ret );
}

glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    glm::vec3 color( 0.f );

    for( int i = 0; i < num_samples; ++i ){

        float cos_theta( powf( samples[ i ].x, 1.f / ( exponent + 1.f ) ) );
        float sin_theta( sqrtf( fmaxf( 0.f, 1.f - cos_theta * cos_theta ) ) );

        float phi( samples[ i ].y * TWO_PI );
        float cos_phi( cosf( phi ) );
        float sin_phi( sinf( phi ) );

        glm::vec3 wh( sin_theta * cos_phi, sin_theta * sin_phi, cos_theta );

        float woDwh( glm::dot( wo, wh ) );

        if( woDwh < 0.f ) wh = -wh;

        glm::vec3 wi( -wo + 2.f * woDwh * wh );

        color += EvaluateScatteredEnergy( wo, wi );
    }

    return color / ( float )num_samples;
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const{

    glm::vec3 wh( glm::normalize( wo + wi ) );
    float cos_theta( fabsf( wh.z ) );
    float woDwh( glm::dot( wo, wh ) );

    if( woDwh <= 0.f ) return 0.f;

    float PDF( ( ( exponent + 1.f ) * powf( cos_theta, exponent ) )
               / ( TWO_PI * 4.f * woDwh ) );

    return PDF;
}

glm::vec3 BlinnMicrofacetBxDF::F( const glm::vec3 &wi, const glm::vec3 &wh ) const{
    return glm::vec3( 1.f );
}

float BlinnMicrofacetBxDF::G( const glm::vec3 &wo, const glm::vec3 &wi, const glm::vec3 &wh ) const{

    float nDwh( fabsf( wh.z ) );
    float nDwo( fabsf( wo.z ) );
    float nDwi( fabsf( wi.z ) );
    float woDwh( fabsf( glm::dot( wo, wh ) ) );

    return fminf( 1.f, fminf( 2.f * nDwh * nDwo / woDwh,
                              2.f * nDwh * nDwi / woDwh ) );
}

float BlinnMicrofacetBxDF::D( const glm::vec3 &wh ) const{

    float cos_theta_wh( fabsf( wh.z ) );
    return ( exponent + 2.f ) * INV_PI * powf( cos_theta_wh, exponent );
}
