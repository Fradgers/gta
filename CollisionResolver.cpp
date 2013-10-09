
#include "CollisionResolver.h"
#include "Objects.h"

void resolve_car2d_object( ICollidable* collidableA, ICollidable* collidableB, const CollisionManifold& manifold )
{
    std::cout << std::endl << "Called resolve_car2d_object()" << std::endl << std::endl;

    if ( manifold.collision_detected == true )
    {
        Car2D* car = dynamic_cast< Car2D* >( collidableA );
        //Object* object = dynamic_cast< Object* >( collidableB );

        // correct car position such that it no longer intersects the collided object
        Vec3 position_correction = manifold.normal * manifold.depth;
        car->move_by( position_correction );

        // apply wall friction and collision elasticity coefficients to get new velocity
        Vec3 velocity_parallel_to_normal = car->get_velocity().dot( manifold.normal ) * manifold.normal;
        Vec3 velocity_perpendicular_to_normal = car->get_velocity() - velocity_parallel_to_normal;

        car->set_velocity(( 0.9f * velocity_perpendicular_to_normal ) + ( -0.4f * velocity_parallel_to_normal ));
    }
}

void resolve_car2d_car2d( ICollidable* collidableA, ICollidable* collidableB, const CollisionManifold& manifold )
{
    std::cout << std::endl << "Called resolve_car2d_car2d()" << std::endl << std::endl;

    if ( manifold.collision_detected == true )
    {
        Car2D* carA = dynamic_cast< Car2D* >( collidableA );
        //Car2D* carB = dynamic_cast< Car2D* >( collidableB );

        // correct car position such that it no longer intersects the collided object
        Vec3 position_correction = manifold.normal * manifold.depth;
        carA->move_by( position_correction );

        // apply wall friction and collision elasticity coefficients to get new velocity
        Vec3 velocity_parallel_to_normal = carA->get_velocity().dot( manifold.normal ) * manifold.normal;
        Vec3 velocity_perpendicular_to_normal = carA->get_velocity() - velocity_parallel_to_normal;

        carA->set_velocity(( 0.9f * velocity_perpendicular_to_normal ) + ( -0.4f * velocity_parallel_to_normal ));
    }
}
