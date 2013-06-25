#include <iostream>
#include <stdlib.h>

#include "Sprite.h"

using namespace std;

#include <stdint.h>
#include <fstream>
#include <vector>
#include <iostream>

#include <cassert>
#include <sstream>

#include "Map.h"
#include "Style.h"

using namespace std;

class Map {
public:
    enum Chunk_Id {
        CMAP = 1346456899,
        DMAP = 1346456900,
        ZONE = 1162760026,
        EDIT = 1414087749,
        LGHT = 1414022988,
        RGEN = 1313163090,
        ANIM = 1296649793,
        PSXM = 1297634128
    };
    enum Endian_Check { GBMP = 1347240519, PMBG = 1195527504 };

    Map( std::ifstream& stream )
    :   dmap( NULL )
    {
        binary_reader map_header_reader( stream, 6 );

        if ( map_header_reader.as<uint32_t>( 0 ) == GBMP )
            std::cout << "<<GBMP>> :: Endian correct" << endl;
        else if ( map_header_reader.as<uint32_t>( 0 ) == PMBG )
            std::cout << "<<GBMP>> :: Endian inverse" << endl;

        while ( ! stream.eof() )
        {
            chunk_reader map_chunk_reader( stream );

            switch ( map_chunk_reader.chunk_type )
            {
                case DMAP :
                {
                    cout << "<DMAP>" << endl;
                    stringstream body_stream( map_chunk_reader.body( stream ));
                    dmap = new dmap_reader( body_stream );
                    break;
                }
                case CMAP :
                case ZONE :
                case PSXM :
                case ANIM :
                {
                    cout << "<ANIM>" << endl;
                    stringstream body_stream( map_chunk_reader.body( stream ));
                    anim = new anim_reader( body_stream );
                    break;
                }
                case LGHT :
                case EDIT :
                case RGEN :
                {
                    map_chunk_reader.body( stream );
                    cout << "<OTHER>" << endl;
                    break;
                }
                default :
                {
                    std::cout << "<OTHER> - " << map_chunk_reader.chunk_type << std::endl;
                    map_chunk_reader.body( stream );
                    break;
                }
            }
        }
    }

    /*void draw( const Tiles& tiles, const Vec3& camera )
    {
        if ( ! dmap ) return;

        if ( coords.z >= offset && coords.z < height )
        {
            uint32_t block_id = blockd[ coords.z - offset ];

            const block_binary_info& block = block_info[ block_id ];

            glPushMatrix();
                glEnable( GL_TEXTURE_2D );

                    face_draw( block, Rear, tiles.tile( block.face_texture( Rear )), coords );
                    face_draw( block, Right, tiles.tile( block.face_texture( Right )), coords );
                    face_draw( block, Front, tiles.tile( block.face_texture( Front )), coords );
                    face_draw( block, Left, tiles.tile( block.face_texture( Left )), coords );
                    face_draw( block, Lid, tiles.tile( block.face_texture( Lid )), coords );

                glDisable( GL_TEXTURE_2D );
            glPopMatrix();


            /// !!!
            ///break;
        }
    }

        //if ( dmap ) dmap->draw( tiles, camera );
    }*/

    ~Map()
    {
        if ( dmap ) delete dmap;
        if ( anim ) delete anim;
    }

    dmap_reader* dmap;
    anim_reader* anim;
};



class Animation {
public:
    Animation()
    {
        updates_per_frame = 1;
        current_frame_index = 0;
        num_updates_until_next_frame = 1;
    }

    unsigned int updates_per_frame;
    unsigned int current_frame_index;
    unsigned int num_updates_until_next_frame;

    uint16_t current_frame() const
    {
        return frames[ current_frame_index ];
    }

    std::vector<uint16_t> frames;
};


class Tiles {
public:
    Texture_Collection::Texture_Name placeholder_texture;
    Tiles()
    {
        placeholder_texture = textures.load_texture( "placeholder.tga" );
        depaletted_pixels = new uint32_t[ 4*64 * 4*64 * 62 ];
    }

