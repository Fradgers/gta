#ifndef _COLLISIONRESOLVER_H_
#define _COLLISIONRESOLVER_H_

#include "ICollidable.h"
#include <map>
#include <utility>

class CollisionResolver {
public:
    typedef std::pair< std::string, std::string > TypePair;
    typedef void (*CollisionResolutionFunc)( ICollidable*, ICollidable*, const CollisionManifold& );
    typedef std::map< TypePair, CollisionResolutionFunc > CollisionResolutionMap;

    void subscribe( const std::string& typeA, const std::string& typeB, CollisionResolutionFunc func )
    {
        TypePair type_pair = std::make_pair( typeA, typeB );
        collision_resolution_map[ type_pair ] = func;

        TypePair reversed_pair = std::make_pair( typeB, typeA );
        collision_resolution_map[ reversed_pair ] = func;
    }

    void resolve( ICollidable& collidableA, ICollidable& collidableB, const CollisionManifold& manifold )
    {
        TypePair type_pair = std::make_pair( collidableA.type_name(), collidableB.type_name() );
        CollisionResolutionMap::iterator iter = collision_resolution_map.find( type_pair );

        // if collision function exists, call it on the given collidables
        if ( iter != collision_resolution_map.end() )
        {
            ( *iter ).second( &collidableA, &collidableB, manifold );
        }
    }

private:
    CollisionResolutionMap collision_resolution_map;
};

void resolve_car2d_mapcolumn( ICollidable* car, ICollidable* mapcolumn, const CollisionManifold& manifold );
void resolve_car2d_car2d( ICollidable* carA, ICollidable* carB, const CollisionManifold& manifold );


#endif // _COLLISIONRESOLVER_H_
