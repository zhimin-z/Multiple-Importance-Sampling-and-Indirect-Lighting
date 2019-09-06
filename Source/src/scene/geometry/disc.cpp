#include <scene/geometry/disc.h>

void Disc::ComputeArea()
{
    //TODO
    glm::vec4 vertices[]{
        { -.5f, -.5f, 0.f, 1.f, },
        { .5f, -.5f, 0.f, 1.f, },
        { -.5f, .5f, 0.f, 1.f, },
    };

    glm::vec4 vertices_world[ 3 ];

    for( int i = 0; i < 3; ++i ){
        vertices_world[ i ] = transform.T() * vertices[ i ];
    }

    glm::vec3 v01( vertices_world[ 1 ] - vertices_world[ 0 ] );
    glm::vec3 v03( vertices_world[ 2 ] - vertices_world[ 0 ] );

    area = .25f * PI * glm::length( glm::cross( v01, v03 ) );
}

Intersection Disc::SampleLight( float a, float b, float c ){

    float sqrt_r( sqrt( a ) );
    float theta( b * TWO_PI );

    float x( sqrt_r * cos( theta ) );
    float y( sqrt_r * sin( theta ) );

    glm::vec3 o( x * 0.5f, y * 0.5f, 1.f );
    glm::vec3 d( 0.f, 0.f, -1.f );

    Ray ray( o, d );

    return GetIntersection( ray.GetTransformedCopy( transform.T() ) );
}

Intersection Disc::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    glm::vec4 P = glm::vec4(t * r_loc.direction + r_loc.origin, 1);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 0.25f)
    {
        glm::vec3 N( ComputeNormal( glm::vec3( P ) ) );

        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4( N, 0 )));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        //TODO: Store the tangent and bitangent

        glm::vec4 T( 1.f, 0.f, 0.f, 0.f );
        glm::vec4 B( 0.f, 1.f, 0.f, 0.f );

        result.tangent = glm::normalize( glm::vec3( transform.T() * T ) );
        result.bitangent = glm::normalize( glm::vec3( transform.T() * B ) );

        return result;
    }
    return result;
}

glm::vec2 Disc::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 Disc::ComputeNormal(const glm::vec3 &P)
{
    return glm::vec3(0,0,1);
}

void Disc::create()
{
    GLuint idx[54];
    //18 tris, 54 indices
    glm::vec3 vert_pos[20];
    glm::vec3 vert_nor[20];
    glm::vec3 vert_col[20];

    //Fill the positions, normals, and colors
    glm::vec4 pt(0.5f, 0, 0, 1);
    float angle = 18.0f * DEG2RAD;
    glm::vec3 axis(0,0,1);
    for(int i = 0; i < 20; i++)
    {
        //Position
        glm::vec3 new_pt = glm::vec3(glm::rotate(glm::mat4(1.0f), angle * i, axis) * pt);
        vert_pos[i] = new_pt;
        //Normal
        vert_nor[i] = glm::vec3(0,0,1);
        //Color
        vert_col[i] = material->base_color;
    }

    //Fill the indices.
    int index = 0;
    for(int i = 0; i < 18; i++)
    {
        idx[index++] = 0;
        idx[index++] = i + 1;
        idx[index++] = i + 2;
    }

    count = 54;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx, 54 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(vert_pos, 20 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(vert_nor, 20 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(vert_col, 20 * sizeof(glm::vec3));
}