    void init( const Style& style, const Map& map )
    {
        if ( ! ( style.ppal && style.tile && style.palx )) return;

        // for each row of textures
        for ( size_t texture_row = 0; texture_row != 4 * 62; ++texture_row )
        {
            // read 4 textures
            for ( unsigned int pixel_row = 0; pixel_row != 64; ++pixel_row )
            {
                for ( unsigned int column = 0; column != 4*64; ++column )
                {
                    int texture_column = column / 64;
                    int pixel_column = column - ( texture_column * 64 );

                    int texture_idx = texture_row * 4 + texture_column;
                    int pixel_idx = (63-pixel_row) * 64 + pixel_column;

                    uint8_t pixel = binary_reader( style.tile->color_index_data, 1 ).as<uint8_t>( 0 );
                    depaletted_pixels[ (texture_idx*64*64) + pixel_idx ] = style.ppal->color_at( style.palx->ppalette( texture_idx ), pixel );
                }
            }

            // after reading the 4 textures in the row of textures,
            // load the 4 textures into opengl and save their texture names
            for ( size_t texture_column = 0; texture_column != 4; ++texture_column )
            {
                int texture_idx = texture_row * 4 + texture_column;
                texture_lookup[ texture_idx ] = textures.load_texture( depaletted_pixels + ( texture_idx*64*64 ));
            }
        }

        // initialise animated tiles
        if ( ! map.anim ) return;

        for ( int a = 0; a != map.anim->animations.size(); ++a )
        {
            Animation animation;

            for ( int frame = 0; frame != map.anim->animations[a].frames.size(); ++frame )
            {
                animation.updates_per_frame = map.anim->animations[a].game_cycles_per_frame;
                animation.frames.push_back( map.anim->animations[a].frames[ frame ] );
            }

            animations.insert( make_pair( map.anim->animations[a].base_tile, animation ));
        }
    }


    Texture_Collection::Texture_Name tile( uint16_t texture_id ) const
    {
        std::map<uint16_t, Animation>::const_iterator iter = animations.find( texture_id );
        if ( iter != animations.end() )
        {
            return texture_lookup[ iter->second.current_frame() ];
        }

        return texture_lookup[ texture_id ];
    }

    void update_animations()
    {
        for (
            std::map<uint16_t, Animation>::iterator iter = animations.begin();
            iter != animations.end();
            ++iter
        ) {
            Animation& animation = iter->second;

            animation.num_updates_until_next_frame -= 1;

            if ( animation.num_updates_until_next_frame <= 0 )
            {
                animation.num_updates_until_next_frame = animation.updates_per_frame;
                ++animation.current_frame_index;

                if ( animation.current_frame_index >= animation.frames.size() )
                    animation.current_frame_index = 0;
            }
        }
    }


    ~Tiles() { delete [] depaletted_pixels; }

    std::map<uint16_t, Animation> animations;

    Texture_Collection textures;
    Texture_Collection::Texture_Name texture_lookup[992];
    uint32_t* depaletted_pixels;
};


class Face_Texture {
public:
    Face_Texture() {}

    Face_Texture( uint16_t data )
    {
        angle = GLfloat( 90.0f ) * (( data >> 14 ) & 0x3 );
        texture_xflip = ( data >> 13 ) & 0x1;
        double_sided = ( data >> 12 ) & 0x1;
        texture_id = ( data & 0x3FF );

        // debug code !! REMOVE !!
        //if ( double_sided ) texture_id = 31;
    }

    GLfloat angle;
    bool texture_xflip;
    uint16_t texture_id;
    bool double_sided;
};

#include "geometry_data.h"
class Map_Block {
public:
    Map_Block( uint16_t rear, uint16_t right, uint16_t front, uint16_t left, uint16_t lid, uint8_t block_type )
    {
        faces[ Rear ] = Face_Texture( rear );
        faces[ Right ] = Face_Texture( right );
        faces[ Front ] = Face_Texture( front );
        faces[ Left ] = Face_Texture( left );
        faces[ Lid ] = Face_Texture( lid );

        type = Block_Type(( block_type >> 2 ) & 0x3F );

        // handle diagonal slope special case
        if ( faces[ Lid ].texture_id != 1023 )
        {
            switch ( type )
            {
                case Diagonal_Slope1_NW : type = Diagonal_Slope3_NW; break;
                case Diagonal_Slope1_NE : type = Diagonal_Slope3_NE; break;
                case Diagonal_Slope1_SW : type = Diagonal_Slope3_SW; break;
                case Diagonal_Slope1_SE : type = Diagonal_Slope3_SE; break;
            }
        }
    }

    uint8_t arrows;
    Block_Type type;
    Face_Texture faces[5]; // rear, right, front, left, lid

//    bool double_sided_faces[5];
};


GLfloat clamp( GLfloat value, const GLfloat& min, const GLfloat& max )
{
    // modifying and returning a copy
    value = std::max( value, min );
    value = std::min( value, max );

    return value;
}


void shift_side_into_position( const GLfloat& facing_angle, const GLfloat& lean_angle, const GLfloat& offset )
{
    glRotatef( facing_angle, 0,0,1 );
    glTranslatef( 0, -offset-0.5f, 0 );
    glRotatef( 90.0f+lean_angle, 1,0,0 );
}

