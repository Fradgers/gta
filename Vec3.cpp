#include "Vec3.h"

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
