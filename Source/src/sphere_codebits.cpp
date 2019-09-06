//Add these functions to your sphere.cpp file
//Make sure UniformCodePDF is put at the top of the file; it's not a member of the Sphere class.

float UniformConePdf(float cosThetaMax)
{
    return 1.f / (2.f * PI * (1.f - cosThetaMax));
}

float Sphere::RayPDF(const Intersection &isx, const Ray &ray) {
    glm::vec3 Pcenter = transform.position();
    float radius = 0.5f * ( transform.getScale().x + transform.getScale().y + transform.getScale().z ) / 3.0f;
    // Return uniform weight if point inside sphere
    if (glm::distance2(isx.point, Pcenter) - radius*radius < 1e-4f)
        return Geometry::RayPDF(isx, ray);

    // Compute general sphere weight
    float sinThetaMax2 = radius*radius / glm::distance2(isx.point, Pcenter);
    float cosThetaMax = glm::sqrt(max(0.f, 1.f - sinThetaMax2));
    return UniformConePdf(cosThetaMax);
}

void Sphere::ComputeArea()
{
    glm::vec3 scale = transform.getScale();
    float a = scale.x * 0.5f;
    float b = scale.y * 0.5f;
    float c = scale.z * 0.5f;
    area = 4.f * PI * glm::pow( ( glm::pow( a * b, 1.6f ) + glm::pow( a * c, 1.6f ) + glm::pow( b * c, 1.6f ) ) / 3.0f, 1 / 1.6f );
}
