
#ifndef _VEC3_H_
#define _VEC3_H_

#include <cmath>

const float deg_to_rad = 3.1416f / 180.0f;
const float rad_to_deg = 1.0f / deg_to_rad;

class Vec3 {
public:
    typedef Vec3 Self;

    Vec3() { ; }

    Vec3( float x_, float y_, float z_ )
    {
        x = x_;
        y = y_;
        z = z_;
    }

    Self& operator+=( const Vec3& rhs )
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    Self& operator-=( const Vec3& rhs )
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    Self& operator*=( const Self& rhs )
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;

        return *this;
    }

    Self& operator*=( const float& rhs )
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;

        return *this;
    }

    Self& operator/=( const float& rhs )
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;

        return *this;
    }

    Vec3 operator-() const
    {
        return Vec3( -x, -y, -z );
    }

    Self& normalise()
    {
        return operator*=( 1.0 / magnitude() );
    }

    float dot( const Vec3& rhs ) const
    {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    float magnitude_squared() const
    {
        //return dot( *this );
        return x*x + y*y + z*z;
    }

    float magnitude() const
    {
        return std::sqrt( magnitude_squared() );
    }

    float x, y, z;
};


#include <iostream>
std::ostream& operator<<( std::ostream& stream, const Vec3& vec );

Vec3 operator+( const Vec3& lhs, const Vec3& rhs );
Vec3 operator-( const Vec3& lhs, const Vec3& rhs );
Vec3 operator*( const Vec3& lhs, const Vec3& rhs );

Vec3 operator*( const Vec3& lhs, const float& rhs );
Vec3 operator/( const Vec3& lhs, const float& rhs );
Vec3 operator*( const float& lhs, const Vec3& rhs );

#endif