const GLfloat deg_to_rad = 3.1416f / 180.0f;

#include <cmath>
void shift_lid_into_position( const GLfloat& rotation_angle, const Axis& axis, const GLfloat& offset )
{
    glTranslatef( 0,0, offset );

    if ( axis & X_Axis ) glRotatef( rotation_angle, 1,0,0 );
    if ( axis & Y_Axis ) glRotatef( rotation_angle, 0,1,0 );
    if ( axis & Z_Axis ) glRotatef( rotation_angle, 0,0,1 );
}


// rotate and flip texture prior to attaching to quad vertices
void texture_rotate_and_flip( const Face_Texture& face_texture )
{
    glMatrixMode( GL_TEXTURE );
        glLoadIdentity();
        glTranslatef( 0.5f, 0.5f, 0.0f );
        glRotatef( face_texture.angle, 0,0,1 );

        if ( face_texture.texture_xflip )
            glRotatef( 180.0f, 0,1,0 );

        glTranslatef( -0.5f, -0.5f, 0.0f );
    glMatrixMode( GL_MODELVIEW );
}

void reverse_order( unsigned int& vertex_number )
{
    vertex_number = 3 - vertex_number;
    return;
}

Block_Face opposite_side( const Block_Face& this_side )
{
    switch ( this_side )
    {
        case Front : return Rear;
        case Rear : return Front;
        case Left : return Right;
        case Right : return Left;
        default : return Lid;
    }
}

/// default param should == true !!! FIX !!!
void face_draw( const Map_Block& block, const Block_Face& side, const Tiles& tiles, uint16_t texture_id, const Vec3& coords, bool check_for_double_sided = true )
{
    bool draw_reversed = false;

    Block_Face target_side = side;

    if ( check_for_double_sided )
    {
        draw_reversed = block.faces[ opposite_side(side) ].double_sided;
        if ( draw_reversed )
            target_side = opposite_side( side );
    }

    Face& face = blocks[ block.type ][ target_side ];

  //  std::cerr << "<" << face.type << ">" << std::endl;

    // zero texture is always invisible
    if ( texture_id == 0 ) return;
    if ( face.type == No_Face ) return;

    glColor3f( 1,1,1 );

    glBindTexture( GL_TEXTURE_2D, tiles.tile( texture_id ));
    glPushMatrix();

    texture_rotate_and_flip( block.faces[ side ]);

    // move the frame of reference to the x,y,z coords of the block being drawn
    glTranslatef( coords.x, coords.y, coords.z );

    // rotate and translate the 5 quads to form a cube
    switch ( target_side )
    {
        case Rear  : shift_side_into_position( 180.0f + face.rotation, face.lean, face.offset ); break;
        case Right : shift_side_into_position( 90.0f + face.rotation, face.lean, face.offset ); break;
        case Front : shift_side_into_position( 0.0f + face.rotation, face.lean, face.offset ); break;
        case Left  : shift_side_into_position( 270.0f + face.rotation, face.lean, face.offset ); break;

        case Lid : shift_lid_into_position( face.rotation, face.axis, face.offset ); break;
        default : break;
    }

    // move block such that origin is in centre
    glTranslatef( -0.5f, -0.5f, 0 );

    /// draw the textured quad
    glBegin( GL_QUADS );

        for ( unsigned int vertex = 0; vertex != 4; ++vertex )
        {
            unsigned int tex_vertex = vertex;

            if ( draw_reversed ) reverse_order( vertex );

            // tex coord is same as x,y of vertex coord to prevent stretching of texture
            // clamped to prevent tiling when quad is bigger than texture
            glTexCoord2f(
                clamp( faces[ face.type ][ tex_vertex*3 ], 0.0f, 1.0f ),
                clamp( faces[ face.type ][ tex_vertex*3 + 1 ], 0.0f, 1.0f )
            );

            glVertex3f(
                faces[ face.type ][ vertex*3 ],
                faces[ face.type ][ vertex*3 + 1 ],
                ( draw_reversed ) ? -0.01f : 0.0f // insert slight gap between neighbouring blocks
            );


            if ( draw_reversed ) reverse_order( vertex );
        }
    glEnd();

    glPopMatrix();

    // if just drew reverse side, and current side isn't flat
    if ( draw_reversed && block.faces[ side ].double_sided )
    {
        // draw front side
        face_draw( block, side, tiles, texture_id, coords, false );
    }
}



class Map_Column {
public:
    Map_Column( uint8_t height, uint8_t offset )
    {
        bottom = offset;
        top = /*offset +*/ height;
    }

