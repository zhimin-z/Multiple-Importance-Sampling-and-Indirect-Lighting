#pragma once
#include <scene/materials/material.h>

//Unlike a default Material, the WeightedMaterial has a set of floats that weight each BxDF this Material contains
//It is assumed by this class that the sum of all the weights is 1.
//When choosing a random BxDF to sample in EvaluateScatteredEnergy,
//each BxDF's probability is weighted by its weight
//This might be used, for example, to create a material that is 50% diffuse and 50% mirrored
class WeightedMaterial : public Material
{
public:
//Constructors/Destructors
    WeightedMaterial();
    WeightedMaterial(const glm::vec3 &color);
//Functions
    //Given an intersection with some Geometry, evaluate the scattered energy at isx given a world-space wo and wi for all BxDFs we contain that match the input flags
    virtual glm::vec3 EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags = BSDF_ALL) const;

    //Given an intersection with some Geometry, generate a world-space wi then evaluate the scattered energy along the world-space wo.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDFType flags = BSDF_ALL) const;
//Members
    QList<float> bxdf_weights;
};
