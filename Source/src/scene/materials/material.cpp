#include <scene/materials/material.h>
#include <scene/materials/bxdfs/lambertBxDF.h>
#include <QColor>
#include <math.h>

Material::Material() :
    Material(glm::vec3(0.5f, 0.5f, 0.5f))
{}

Material::Material(const glm::vec3 &color):
    name("MATERIAL"),
    bxdfs(),
    is_light_source(false),
    base_color(color),
    intensity(0)
{
    texture = NULL;
    normal_map = NULL;
}

glm::vec3 Material::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags) const
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

    return bxdfs.front()->EvaluateScatteredEnergy( wo, wi )
            * isx.texture_color
            * isx.object_hit->material->base_color;
}

glm::vec3 Material::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDFType flags) const
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

    glm::mat3 t2w( glm::transpose( w2t ) );
    glm::vec3 wo( glm::normalize( w2t * woW ) );
    glm::vec3 wi( 0.f );
    float PDF( 0.f );
    float rand1( distribution( generator ) );
    float rand2( distribution( generator ) );

    glm::vec3 color( bxdfs.front()->SampleAndEvaluateScatteredEnergy( wo, wi, rand1, rand2, PDF )
                     * isx.object_hit->material->base_color
                     * isx.texture_color );

    wiW_ret = t2w * wi;
    pdf_ret = PDF;

    return color;
}

glm::vec3 Material::EvaluateHemisphereScatteredEnergy(const Intersection &isx, const glm::vec3 &wo, int num_samples, BxDFType flags) const
{
    //TODO
    return glm::vec3(0);
}

glm::vec3 Material::GetImageColor(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL || uv_coord.x < 0 || uv_coord.y < 0 || uv_coord.x >= 1.0f || uv_coord.y >= 1.0f)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue())/255.0f;
    }
}

glm::vec3 Material::GetImageColorInterp(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL || uv_coord.x < 0 || uv_coord.y < 0 || uv_coord.x >= 1.0f || uv_coord.y >= 1.0f)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        //Use bilinear interp.

        float X = image->width() * uv_coord.x;
        float Y = image->height() * (1.0f - uv_coord.y);

        glm::vec2 floors = glm::vec2(floor(X), floor(Y));
        glm::vec2 ceils = glm::vec2(ceil(X), ceil(Y));
        ceils = glm::min(ceils, glm::vec2(image->width()-1, image->height()-1));

        QColor qll = image->pixel(floors.x, floors.y); glm::vec3 ll(qll.red(), qll.green(), qll.blue());
        QColor qlr = image->pixel(ceils.x, floors.y); glm::vec3 lr(qlr.red(), qlr.green(), qlr.blue());
        QColor qul = image->pixel(floors.x, ceils.y); glm::vec3 ul(qul.red(), qul.green(), qul.blue());
        QColor qur = image->pixel(ceils.x, ceils.y); glm::vec3 ur(qur.red(), qur.green(), qur.blue());

        float distX = (X - floors.x);
        glm::vec3 color_low = ll * (1-distX) + lr * distX;
        glm::vec3 color_high = ul * (1-distX) + ur * distX;

        float distY = (Y - floors.y);

        glm::vec3 result = (color_low * (1 - distY) + color_high * distY)/255.0f;

        return result;


        glm::vec2 XY(X,Y);
        //Want floor and ceil of both X and Y
        //Do square interp of <X,Y>

        float dist_ll = glm::distance(XY, floors);
        float dist_lr = glm::distance(XY, glm::vec2(ceils.x, floors.y));
        float dist_ul = glm::distance(XY, glm::vec2(floors.x, ceils.y));
        float dist_ur = glm::distance(XY, ceils);
        float sum = dist_ll + dist_lr + dist_ul + dist_ur;

        float llc = (1 - dist_ll/sum);
        float lrc = (1 - dist_lr/sum);
        float ulc = (1 - dist_ul/sum);
        float urc = (1 - dist_ur/sum);

        float checksum = llc + lrc + ulc + urc;

        glm::vec3 final_color = llc * ll + lrc * lr + ulc * ul + urc * ur;
        return final_color/255.0f;
    }
}