    uint32_t block_at_height( uint8_t height )
    {
        if ( height >= top || height < bottom )
            return 0;
        else
            return block_indices[ height - bottom ];
    }

    uint8_t top;
    uint8_t bottom;
    std::vector<uint32_t> block_indices;
};

#include <boost/array.hpp>
class City {
public:
    void init( const Map& map, const Tiles& tiles )
    {
        if ( ! map.dmap ) return;

        // convert map info
        for ( int y = 0; y != 256; ++y )
        {
            for ( int x = 0; x != 256; ++x )
            {
                map_tiles[x][y] = map.dmap->base[ y*256 + x ];
              //  std::cout << x << " " << y << " " << map_tiles[ x ][ y ] << std::endl;
            }
        }

        // convert column info
        for (
            std::map<uint32_t, dmap_column_binary_info>::iterator c = map.dmap->columns.begin();
            c != map.dmap->columns.end();
            ++c
        ) {
            Map_Column column( c->second.height, c->second.offset );

            for ( int c_block = 0; c_block != c->second.blockd.size(); ++c_block )
            {
                column.block_indices.push_back( c->second.blockd[ c_block ] );
            }

            columns.insert( make_pair( c->first, column ));
        }

        // convert block info
        for ( int b = 0; b != map.dmap->blocks.size(); ++b )
        {
            Map_Block block(
                map.dmap->blocks[ b ].faces[ Rear ],
                map.dmap->blocks[ b ].faces[ Right ],
                map.dmap->blocks[ b ].faces[ Front ],
                map.dmap->blocks[ b ].faces[ Left ],
                map.dmap->blocks[ b ].faces[ Lid ],
                map.dmap->blocks[b].slope_type
            );

            block.arrows = map.dmap->blocks[b].arrows;

            //block.type = Block_Type( static_cast<uint8_t>(( map.dmap->blocks[b].slope_type >> 2 ) & 0x3F ));

            map_blocks.push_back( block );
        }
    }

    ///void draw(
    void draw( const Tiles& tiles, const Vec3& camera )
    {
        for ( unsigned int z = 0; z != 7; ++z )
        {
            /// 0 to 256
            for ( int x = 0; x != 256; ++x )
            {
                for ( unsigned int y = 0; y != 256; ++y )
                {
                    uint32_t column_index = map_tiles[ x ][ y ];

                    // map is defined south-to-north
                    Vec3 coords = Vec3( x, 255-y, z );

                    Map_Column& column = columns.at( column_index );

                    if ( coords.z >= column.bottom && coords.z < column.top )
                    {
                        uint32_t block_id = column.block_at_height( static_cast<uint8_t>( coords.z ));

                        const Map_Block& block = map_blocks[ block_id ];

                        glPushMatrix();
                            glEnable( GL_TEXTURE_2D );
                                face_draw( block, Rear, tiles, block.faces[ Rear ].texture_id, coords );
                                face_draw( block, Right, tiles, block.faces[ Right ].texture_id, coords );
                                face_draw( block, Front, tiles, block.faces[ Front ].texture_id, coords );
                                face_draw( block, Left, tiles, block.faces[ Left ].texture_id, coords );
                                face_draw( block, Lid, tiles, block.faces[ Lid ].texture_id, coords, false );
                            glDisable( GL_TEXTURE_2D );
                        glPopMatrix();
                    }
                }
            } // y
        } // x
    } // z

    boost::array< boost::array<uint32_t, 256>, 256 > map_tiles;
    std::map<uint32_t, Map_Column> columns;
    std::vector<Map_Block> map_blocks;

};



/** TODO:

only draw the visible portion of the map

implement diagonal-slope faces

implement map objects in general - ON HOLD ( need to parse script files for that )
*/

void load_files( const std::string& style_file, const std::string& map_file, Sprites* sprites, Tiles* tiles, City* city/*, Animations* animations*/ )
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


class Object {
public:
    Object()
    {
        sprite_number = 1;
        width = 32;
        height = 64;
        position = Vec3( 76.5f, 83.0f, 1.5f );
        angle = 35.0f;
        velocity = 0.25f;
    }

    Vec3 movement_vector()
    {
        return Vec3(
            velocity * sin( deg_to_rad * angle ),
            velocity * cos( deg_to_rad * angle ),
            0
        );
    }

    void draw( const Sprites& sprites )
    {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, sprites.texture_lookup[ sprite_number ] );

