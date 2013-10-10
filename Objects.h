#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "Sprites.h"
#include "Vec3.h"
#include <boost/foreach.hpp>
#include "CollisionResolver.h"

enum Color { White = 0, Red, Green, Blue, Cyan, Purple, Yellow, Orange, COLOR_COUNT };




class Object : public ICollidable {
public:
    Object( Vec3 p, Vec3 d, float a, Color c )
        : pos(p), dimensions(d), angle_degrees(a), color(c)
    {
        w = dimensions.x;
        h = dimensions.y;

        sprite_number = 10;
        wdiv2 = (w*1.4f) / 2.0f;
        hdiv2 = h / 2.0f;
        velocity = 0.25f;
    }

    virtual void draw( const Sprites& sprites ) const
    {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, sprites.texture_lookup[ sprite_number ] );

        glMatrixMode( GL_TEXTURE );
            glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );

        glPushMatrix();
            // move the frame of reference to the x,y,z coords of the block being drawn
            // rotate by angle around centre
            glTranslatef( pos.x, pos.y, pos.z );
            glRotatef( angle_degrees, 0, 0,-1 );

            /// draw the textured quad
            glBegin( GL_QUADS );
                glTexCoord2f( 0,1 );
                glVertex3f( -wdiv2, hdiv2, 0 );
                glTexCoord2f( 0,0 );
                glVertex3f( -wdiv2, -hdiv2, 0 );
                glTexCoord2f( 1,0 );
                glVertex3f( wdiv2, -hdiv2, 0 );
                glTexCoord2f( 1,1 );
                glVertex3f( wdiv2, hdiv2, 0 );
            glEnd();
        glPopMatrix();
    }

    Collision_Volume collision_volume() const
    {
        std::vector<Vec3> vertices;

        Vec3 X( cos( deg_to_rad * angle_degrees ), -sin( deg_to_rad * angle_degrees ), 0 );
        Vec3 Y( sin( deg_to_rad * angle_degrees ), cos( deg_to_rad * angle_degrees ), 0 );

        X *= dimensions.x / 2;
        Y *= dimensions.y / 2;

        vertices.push_back( position() - X - Y );
        vertices.push_back( position() + X - Y );
        vertices.push_back( position() + X + Y );
        vertices.push_back( position() - X + Y );

        std::vector<SeparatingAxis> axes;

        axes.push_back( SeparatingAxis( vertices[1] - vertices[0], vertices ));
        axes.push_back( SeparatingAxis( vertices[3] - vertices[0], vertices ));

        return Collision_Volume( axes, vertices );
    }

    virtual std::string type_name() const { return "Object"; };

    Vec3 position() const { return pos; }
    void position( const Vec3& p ) { pos = p; }
    void move_by( const Vec3& vec ) { pos += vec; }
    float orientation() { return angle_degrees; }

    uint8_t sprite_number;
    GLfloat w;
    GLfloat h;
    GLfloat wdiv2;
    GLfloat hdiv2;
    GLfloat velocity;

    Vec3 pos;
    Vec3 dimensions;
    float angle_degrees;
    Color color;
};

enum Steerable { No_Steer = 0, Steer };

class Wheel {
public:
    Wheel( Steerable steer, Vec3 off )
    :   can_steer( steer ),
        offset( off )
    { ; }

    void apply_steering( float steer, float car_angle )
    {
        if ( can_steer )
            angle = steer + car_angle;
        else
            angle = car_angle;
    }

    Vec3 forward_unit_vector()
    {
        float wheel_world_angle = angle;
        return Vec3( sin( deg_to_rad * wheel_world_angle ), cos( deg_to_rad * wheel_world_angle ), 0 );
    }

    Vec3 lateral_unit_vector()
    {
        float wheel_world_angle = angle;
        return Vec3( cos( deg_to_rad * wheel_world_angle ), -sin( deg_to_rad * wheel_world_angle ), 0 );
    }

    Vec3 velocity_forward_component( Vec3 car_velocity )
    {
        return forward_speed( car_velocity ) * forward_unit_vector();
    }

    Vec3 velocity_lateral_component( Vec3 car_velocity )
    {
        return lateral_speed( car_velocity ) * lateral_unit_vector();
    }

    float forward_speed( Vec3 car_velocity )
    {
        return car_velocity.dot( forward_unit_vector());
    }

    float lateral_speed( Vec3 car_velocity )
    {
        return car_velocity.dot( lateral_unit_vector());
    }

    /*Vec3 calculate_velocity( Vec3 car_velocity, float car_angular_velocity )
    {
        Vec3 velocity = car_velocity;
        // using lateral
        velocity += lateral_unit_vector() * car_angular_velocity *
    }*/

