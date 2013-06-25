

class Face_Texture {
public:
    Face_Texture() {}

    Face_Texture( uint16_t data )
    {
        angle = GLfloat( 90.0f ) * (( data >> 14 ) & 0x3 );
        texture_xflip = ( data >> 13 ) & 0x1;
        double_sided = ( data >> 12 ) & 0x1;
        texture_id = ( data & 0x3FF );
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
                default : break;
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

#include "Vec3.h"
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

    // zero texture is always invisible
    if ( texture_id == 0 ) return;
    if ( face.type == No_Face ) return;

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

#include "Collision_Volume.h"

class Map_Column : public ICollidable {
public:
    Map_Column()
    :   ICollidable( Vec3(76,83,2) ),
        bottom( 0 ),
        top( 0 )
    { ; }

    Map_Column( uint8_t height, uint8_t offset )
    :   ICollidable(Vec3(84,75,2))
    {
        bottom = offset;
        top = height;
    }

    /*const Map_Column& operator=( const Map_Column& other )
    {
        this->top = other.top;
        this->bottom = other.bottom;
        this->block_indices = other.block_indices;

        return *this;
    }*/

    uint32_t block_at_height( uint8_t height )
    {
        if ( height >= top || height < bottom )
            return 0;
        else
            return block_indices[ height - bottom ];
    }


    Collision::Collision_Volume collision_volume() const
    {
        std::vector<Vec3> vertices;

        Vec3 X( 0.5f, 0, 0 );
        Vec3 Y( 0, 0.5f, 0 );

        vertices.push_back( pos - X - Y );
        vertices.push_back( pos + X - Y ); // [1]
        vertices.push_back( pos + X + Y );
        vertices.push_back( pos - X + Y ); // [3]

        std::vector<Collision::Axis> axes;

        axes.push_back( Collision::Axis( Vec3( 1,0,0 ), vertices ));
        axes.push_back( Collision::Axis( Vec3( 0,1,0 ), vertices ));

        return Collision::Collision_Volume( axes, vertices );
    }

    virtual void collision_response( const Collision::Physics_Data& ) { ; }

    virtual ~Map_Column() { ; }

    void position( const Vec3& new_pos ) { pos = new_pos; }
    void add_block( uint32_t block ) { block_indices.push_back( block ); }

    uint8_t bottom;
    uint8_t top;
    std::vector<uint32_t> block_indices;
};


#include <boost/array.hpp>
class City {
public:
    City()
    :   map_columns(
            new boost::array< boost::array< Map_Column, 256 >, 256 >()
        )
    { ; }

    void init( const Map& map, const Tiles& tiles )
    {
        if ( ! map.dmap ) return;

        boost::array< boost::array<uint32_t, 256>, 256 > map_indices;

        // load map
        for ( int y = 0; y != 256; ++y )
        {
            for ( int x = 0; x != 256; ++x )
            {
                map_indices[x][y] = map.dmap->base[ y*256 + x ];
            }
        }

        // load columns
        std::map<uint32_t, Map_Column> columns;

        for (
            std::map<uint32_t, dmap_column_binary_info>::iterator c = map.dmap->columns.begin();
            c != map.dmap->columns.end();
            ++c
        ) {
            Map_Column column( c->second.height, c->second.offset );

            for ( unsigned int c_block = 0; c_block != c->second.blockd.size(); ++c_block )
            {
                column.add_block( c->second.blockd[ c_block ] );
            }

            columns.insert( std::make_pair( c->first, column ));
        }

        // convert column info
        for ( int y = 0; y != 256; ++y )
        {
            for ( int x = 0; x != 256; ++x )
            {
                column_at( x, y ) = columns.at( map_indices[ x ][ 255-y ] );
                column_at( x, y ).position( Vec3( x, y, 0 ));
            }
        }

        // convert block info
        for ( unsigned int b = 0; b != map.dmap->blocks.size(); ++b )
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

            map_blocks.push_back( block );
        }
    }

    ///void draw(
    void draw( const Tiles& tiles, const Vec3& camera )
    {

        int x_halfwidth = camera.z * 0.5f;
        int y_halfwidth = camera.z * 0.375f;

        int x_min = camera.x - x_halfwidth - 2;
        int x_max = camera.x + x_halfwidth + 2;

        int y_min = camera.y - y_halfwidth - 2;
        int y_max = camera.y + y_halfwidth + 2;

        for ( unsigned int z = 0; z != 7; ++z )
        {
            /// 0 to 256
            for ( int x = x_min; x != x_max; ++x )
            {
                for ( int y = y_min; y != y_max; ++y )
                {
                    // map is defined south-to-north
                    Vec3 coords = Vec3( x, y, z );

                    Map_Column& column = column_at( x, y );

                    if ( coords.z >= column.bottom && coords.z < column.top )
                    {
                        uint32_t block_id = column.block_at_height( static_cast<uint8_t>( coords.z ));

                        const Map_Block& block = map_blocks[ block_id ];

                        glPushMatrix();
                            face_draw( block, Rear, tiles, block.faces[ Rear ].texture_id, coords );
                            face_draw( block, Right, tiles, block.faces[ Right ].texture_id, coords );
                            face_draw( block, Front, tiles, block.faces[ Front ].texture_id, coords );
                            face_draw( block, Left, tiles, block.faces[ Left ].texture_id, coords );
                            face_draw( block, Lid, tiles, block.faces[ Lid ].texture_id, coords, false );
                        glPopMatrix();
                    }
                } // y
            } // x
        } // z
    }

    Map_Column& column_at( unsigned int x, unsigned int y )
    {
        return map_columns->at( x ).at( y );
    }

private:
    boost::array< boost::array<Map_Column, 256>, 256 >* map_columns;
    std::vector<Map_Block> map_blocks;

};


   /* void collide( const Object& object )
    {
        const Collision::Collision_Volume& object_volume = object.collision_volume();

        int object_z = object.position.z;

        int x_min = std::max( (int)object.position.x - 3, 0   );
        int x_max = std::min( (int)object.position.x + 4, 255 );
        int y_min = std::max( (int)object.position.y - 3, 0   );
        int y_max = std::min( (int)object.position.y + 4, 255 );

        //std::cout << '\n';

        for ( int x = x_min; x != x_max; ++x )
            for ( int y = 255-y_max; y != 255-y_min; ++y )
            {
                Map_Column& column = columns.at( map_tiles[ x ][ y ] );

                if ( column.block_at_height( object_z ) != 0 )
                {
               //     std::cout << "(" << x << ',' << (255-y) << "): ";

                    Collision::Collision_Volume column_volume = column.collision_volume( Vec3( x, 255-y, object_z + 4 ));

                    if ( object_volume.intersects( column_volume ))
                        column_volume.draw( Vec3( 0.0f, 1.0f, 0.0f ));
                    else
                        column_volume.draw( Vec3( 1.0f, 0.0f, 0.0f ));

                 //   std::cout << std::endl;
                }
            }
    }*/
