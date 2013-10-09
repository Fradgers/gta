#include "ICollidable.h"

enum Color { White = 0, Red, Green, Blue, Cyan, Purple, Yellow, Orange, COLOR_COUNT };
GLfloat colors[COLOR_COUNT][3] = {
    { 1, 1, 1 },
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 },
    { 0, 1, 1 },
    { 1, 0, 1 },
    { 1, 1, 0 },
    { 1,.5f, 0 },
};

CollisionManifold project( const std::vector<SeparatingAxis>& axes, const std::vector<Vec3>& vertices )
{
    Vec3 collision_normal;
    float collision_depth = 0x3f800000;

    // for both axes of this OBB
    for ( unsigned int axis = 0; axis != axes.size(); ++axis )
    {
        axes[ axis ].print();

        // project vertices onto the axis
        // find min and max positions on axis
        float extent = axes[ axis ].extent( vertices[0] );

        float min_extent = extent;
        float max_extent = extent;

        for ( unsigned int vertex = 1; vertex != vertices.size(); ++vertex )
        {
            extent = axes[ axis ].extent( vertices[ vertex ] );

            min_extent = std::min( min_extent, extent );
            max_extent = std::max( max_extent, extent );
        }

        // no overlap, bail out early
        if (
            ( min_extent > axes[ axis ].max() ) ||
            ( max_extent < axes[ axis ].min() )
        )
        {
            std::cout << "No overlap - no collision." << std::endl;
            return CollisionManifold();
        }

        float depthA = axes[ axis ].max() - min_extent;
        float depthB = axes[ axis ].min() - max_extent;

        std::cout << "Depths: A(" << depthA << ") B(" << depthB << ")" << std::endl;

        // check if depthA or depthB is the smallest collision depth so far
        if ( std::abs(depthA) < std::abs(collision_depth) )
        {
            collision_depth = depthA;
            collision_normal = axes[ axis ].direction().normalise();
        }

        if ( std::abs(depthB) < std::abs(collision_depth) )
        {
            collision_depth = depthB;
            collision_normal = axes[ axis ].direction().normalise();
        }
    }
    std::cout << "Normal:" << collision_normal << " Depth:" << collision_depth << std::endl;

    return CollisionManifold( collision_normal, collision_depth );
}

#include <boost/foreach.hpp>
void Collision_Volume::draw() const
{
    glPushMatrix();

    glColor3fv( colors[ White ] );
    glBegin( GL_LINE_LOOP );
        BOOST_FOREACH( const Vec3& vertex, vertices )
            glVertex3f( vertex.x, vertex.y, 2.0f );
    glEnd();

    glPopMatrix();
}

/*ICollidable::~ICollidable() { ; }
//Collision_Volume::~Collision_Volume() { ; }

bool intersect_impl( const Oriented_Bounding_Box& O1, const Oriented_Bounding_Box& O2 )
{
    return false;
}

bool intersect_impl( const Oriented_Bounding_Box& O, const AA_Bounding_Box& A )
{
    return false;
}

bool intersect_impl( const AA_Bounding_Box& A1, const AA_Bounding_Box& A2 )
{
    return false;
}

bool intersect_impl( const AA_Bounding_Box& A, const Line_Segment& L )
{
    return false;
}

bool intersect_impl( const Line_Segment& L1, const Line_Segment& L2 )
{
    return false;
}

bool intersect_impl( const Line_Segment& L, const Oriented_Bounding_Box& O )
{
    return false;
}
*/
