#include <scene/materials/weightedmaterial.h>
#include <ctime>

WeightedMaterial::WeightedMaterial() :
    Material(){
}

WeightedMaterial::WeightedMaterial(const glm::vec3 &color) : Material(color){}

glm::vec3 WeightedMaterial::EvaluateScatteredEnergy( const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags ) const
{
    //TODO
    glm::mat3 w2t;

    w2t[ 0 ][ 0 ] = isx.tangent[ 0 ];
    w2t[ 1 ][ 0 ] = isx.tangent[ 1 ];
    w2t[ 2 ][ 0 ] = isx.tangent[ 2 ];

    w2t[ 0 ][ 1 ] = isx.bitangent[ 0 ];
    w2t[ 1 ][ 1 ] = isx.bitangent[ 1 ];
    w2t[ 2 ][ 1 ] = isx.bitangent[ 2 ];

    w2t[ 0 ][ 2 ] = isx.normal[ 0 ];
    w2t[ 1 ][ 2 ] = isx.normal[ 1 ];
    w2t[ 2 ][ 2 ] = isx.normal[ 2 ];

    glm::vec3 wo( glm::normalize( w2t * woW ) );
    glm::vec3 wi( glm::normalize( w2t * wiW ) );

    glm::vec3 color( 0.f );
    float rand0( distribution( generator ) );
    int nBxDFs( bxdfs.size() );
    int iBxDF( 0 );

    for( float weight : bxdf_weights ){
        if( rand0 <= weight || iBxDF == nBxDFs - 1 ){
            color = bxdfs[ iBxDF ]->EvaluateScatteredEnergy( wo, wi )
                    * isx.texture_color
                    * isx.object_hit->material->base_color;
            break;
        }else{
            rand0 -= weight;
            ++iBxDF;
        }
    }

    return color;
}

glm::vec3 WeightedMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDFType flags) const
{
    return isx.object_hit->material->base_color;
    //TODO
    glm::mat3 w2t;

    w2t[ 0 ][ 0 ] = isx.tangent[ 0 ];
    w2t[ 1 ][ 0 ] = isx.tangent[ 1 ];
    w2t[ 2 ][ 0 ] = isx.tangent[ 2 ];

    w2t[ 0 ][ 1 ] = isx.bitangent[ 0 ];
    w2t[ 1 ][ 1 ] = isx.bitangent[ 1 ];
    w2t[ 2 ][ 1 ] = isx.bitangent[ 2 ];

    w2t[ 0 ][ 2 ] = isx.normal[ 0 ];
    w2t[ 1 ][ 2 ] = isx.normal[ 1 ];
    w2t[ 2 ][ 2 ] = isx.normal[ 2 ];

    glm::mat3 t2w( glm::transpose( w2t ) );
    glm::vec3 wo( glm::normalize( w2t * woW ) );
    glm::vec3 wi_ret( 0.f );
    glm::vec3 color( 0.f );

    float rand0( distribution( generator ) );
    float rand1( distribution( generator ) );
    float rand2( distribution( generator ) );
    int nBxDFs( bxdfs.size() );
    int iBxDF( 0 );

    for( float weight : bxdf_weights ){
        if( rand0 <= weight || iBxDF == nBxDFs - 1 ){
            color = bxdfs[ iBxDF ]->SampleAndEvaluateScatteredEnergy( wo, wi_ret, rand1, rand2, pdf_ret )
                    * isx.object_hit->material->base_color
                    * isx.texture_color;
            wiW_ret = t2w * wi_ret;
            break;
        }else{
            rand0 -= weight;
            ++iBxDF;
        }
    }

    return color;
}
