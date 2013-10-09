#ifndef _ICOLLIDABLE_H_
#define _ICOLLIDABLE_H_

#include <boost/shared_ptr.hpp>
#include "Vec3.h"


#include <vector>
#include <limits>
#include <iostream>
#include <cmath>
#include <boost/foreach.hpp>

class SeparatingAxis {
public:
    SeparatingAxis( const Vec3& d, const std::vector<Vec3>& points )
    :   dir(d),
        min_extent( std::numeric_limits<float>::max() ),
        max_extent( -min_extent )
    {
        dir.normalise();

        for ( unsigned int p = 0; p != points.size(); ++p )
        {
            float e = extent( points[ p ] );

            if ( min_extent > e ) min_extent = e;
            if ( max_extent < e ) max_extent = e;
        }
    }

    float max() const { return max_extent; }
    float min() const { return min_extent; }

    float extent( const Vec3& point ) const
    {
        return dir.dot( point );
    }

    Vec3 direction() const
    {
        return dir;
    }

    void print() const
    {
        //std::cout << "Axis:" << dir << " Min:" << min() << " Max:" << max() << std::endl;
    }

private:
    Vec3 dir;
    float min_extent;
    float max_extent;
};

class CollisionManifold {
public:
    CollisionManifold() {
        collision_detected = false;
        depth = 0.0f;
    }

    CollisionManifold(Vec3 n, float d)
    : normal(n), depth(d)
    { collision_detected = true; }

    bool collision_detected;
    Vec3 normal;
    float depth;
};

CollisionManifold project( const std::vector<SeparatingAxis>& axes, const std::vector<Vec3>& vertices );


class Collision_Volume {
public:
    Collision_Volume( const std::vector<SeparatingAxis>& as, const std::vector<Vec3>& vs )
    :   axes(as), vertices(vs)
    { ; }

    CollisionManifold intersects( const Collision_Volume& other )
    {
        // perform seperating axes algorithm on volumes
        std::cout << "Checking car's vertices against object's axes" << std::endl;
        CollisionManifold manifoldA = project( axes, other.vertices );
        std::cout << "Checking object's vertices against car's axes" << std::endl;
        CollisionManifold manifoldB = project( other.axes, vertices );

        if ( manifoldA.collision_detected && manifoldB.collision_detected )
        {
            // return the collision manifold with the least penetration
            if ( std::abs(manifoldA.depth) < std::abs(manifoldB.depth) )
            {
                std::cout << "LeastPen: Normal:" << manifoldA.normal << " Depth:" << manifoldA.depth << std::endl;
                return manifoldA;
            }
            else
            {
                // normal reversed if a vertex of object This has intersected object Other
                manifoldB.normal = -manifoldB.normal;
                std::cout << "LeastPen: Normal:" << manifoldB.normal << " Depth:" << manifoldB.depth << std::endl;
                return manifoldB;
            }
        }
        else
        {
            std::cout << "intersects: No collision." << std::endl;
            return CollisionManifold();
        }
    }

    void draw() const;

private:
    std::vector<SeparatingAxis> axes;
    std::vector<Vec3> vertices;
};

#include "Sprites.h"

class ICollidable {
public:
    virtual Collision_Volume collision_volume() const = 0;
    virtual void draw( const Sprites& ) const = 0;
    virtual Vec3 position() const = 0;
    virtual void position( const Vec3& p ) = 0;
    //virtual void move_by( const Vec3& vec ) = 0;
    virtual std::string type_name() const = 0;
};

/*class Line_Segment;
class AA_Bounding_Box;
class Oriented_Bounding_Box;

class Collision_Volume {
public:
    virtual bool intersects( Collision_Volume& ) = 0;
    virtual bool intersects( Oriented_Bounding_Box& ) = 0;
    virtual bool intersects( AA_Bounding_Box& ) = 0;

    virtual ~Collision_Volume() = 0;
};

bool intersect_impl( const Oriented_Bounding_Box& O1, const Oriented_Bounding_Box& O2 );
bool intersect_impl( const Oriented_Bounding_Box& O, const AA_Bounding_Box& A );
bool intersect_impl( const AA_Bounding_Box& A1, const AA_Bounding_Box& A2 );
bool intersect_impl( const AA_Bounding_Box& A, const Line_Segment& L );
bool intersect_impl( const Line_Segment& L1, const Line_Segment& L2 );
bool intersect_impl( const Line_Segment& L, const Oriented_Bounding_Box& O );

class Line_Segment : public Collision_Volume {
public:
    Line_Segment( Vec3, Vec3 ) { }

    virtual bool intersects( Oriented_Bounding_Box* ) = 0;
    virtual bool intersects( AA_Bounding_Box* ) = 0;
};

#include <iostream>
class AA_Bounding_Box : public Collision_Volume {
public:
    AA_Bounding_Box( Vec3, Vec3 ) { }

    virtual bool intersects( Collision_Volume& other )
    {
        return other.intersects( *this );
    }

    virtual bool intersects( Oriented_Bounding_Box& other )  { std::cout << "A-O"; return intersect_impl( other, *this ); };
    virtual bool intersects( AA_Bounding_Box& other ) { std::cout << "A-A"; return intersect_impl( *this, other ); };
};

class Oriented_Bounding_Box : public Collision_Volume {
public:
    Oriented_Bounding_Box( Vec3 position, Vec3 dimensions, float angle )
    {
        Vec3 X( cos( angle ), sin( angle ), 0 );
        Vec3 Y( sin( angle ), cos( angle ), 0 );

        X *= dimensions.x / 2;
        Y *= dimensions.y / 2;

        corner[0] = position - X - Y;
        corner[1] = position + X - Y;
        corner[2] = position + X + Y;
        corner[3] = position - X + Y;

        compute_axes();
    }

    virtual bool intersects( Collision_Volume& other )
    {
        return other.intersects( *this );
    }

    virtual bool intersects( Oriented_Bounding_Box& other )  { std::cout << "O-O"; return intersect_impl( *this, other ); };
    virtual bool intersects( AA_Bounding_Box& other )  { std::cout << "O-A"; return intersect_impl( *this, other ); };

    Vec3 corner[4];
    Vec3 axis[2];
    float origin[2];

private:
    void compute_axes()
    {
        axis[0] = corner[1] - corner[0];
        axis[1] = corner[3] - corner[0];

        for ( int a = 0; a < 2; ++a )
        {
            axis[a] /= axis[a].magnitude_squared();
            origin[a] = corner[0].dot( axis[a] );
        }
    }
};


class ICollidable {
public:
    virtual boost::shared_ptr<Collision_Volume> collision_volume() = 0;
    virtual void draw() const = 0;

    virtual ~ICollidable() = 0;
};*/


#endif // _ICOLLIDABLE_H_