        glMatrixMode( GL_TEXTURE );
            glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );

        glPushMatrix();
            // move the frame of reference to the x,y,z coords of the block being drawn
            // rotate by angle around centre
            glTranslatef( position.x + 0.5f, position.y + 0.5f, position.z );
            glRotatef( -angle, 0, 0, 1 );
            glTranslatef( -0.5f, -0.5f, 0 );

            /// draw the textured quad
            glBegin( GL_QUADS );
                glTexCoord2f( 0,1 );
                glVertex3f( 0,1,0 );
                glTexCoord2f( 0,0 );
                glVertex3f( 0,0,0 );
                glTexCoord2f( 1,0 );
                glVertex3f( 1,0,0 );
                glTexCoord2f( 1,1 );
                glVertex3f( 1,1,0 );
            glEnd();
        glPopMatrix();

        glDisable( GL_TEXTURE_2D );
    }

    uint8_t sprite_number;
    uint8_t width;
    uint8_t height;
    Vec3 position;
    GLfloat angle;
    GLfloat velocity;
};

class Oriented_Bounding_Box {
public:
    Oriented_Bounding_Box( const Object& obj )
    {
        Vec3 X( cos( obj.angle ), sin( obj.angle ), 0 );
        Vec3 Y( sin( obj.angle ), cos( obj.angle ), 0 );

        X *= obj.width / 2;
        Y *= obj.height / 2;

        corner[0] = obj.position - X - Y;
        corner[1] = obj.position + X - Y;
        corner[2] = obj.position + X + Y;
        corner[3] = obj.position - X + Y;

        compute_axes();
    }

    bool intersects( const Oriented_Bounding_Box& other ) const
    {
        return this->overlaps_one_way( other ) && other.overlaps_one_way( *this );
    }

private:
    bool overlaps_one_way( const Oriented_Bounding_Box& other ) const
    {
        for ( int a = 0; a < 2; ++a )
        {
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
};




int main( int argc, char** argv )
{
    OpenGL opengl;
    if ( ! opengl.good() ) return -1;

    Sprites sprites;
    City city;
    Tiles tiles;

    Object obj1;
    Vec3 camera( 168.0f,107.0f,18.0f );

    //load_files( "bil.sty", "MP1-comp.gmp", &sprites, &tiles, &city );
    //obj1.position = Vec3( 76.5f, 83.0f, 20.0f );

    load_files( "wil.sty", "wil.gmp", &sprites, &tiles, &city );
    obj1.position = Vec3( 140.0f, 105.0f, 2.0f );

    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );

    glAlphaFunc( GL_NOTEQUAL, 0.0f );
    glEnable(  GL_ALPHA_TEST );

    while ( ! glfwGetKey( GLFW_KEY_ESC ) )
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        if ( glfwGetKey( GLFW_KEY_RIGHT )){ camera += Vec3( 0.03f, 0.0f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_LEFT )) {  camera += Vec3(  -0.03f, 0.0f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_UP ))  { camera += Vec3(  0.0f, 0.03f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_DOWN )){ camera += Vec3( 0.0f, -0.03f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}

        if ( glfwGetKey( GLFW_KEY_KP_6 )){ camera += Vec3( 0.5f, 0.0f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_KP_4 )) {  camera += Vec3(  -0.5f, 0.0f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_KP_8 ))  { camera += Vec3(  0.0f, 0.5f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_KP_2 )){ camera += Vec3( 0.0f, -0.5f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}

        if ( glfwGetKey( GLFW_KEY_KP_ADD )) camera += Vec3( 0, 0, -1.0f );
        if ( glfwGetKey( GLFW_KEY_KP_SUBTRACT )) camera += Vec3( 0, 0, 1.0f );

        if ( glfwGetKey( 'D' ) ) obj1.angle += 5.5f;
        if ( glfwGetKey( 'A' ) ) obj1.angle -= 5.5f;
        if ( glfwGetKey( 'W' ) ) obj1.position += obj1.movement_vector();
        if ( glfwGetKey( 'S' ) ) obj1.position -= obj1.movement_vector();

        tiles.update_animations();


        ///camera = obj1.position + Vec3( 0, 0, 15.0f );
        GLfloat camera_orientation = obj1.angle;

        /// draw here
        ///glRotatef( camera_orientation, 0, 0, 1 );
        glRotatef( 40.0f, -1.0f, 0,0 );
        glRotatef( -45.0f, 0,0,1.0f );
        glTranslatef( -camera.x, -camera.y, -camera.z );

        city.draw( tiles, camera );
        //sprites.draw( camera.x, camera.y, 2.0f );
        obj1.draw( sprites );

        glfwSwapBuffers();
    }

    return 0;
}




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
