
#include "Objects.h"
#include "City.h"

class Collision_System {
public:
    Collision_System( ICollidable* _car, City* _city )
    :   car(_car), city(_city)
    { ; }

    void run_collisions()
    {
        int object_z = car->position().z;

        int x_min = std::max( (int)car->position().x - 3, 0   );
        int x_max = std::min( (int)car->position().x + 4, 255 );
        int y_min = std::max( (int)car->position().y - 3, 0   );
        int y_max = std::min( (int)car->position().y + 4, 255 );

        for ( int x = x_min; x != x_max; ++x )
        {
            for ( int y = y_min; y != y_max; ++y )
            {
                Map_Column& column = city->column_at( x, y );

                if ( ! column.block_at_height( object_z ))
                    continue;

                if (
                    car->collision_volume().intersects(
                        column.collision_volume()
                    )
                ) {
                    Collision::Physics_Data column_data = column.collision_data();
                    Collision::Physics_Data car_data    = car->collision_data();

                    car->collision_response( column_data );
                    column.collision_response( car_data );
                }
            }
        }
    }

private:
    ICollidable* car;

    City* city;
};
