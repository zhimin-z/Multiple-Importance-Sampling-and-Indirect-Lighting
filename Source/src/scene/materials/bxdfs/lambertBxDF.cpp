
#include <scene/materials/bxdfs/lambertBxDF.h>



glm::vec3 LambertBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    return diffuse_color * INV_PI;
}

glm::vec3 LambertBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return EvaluateScatteredEnergy( glm::vec3( 0.f ), glm::vec3( 0.f ) );
}
