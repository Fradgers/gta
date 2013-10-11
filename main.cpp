
#include <fstream>

#include "Tiles.h"
#include "Objects.h"
#include "City.h"
#include "CollisionResolver.h"

GLfloat depth = 40.0f;

using namespace std;

/** TODO:

only draw the visible portion of the map

implement map objects in general - ON HOLD ( need to parse script files for that )
*/

void load_files( const std::string& style_file, const std::string& map_file, Sprites* sprites, Tiles* tiles, City* city )
{
    std::ifstream style_stream( style_file.c_str(), ios::in | ios::binary );
    if ( ! style_stream.is_open() ) return;

    std::ifstream map_stream( map_file.c_str(), ios::in | ios::binary );
    if ( ! map_stream.is_open() ) return;

    Style style( style_stream );
    Map map( map_stream );

    sprites->init( style );

    tiles->init( style, map );

    city->init( map, *tiles );
}

/**
In progress:
    change Map_Column to use its own stored x,y position

    change City to load Map_Columns into a 2D array,
    rather than storing a 2D array of indices

    fix boost array out of range crash - City.h line 337 ("@@@")
    boost array heap!
*/

int main( int argc, char** argv )
{
    std::cerr << "Program started." << std::endl;

    OpenGL opengl;
    if ( ! opengl.good() ) return -1;
    glEnable( GL_TEXTURE_2D );

    Sprites sprites;
    City city;
    Tiles tiles;

    Car2D car( Vec3(75.5f,83.0f,2.0f), Vec3(0.7f,2.0f,0.0f) );
    Car2D car2( Vec3(77.5f,83.0f,2.0f), Vec3(0.7f,2.0f,0.0f) );
    GLfloat camera_orientation = 0.0f;

    Vec3 camera;
    //load_files( "data/bil.sty", "data/MP1-comp.gmp", &sprites, &tiles, &city );
    //camera( 0.0f, 0.0f, 12.0f );

    load_files( "data/wil.sty", "data/wil.gmp", &sprites, &tiles, &city );
    camera = Vec3( 168.5f,96.5f,12.0f );

    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );

    glAlphaFunc( GL_NOTEQUAL, 0.0f );
    glEnable(  GL_ALPHA_TEST );

    double time = 0;
    int frame_counter = 0;

    CollisionResolver collision_resolver;
    collision_resolver.subscribe( "Car2d", "Map_Column", resolve_car2d_mapcolumn );
    collision_resolver.subscribe( "Car2d", "Car2d", resolve_car2d_car2d );

    while ( ! glfwGetKey( GLFW_KEY_ESC ) )
    {
        double current_time = glfwGetTime();
        if ( current_time > time + 1.0 )
        {
            time = current_time;

            std::stringstream stream;
            stream << "GTA " << frame_counter;

            glfwSetWindowTitle( stream.str().c_str() );

            frame_counter = 0;
        }

        ++frame_counter;

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        camera_orientation = car.orientation();
        glRotatef( camera_orientation, 0.0f,0.0f,1.0f );

        depth = std::min( 30.0f, std::max( 20.0f, car.get_velocity().magnitude() * 1.0f ));
        camera = car.position() + Vec3( 0.0f, 0.0f, depth ) + car.forward_unit_vector() * 2.0f;
        glTranslatef( -camera.x, -camera.y, -camera.z );

        city.draw( tiles, camera );

        car.collision_volume().draw(); // highlight collision in white
        car2.collision_volume().draw();

        car.draw( sprites );
        car2.draw( sprites );

        car.update(0.02f);
        car2.update(0.02f);

        car.centre_steering();

        if ( glfwGetKey( 'W' )) car.accelerate();
        if ( glfwGetKey( 'S' )) car.brake();
        if ( glfwGetKey( 'D' )) car.steer(3.0f);
        if ( glfwGetKey( 'A' )) car.steer(-3.0f);

        // check for collisions between car and obj
        CollisionManifold collision = car2.collision_volume().intersects( car.collision_volume() );

        if ( collision.collision_detected )
            collision_resolver.resolve( car, car2, collision );

        // for all map blocks immediately around car
        const int detection_radius = 5;

        for ( int x = static_cast<uint8_t>(car.position().x) - detection_radius; x != static_cast<uint8_t>(car.position().x) + detection_radius; ++x )
            for ( int y = static_cast<uint8_t>(car.position().y) - detection_radius; y != static_cast<uint8_t>(car.position().y) + detection_radius; ++y )
            {
                // check the column at these coords has a block at the car's z-coord
                Map_Column& column = city.column_at( x, y );
                Map_Block* block = city.block_at( x, y, static_cast< uint8_t >( car.position().z ));

                if ( block != NULL && (block->faces[ Left ].collide || block->faces[ Right ].collide || block->faces[ Front ].collide || block->faces[ Rear ].collide ))
                //if ( column.block_at_height( static_cast< uint8_t >( car.position().z )))
                {
                    // if it does, check for collision between the car and the map column
                    collision = column.collision_volume().intersects( car.collision_volume() );

                    if ( collision.collision_detected )
                    {
                        // resolve the first collision detected and skip to next frame
                        collision_resolver.resolve( car, column, collision );
                        break;
                    }
                }
            }

        tiles.update_animations();

        glfwSwapBuffers();
    }

    return 0;
}


