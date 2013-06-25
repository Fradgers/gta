
#ifndef _COLLISION_VOLUME_H_
#define _COLLISION_VOLUME_H_

#include "Vec3.h"
/*
enum Color { White = 0, Red, Green, Blue, COLOR_COUNT };

GLfloat colors[COLOR_COUNT][3] = {
    { 1,1,1 },
    { 1,0,0 },
    { 0,1,0 },
    { 0,0,1 },
};*/

#include <vector>
#include <limits>
#include <iostream>
#include <iomanip>

#include <boost/foreach.hpp>
namespace Collision {

class Axis {
public:
    Axis( const Vec3& d, const std::vector<Vec3>& points )
    :   direction(d),
        min_extent( std::numeric_limits<float>::max() ),
        max_extent( -min_extent )
    {
        for ( unsigned int p = 0; p != points.size(); ++p )
        {
            float e = extent( points[ p ] );

            if ( min_extent > e ) min_extent = e;
            if ( max_extent < e ) max_extent = e;
        }
    }

    const Vec3& vector() const { return direction; }

    float max() const { return max_extent; }
    float min() const { return min_extent; }

    float extent( const Vec3& point ) const
    {
        return direction.dot( point );
    }

    void print() const
    {
        std::cout << std::setprecision(3) << direction << " Min: " << min() << " Max: " << max() << std::endl;
    }

private:
    Vec3 direction;
    float min_extent;
    float max_extent;
};

} // </ namespace Collision>


bool project( const std::vector<Collision::Axis>& axes, const std::vector<Vec3>& vertices );


namespace Collision {

class Collision_Volume {
public:
    Collision_Volume( const std::vector<Axis>& as, const std::vector<Vec3>& vs )
    :   axes(as), vertices(vs)
    { ; }

    bool intersects( const Collision_Volume& other ) const
    {
        return ( project( axes, other.vertices ) && project( other.axes, vertices ));
    }

    /*void translate_vertices( const Vec3& translation )
    {
        BOOST_FOREACH( Vec3& vertex, vertices )
        {
            vertex += translation;
        }
    }*/

    void draw( const Vec3& colour = Vec3( 1.0f, 1.0f, 1.0f )) const;

private:
    std::vector<Axis> axes;
    std::vector<Vec3> vertices;
};

struct Physics_Data { ; };

} // </ namespace Collision >


class ICollidable {
public:
    ICollidable( const Vec3& position = Vec3( 0,0,0 ))
    :   pos(position)
    { ; }

    virtual Collision::Collision_Volume collision_volume() const = 0;
    virtual void collision_response( const Collision::Physics_Data& ) = 0;

    Vec3 position() const
    {
        return pos;
    }

    virtual Collision::Physics_Data collision_data() const
    {
        return physics_data;
    }

    virtual ~ICollidable();

    Vec3 pos;
protected:
    Collision::Physics_Data physics_data;
};

#endif

