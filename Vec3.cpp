#include "Vec3.h"

std::ostream& operator<<( std::ostream& stream, const Vec3& vec )
{
    stream << '(' << vec.x << ',' << vec.y << ',' << vec.z << ')';
    return stream;
}

Vec3 operator+( const Vec3& lhs, const Vec3& rhs )
{
    return Vec3( lhs ) += rhs;
}

Vec3 operator-( const Vec3& lhs, const Vec3& rhs )
{
    return Vec3( lhs ) -= rhs;
}

Vec3 operator*( const Vec3& lhs, const float& rhs )
{
    return Vec3( lhs ) *= rhs;
}

Vec3 operator/( const Vec3& lhs, const float& rhs )
{
    return Vec3( lhs ) /= rhs;
}

Vec3 operator*( const float& lhs, const Vec3& rhs )
{
    return Vec3( rhs ) *= lhs;
}