/*
class Oriented_Bounding_Box {
public:
    Oriented_Bounding_Box( const Object& obj )
    {
        Vec3 X( cos( obj.angle ), sin( obj.angle ), 0 );
        Vec3 Y( sin( obj.angle ), cos( obj.angle ), 0 );

        X *= obj.w / 2;
        Y *= obj.h / 2;

        corner[0] = obj.position - X - Y;
        corner[1] = obj.position + X - Y;
        corner[2] = obj.position + X + Y;
        corner[3] = obj.position - X + Y;

        compute_axes();
    }

    Oriented_Bounding_Box( const Map_Block& block, const Vec3& coords )
    {

    }

    //Oriented_Bounding_Box(  )

    bool intersects( const Oriented_Bounding_Box& other ) const
    {
        return this->overlaps_one_way( other ) && other.overlaps_one_way( *this );
    }

private:


    bool overlaps_one_way( const Oriented_Bounding_Box& other ) const
    {
        // for both axes of this OBB
        for ( int a = 0; a < 2; ++a )
        {
            // project vertices of other OBB onto the axis
            // find min and max positions on axis
            float t = other.corner[0].dot( axis[a] );

            float tMin = t;
            float tMax = t;

            for ( int c = 1; c < 4; ++c )
            {
                t = other.corner[c].dot( axis[a] );

                if ( t < tMin )
                    tMin = t;
                else
                    if ( t > tMax )
                        tMax = t;
            }

            // no overlap, bail out early
            if (( tMin > origin[a] + 1 ) || ( tMax < origin[a] ))
                return false;
        }

        return true;
    }

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

    Vec3 corner[4];
    Vec3 axis[2];
    float origin[2];
};*/


/*bool collision( const Object& object, const City& city )
{
    // for each map_tile in object proximity
        // column = city.columns[ map_tile ]
        // if ( column.bottom < object.position.z < column.top )
            // if ( collision( object, column.block_at_height( object.position.z )) )
                // add block to potential_collisions

    // for each block in potential_collisions list
        // for each block_face in block
            // if ( block_face.collidable )
                // if ( collision( object, block_face ) )
                    // object.react_to_collision( block_face )
    return true;
}
*/


/*    GLfloat fogColor[4]= {0.45f, 0.55f, 0.45f, 1.0f};
    glFogi(GL_FOG_MODE, GL_LINEAR );
    glFogfv( GL_FOG_COLOR, fogColor );
    glFogf(GL_FOG_DENSITY, 0.05f);
    glFogf(GL_FOG_START, 30.0f);
    glFogf(GL_FOG_END, 100.0f);
    glEnable( GL_FOG );*/

/**class Object {
public:
    GLfloat x; // the position data in the map file must be converted before use
    GLfloat y;
    GLfloat z_rotation; // 0-255
    unsigned int archetype_id;
};

class Objects {
public:
    void init( const Style& style, const Map& map, const Sprites& sprites )
    {
        if ( ! ( style.obji && style.sprb && map.mobj )) return;

        uint16_t current_sprite = style.sprb->map_obj_sprite_base;

        for ( size_t obj = 0; obj != style.obji->objects.size(); ++obj )
        {
            Object_Archetype arche;

            arche.model_id = style.obji->objects[ obj ].model_number;

            // if object has 0 sprites, use sprite from previous object
            // it is assumed that the first object has >0 sprites
            if ( style.obji->objects[obj].sprite_count == 0 )
                arche.sprites.push_back( sprites.texture_lookup[ current_sprite ]);

            for ( unsigned int sprite = 0; sprite <= style.obji->objects[ obj ].sprite_count; ++sprite )
            {
                ++current_sprite;
                arche.sprites.push_back( sprites.texture_lookup[ current_sprite ] );
            }

            archetypes.push_back( arche );
        }

        for ( size_t obj = 0; obj != map.mobj->map_objects.size(); ++obj )
        {
            Object object;


            object.z_rotation = map.mobj->map_objects[ obj ].rotation * GLfloat( 1.40625f ) ;
            object.archetype_id = map.mobj->map_objects[ obj ].object_type;
        }
    }

    std::vector<Object_Archetype> archetypes;
    std::vector<Object> objects;
};*/