    Steerable can_steer;
    bool brake_lock;
    float angle;
    Vec3 offset;
};

class Car2D : public Object {
public:
    Car2D( Vec3 p, Vec3 d )
    :   Object( p, d, 0.0f, Purple ),
        mass( 1500.0f ), // kg
        velocity( 0.0f,0.0f,0 ),
        forces( 0,0,0 )
    {
        angular_velocity = 0;
        torques = 0;
        steering_angle = 0;

        inertia = 0.8f * mass * dimensions.magnitude_squared();

        wheels.push_back( Wheel( Steer,    d * Vec3( -0.5f,  0.5f, 0 )));
        wheels.push_back( Wheel( Steer,    d * Vec3(  0.5f,  0.5f, 0 )));
        wheels.push_back( Wheel( No_Steer, d * Vec3(  0.5f, -0.5f, 0 )));
        wheels.push_back( Wheel( No_Steer, d * Vec3( -0.5f, -0.5f, 0 )));
    }

    Vec3 forward_unit_vector()
    {
        return Vec3( sin( deg_to_rad * angle_degrees ), cos( deg_to_rad * angle_degrees ), 0 );
    }

    Vec3 lateral_unit_vector()
    {
        return Vec3( cos( deg_to_rad * angle_degrees ), -sin( deg_to_rad * angle_degrees ), 0 );
    }

    void apply_drag()
    {
        float drag_coefficient = 0.4f;
        Vec3 drag = ( drag_coefficient * velocity.magnitude() * -velocity );

        //draw_vector( pos, drag, Orange );

        forces += drag;
    }


    /// for each wheel
    ///    use car velocity & car angular velocity to
    ///        calculate wheel velocity
    ///    use wheel velocity & steering angle to
    ///        calculate wheel force
    ////    sum all wheel forces and apply to car forces & car torque

    ///  apply drag, drive & external forces to car forces & car torque
    ///  use car forces to calculate new car velocity
    //  use car torque to calculate new car angular velocity
    ///  store car velocity & car angular velocity for use next frame
    void update( float timestep )
    {
        //draw_vector( pos, forces, Blue );
        //std::cout << "Velocity: " << velocity << std::endl;

        unsigned int wheel_count = 4;

        /// for each wheel
        for ( unsigned int w = 0; w != wheel_count; ++w )
        {
            Vec3 wheel_world_position = pos + (wheels[ w ].offset.x - 0.0f) * lateral_unit_vector() + (wheels[ w ].offset.y - 0.0f) * forward_unit_vector();

            // distances from centre of mass
            Vec3 centre_of_mass_to_wheel = wheel_world_position - pos;
            Vec3 rotation_tangent( centre_of_mass_to_wheel.y, -centre_of_mass_to_wheel.x, 0 );
            rotation_tangent.normalise();

            wheels[ w ].apply_steering( steering_angle, angle_degrees );
            //draw_vector( wheel_world_position, 10.0f * wheels[ w ].forward_unit_vector(), White );

            ///    use car velocity & car angular velocity to
            ///        calculate wheel velocity

            // wheel going same speed/direction as car
            Vec3 wheel_velocity = velocity;

            // except when car is also rotating
            wheel_velocity += (rotation_tangent * angular_velocity * centre_of_mass_to_wheel.magnitude());

           // draw_vector( pos, velocity, Green );
            //draw_vector( wheel_world_position, wheel_velocity * 5, Yellow );

            ///    use wheel velocity & steering angle to
            ///        calculate wheel forces

            // assume the wheels share the load of the car body equally
            float load = mass / wheel_count;
            float friction_coefficient = 0.7f;
            float rr_coefficient = 0.015f;

            // FIXME: wheel velocity magnitude technically incorrect here - should be normalised,
            // but doing so gives bad behaviour
            Vec3 F_lat_wheel = friction_coefficient * load * 9.98 * -wheels[w].velocity_lateral_component( wheel_velocity );
            Vec3 F_long_wheel = rr_coefficient * load * 9.98 * -wheels[w].velocity_forward_component( wheel_velocity );

            /// testing locked back wheels
            //if ( wheels[ w ].brake_lock )
                //F_lat_wheel /= load;


      //      draw_vector( wheel_world_position, tyre_forward_unit_vector, White );
       //     draw_vector( wheel_world_position, Vec3(1,0,0), Cyan );
          //  draw_vector( wheel_world_position, F_lat_wheel, Red );
          //  draw_vector( wheel_world_position, F_long_wheel, Red );
            ///draw_vector( wheel_world_position, (F_long_wheel + F_lat_wheel), Red );

            //std::cout << "Velocity (x,y): " << velocity << std::endl;
            //std::cout << "Friction (lat,long): " << F_lat_wheel << ", " << F_long_wheel << std::endl;
            //std::cout << "Lateral speed: " << wheels[w].lateral_speed( velocity ) << std::endl;

            /// apply the wheel's forces to the car body
            forces += F_lat_wheel;
            forces += F_long_wheel;

      ///      draw_vector( wheel_world_position, 10.0f*rotation_tangent, White );

            /// apply the wheel's forces to the torque of the car body
            float rotational_force_magnitude = rotation_tangent.dot( F_lat_wheel + F_long_wheel );

            torques += rotational_force_magnitude * centre_of_mass_to_wheel.magnitude();
            ///draw_vector( wheel_world_position, 10.0f * (rotation_tangent * angular_velocity * centre_of_mass_to_wheel.magnitude()), Orange );

            /// draw velocities
            //draw_vector( pos, velocity, Green );
            //draw_vector( wheel_world_position, wheel_velocity, Yellow );
        }

        /// draw torque
        ///draw_vector( pos + forward_unit_vector(), torques * lateral_unit_vector(), Cyan );
        //draw_vector( pos + forward_unit_vector(), forward_unit_vector(), White );

        ///  apply drag, drive & external forces to car forces & car torque
        apply_drag();

        ///draw_vector( pos, forces, Blue );

        ///  use car forces to calculate new car velocity
        Vec3 acceleration = forces / mass;
        velocity += acceleration * timestep;
        //std::cout << "Velocity: " << velocity.magnitude() << std::endl;
        pos += velocity * timestep;

        //threshold_print( std::cout, 0.001f, torques ) << " / ";
        //threshold_print( std::cout, 0.001f, inertia ) << " = ";
        //threshold_print( std::cout, 0.001f, torques / inertia ) << std::endl;

        ///  use car torque to calculate new car angular velocity
        float angular_acceleration = torques / inertia;
        angular_velocity += angular_acceleration * timestep;
        angle_degrees += rad_to_deg * angular_velocity * timestep;

        //std::cout << "Angular Velocity: ";
        //threshold_print( std::cout, 0.001f, angular_velocity,     "Rotation Stopped." ) << std::endl;

        //std::cout << std::endl;

        ///  store car velocity & car angular velocity for use next frame
        forces = Vec3(0,0,0);
        torques = 0;
    }

