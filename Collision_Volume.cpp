#include "Collision_Volume.h"

bool project( const std::vector<Collision::Axis>& axes, const std::vector<Vec3>& vertices )
{
    // for both axes of this OBB
    for ( unsigned int axis = 0; axis != axes.size(); ++axis )
    {
        //axes[ axis ].print();

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

        //std::cout << min_extent << ' ' << max_extent << '\n';

        // no overlap, bail out early
        if (
            ( min_extent > axes[ axis ].max() ) ||
            ( max_extent < axes[ axis ].min() )
        )
        {
            //std::cout << " No Collision.";
            return false;
        }
    }

    //std::cout << " COLLISION!!";
    return true;

    /*
    if ( axes.empty() || vertices.empty() )
        return false;

    // for all axes
    for ( unsigned int a = 0; a != axes.size(); ++a )
    {
        // project vertices onto the current axis
        // find min and max positions on axis
        float t = vertices[0].dot( axes[a].vector() );

        float tMin = t;
        float tMax = t;

        // for remaining vertices
        for ( unsigned int v = 1; v != vertices.size(); ++v )
        {
            t = vertices[v].dot( axes[a].vector() );

            if ( t < tMin )
                tMin = t;
            else
                if ( t > tMax )
                    tMax = t;
        }

        // no overlap, bail out early
        if (( tMin > axes[a].max() ) || ( tMax < axes[a].min() ))
            return false;
    }

    return true;*/
}

#include "OpenGL.h"

void Collision::Collision_Volume::draw( const Vec3& colour ) const
{
    glPushMatrix();

    glDisable( GL_TEXTURE_2D );

    glColor3f( colour.x, colour.y, colour.z );
    glBegin( GL_LINE_LOOP );
        BOOST_FOREACH( const Vec3& vertex, vertices )
            glVertex3f( vertex.x, vertex.y, vertex.z );
    glEnd();

    glColor3f( 1.0f, 1.0f, 1.0f );

    glEnable( GL_TEXTURE_2D );

    glPopMatrix();
}

ICollidable::~ICollidable() { ; }