    void accelerate()
    {
        // thrust = (P:W) * mass * grav / velocity
        float thrust = std::min( 1856340.0f / velocity.magnitude(), 30000.0f );

        // alternative: thrust = (T:W) * mass

        forces += forward_unit_vector() * thrust;
    }

    void brake()
    {
        forces += forward_unit_vector() * -2000.0f;
    }

    void handbrake( bool locked )
    {
        BOOST_FOREACH( Wheel wheel, wheels )
        {
            if ( ! wheel.can_steer )
                wheel.brake_lock = locked;
        }
    }

    void centre_steering()
    {
        if ( steering_angle > 1.0f )
            steering_angle -= 1.0f;
        else if ( steering_angle < -1.0f )
            steering_angle += 1.0f;
        else
            steering_angle = 0.0f;
    }

    void steer( float steering_change )
    {
        float steering_max = 50.0f;

        steering_angle += steering_change;

        if ( steering_angle < -steering_max )
            steering_angle = -steering_max;

        if ( steering_angle > steering_max )
            steering_angle = steering_max;
    }

  /*  void resolve_collision( CollisionManifold manifold )
    {
        if ( manifold.collision_detected == true )
        {
            // correct car position such that it no longer intersects the collided object
            Vec3 position_correction = manifold.normal * manifold.depth;
            pos += position_correction;

            // apply wall friction and collision elasticity coefficients to get new velocity
            Vec3 velocity_parallel_to_normal = velocity.dot( manifold.normal ) * manifold.normal;
            Vec3 velocity_perpendicular_to_normal = velocity - velocity_parallel_to_normal;

            velocity = (0.9f * velocity_perpendicular_to_normal) + (-0.4f * velocity_parallel_to_normal);
        }
    }
*/
    virtual void draw( const Sprites& sprites ) const
    {
        Object::draw( sprites );
    }

    Vec3 get_velocity() { return velocity; }
    void set_velocity( const Vec3& v ) { velocity = v; }

    std::string type_name() const { return "Car2d"; }

private:
    std::vector<Wheel> wheels;

    float mass;
    Vec3 velocity;
    Vec3 forces;

    float inertia;
    float angular_velocity;
    float torques;

    float steering_angle;
};


class Objects {
public:
    void spawn()
    {

    }

private:
    std::vector<Object> objects;
};

#endif // _OBJECTS_H_
