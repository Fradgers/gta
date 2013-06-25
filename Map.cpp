#include "Map.h"
#include "Style.h"

enum Shape_Type {
    Cube = 0,
    Two_Point_Slope_26,
    Two_Point_Slope_7,
    Two_Point_Slope_45,
    Diagonal,
    One_Point_Slope,
    Three_Point_Slope,
    Half_Cube,
    Quarter_Cube,
    Pillar,
    None,
    SHAPE_TYPE_COUNT
};

enum Vertex { Upper_Left = 0, Upper_Right, Lower_Right, Lower_Left, VERTEX_COUNT };

GLfloat colours[10][3] = {
    0.3f, 0.3f, 0.3f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,///
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,///
    1.0f, 0.0f, 1.0f,
    1.0f, 0.3f, 1.0f,///
    1.0f, 0.5f, 0.5f,
    0.5f, 1.0f, 0.5f,
    0.5f, 0.5f, 1.0f
};

Orientation orientations[64] = {
    /// 0
    zero_degrees,
    /// 1-8
    zero_degrees, zero_degrees,
    one_eighty_degrees, one_eighty_degrees,
    two_seventy_degrees, two_seventy_degrees,
    ninety_degrees, ninety_degrees,
    /// 9-40
    zero_degrees, zero_degrees, zero_degrees, zero_degrees, zero_degrees, zero_degrees, zero_degrees, zero_degrees,
    one_eighty_degrees, one_eighty_degrees, one_eighty_degrees, one_eighty_degrees, one_eighty_degrees, one_eighty_degrees, one_eighty_degrees, one_eighty_degrees,
    two_seventy_degrees, two_seventy_degrees, two_seventy_degrees, two_seventy_degrees, two_seventy_degrees, two_seventy_degrees, two_seventy_degrees, two_seventy_degrees,
    ninety_degrees, ninety_degrees, ninety_degrees, ninety_degrees, ninety_degrees, ninety_degrees, ninety_degrees, ninety_degrees,
    /// 41-44
    zero_degrees, one_eighty_degrees, two_seventy_degrees, ninety_degrees,
    /// 45-48
    zero_degrees, ninety_degrees, two_seventy_degrees, one_eighty_degrees,
    /// 49-52
    zero_degrees, ninety_degrees, two_seventy_degrees, one_eighty_degrees,
    /// 53-56
    two_seventy_degrees, ninety_degrees, zero_degrees, one_eighty_degrees,
    /// 57-60
    zero_degrees, ninety_degrees, one_eighty_degrees, two_seventy_degrees,
    /// 61-63
    zero_degrees, zero_degrees, zero_degrees
};

Shape_Type shape_types[64] = {
    Cube,
    Two_Point_Slope_26, Two_Point_Slope_26, Two_Point_Slope_26, Two_Point_Slope_26, Two_Point_Slope_26, Two_Point_Slope_26, Two_Point_Slope_26, Two_Point_Slope_26,
    Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7,
    Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7,
    Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7,
    Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7, Two_Point_Slope_7,
    Two_Point_Slope_45, Two_Point_Slope_45, Two_Point_Slope_45, Two_Point_Slope_45,
    Diagonal, Diagonal, Diagonal, Diagonal,
    Three_Point_Slope, Three_Point_Slope, Three_Point_Slope, Three_Point_Slope,
    Half_Cube, Half_Cube, Half_Cube, Half_Cube,
    Quarter_Cube, Quarter_Cube, Quarter_Cube, Quarter_Cube,
    Pillar, Cube, Cube
};

GLfloat vertices[SHAPE_TYPE_COUNT][5][VERTEX_COUNT][3] = {
    /// Cube
    1, 1, 0,   1, 0, 0,   1, 0, 1,   1, 1, 1,   // left face
    0, 0, 0,   0, 1, 0,   0, 1, 1,   0, 0, 1,   // right face
    0, 1, 0,   1, 1, 0,   1, 1, 1,   0, 1, 1,   // front face
    1, 0, 0,   0, 0, 0,   0, 0, 1,   1, 0, 1,   // rear face
    0, 1, 1,   1, 1, 1,   1, 0, 1,   0, 0, 1,   // lid face
    /// Two_Point_Slope_26
    1, 1, 0,   1, 0, 0,   1, 0, 1,   1, 1, 0.5f,   // left face
    0, 0, 0,   0, 1, 0,   0, 1, 0.5f,   0, 0, 1,   // right face
    0, 1, 0,   1, 1, 0,   1, 1, 0.5f,   0, 1, 0.5f,   // front face
    1, 0, 0,   0, 0, 0,   0, 0, 1,   1, 0, 1,   // rear face
    0, 1, 0.5f,   1, 1, 0.5f,   1, 0, 1,   0, 0, 1,   // lid face
    /// Two_Point_Slope_26
    1, 1, 0,   1, 0, 0,   1, 0, 0.5f,   1, 1, 0,   // left face
    0, 0, 0,   0, 1, 0,   0, 1, 0,   0, 0, 0.5f,   // right face
    0, 1, 0,   1, 1, 0,   1, 1, 4,   0, 1, 4,   // front face
    1, 0, 0,   0, 0, 0,   0, 0, 0.5f,   1, 0, 0.5f,   // rear face
    0, 1, 0,   1, 1, 0,   1, 0, 0.5f,   0, 0, 0.5f,   // lid face
};

//possibly inverted?
GLfloat vertical_offsets[64] = {
    0,
    -0.5f, 0, -0.5f, 0, -0.5f, 0, -0.5f, 0,
    0, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f,
    0, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f,
    0, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f,
    0, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0
};

class Geometry {
public:
    Geometry( uint8_t shape, uint16_t lid_texture_id )
    {
        //uint8_t ground_type = ( shape & 0x3 );
        orientation = orientations[ ( shape >> 2 ) & 0x3F ];
        shape_type = shape_types[ ( shape >> 2 ) & 0x3F ];
        vertical_offset = vertical_offsets[ ( shape >> 2 ) & 0x3F ];

        if (( shape_type == Three_Point_Slope ) && ( lid_texture_id == 1023 ))
            shape_type = One_Point_Slope;
    }

    GLfloat vertical_offset;
    Orientation orientation;

    Shape_Type shape_type;
};

Block_Face cycle_face_clockwise( Block_Face face, unsigned int steps = 1 )
{
    if ( steps == 0 ) return face;

    switch ( face )
    {
        case Front : return cycle_face_clockwise( Left, --steps );
        case Left : return cycle_face_clockwise( Rear, --steps );
        case Rear : return cycle_face_clockwise( Right, --steps );
        case Right : return cycle_face_clockwise( Front, --steps );
        default : return Lid;
    }
}

enum Face_Orientation { X_Plane = 0, Y_Plane, Z_Plane,  };

struct Coord3f {
    Coord3f( GLfloat a, GLfloat b, GLfloat c )
    :   x( a ), y( b ), z( c ) { ; }

    GLfloat x, y, z;
};

Coord3f coord_at( Shape_Type shape, Block_Face face, Vertex vertex )
{
    return Coord3f(
        vertices[ shape ][ face ][ vertex ][0],
        vertices[ shape ][ face ][ vertex ][1],
        vertices[ shape ][ face ][ vertex ][2]
    );
}

struct Coord2f {
    Coord2f( GLfloat a, GLfloat b )
    :   x( a ), y( b ) { ; }

    GLfloat x, y;
};

Coord2f tex_coord_at( Shape_Type shape, Block_Face face, Vertex vertex )
{
    Coord3f vertex_coords = coord_at( shape, face, vertex );

    switch ( face )
    {
        case Left :
        case Right :
            return Coord2f( vertex_coords.y, vertex_coords.z );
        case Front :
        case Rear :
            return Coord2f( vertex_coords.x, vertex_coords.z );
        case Lid :
            return Coord2f( vertex_coords.x, vertex_coords.y );
    }
}

Coord2f tex_coord_at( Block_Face face, Coord3f vertex_coords )
{
    switch ( face )
    {
        case Left :
        case Right :
            return Coord2f( vertex_coords.y, vertex_coords.z );
        case Front :
        case Rear :
            return Coord2f( vertex_coords.x, vertex_coords.z );
        case Lid :
            return Coord2f( vertex_coords.x, vertex_coords.y );
    }
}

void face_draw( const block_binary_info& block, const Geometry& geometry, const Block_Face& face, const Style& style )
{
    enum Flip { No_Flip = 0, Flip_Rotate, FLIP_TYPES_COUNT };

    ///                                                                                                                                       no flip,                     flip-rotate
    static GLfloat tex_rot0[FLIP_TYPES_COUNT][VERTEX_COUNT][2] = {{ 0,0, 1,0, 1,1, 0,1 }, { 1,0, 0,0, 0,1, 1,1 }};
    static GLfloat tex_rot90[2][4][2] = { 1,0, 1,1, 0,1, 0,0,  1,1, 1,0, 0,0, 0,1 };
    static GLfloat tex_rot180[2][4][2] = { 1,1, 0,1, 0,0, 1,0,  0,1, 1,1, 1,0, 0,0 };
    static GLfloat tex_rot270[2][4][2] = { 0,1, 0,0, 1,0, 1,1,  0,0, 0,1, 1,1, 1,0 };

    static Vertex rot0[ FLIP_TYPES_COUNT ][ VERTEX_COUNT ] = {{ Upper_Left, Upper_Right, Lower_Right, Lower_Left }, { Upper_Right, Upper_Left, Lower_Left, Lower_Right }};
    static Vertex rot90[ FLIP_TYPES_COUNT ][ VERTEX_COUNT ] = {{ Upper_Right, Lower_Right, Lower_Left, Upper_Left }, { Lower_Right, Upper_Right, Upper_Left, Lower_Left }};
    static Vertex rot180[ FLIP_TYPES_COUNT ][ VERTEX_COUNT ] = {{ Lower_Right, Lower_Left, Upper_Left, Upper_Right }, { Lower_Left, Lower_Right, Upper_Right, Upper_Left }};
    static Vertex rot270[ FLIP_TYPES_COUNT ][ VERTEX_COUNT ] = {{ Lower_Left, Upper_Left, Upper_Right, Lower_Right }, { Upper_Left, Lower_Left, Lower_Right, Upper_Right }};

    /// pivoted_face == the texture that will occupy the position of 'face' after rotating the whole block
    Block_Face pivoted_face = cycle_face_clockwise( face, geometry.orientation );

    uint16_t texture_id = block.face_texture( pivoted_face );
    Orientation tex_rotate = block.face_orientation( pivoted_face );
    bool xflip = block.face_xflip( pivoted_face );
    Flip flip_type = ( xflip ) ? Flip_Rotate : No_Flip;

    if ( true )
        glColor3fv( colours[ xflip ] );
    else
        glColor3f( 1.0f, 1.0f, 1.0f );

    //if ( geometry.shape_type == Cube ) return;
    //if ( face == Lid ) return;

    glPushMatrix();

    /// rotate entire block
    switch ( geometry.orientation )
    {
        case ninety_degrees : glRotatef( 90, 0.0f, 0.0f, 1.0f ); break;
        case one_eighty_degrees : glRotatef( 180, 0.0f, 0.0f, 1.0f ); break;
        case two_seventy_degrees : glRotatef( 270, 0.0f, 0.0f, 1.0f ); break;
        default : break;
    }

    /// move block such that origin is in centre, and shift vertically (for multi-part slopes)
    glTranslatef( -0.5f, -0.5f, -0.5f + geometry.vertical_offset );

    /// zero texture is always invisible
    if ( texture_id != 0 )
    {
        glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ texture_id ] );
        glBegin( GL_QUADS );

            Orientation rotation_value = tex_rotate;

            /// when rotating Lid texture, take into account the rotation of the block itself
            if ( face == Lid )
            {
                rotation_value = Orientation(
                    ( xflip )
                    ? (( tex_rotate + geometry.orientation ) + 4 ) % 4
                    : (( tex_rotate - geometry.orientation ) + 4 ) % 4
                );
            }

            for ( unsigned int quad_vertex = Upper_Left; quad_vertex != VERTEX_COUNT; quad_vertex += 1 )
            {
                GLfloat (*tex_coord)[4][2];
                switch ( rotation_value )
                {
                    case 0 : tex_coord = tex_rot0; break;
                    case 1 : tex_coord = tex_rot90; break;
                    case 2 : tex_coord = tex_rot180; break;
                    case 3 : tex_coord = tex_rot270; break;
                }

                Vertex (*rot)[ FLIP_TYPES_COUNT ][ VERTEX_COUNT ];

                glTexCoord2f(
                    tex_coord[ flip_type ][ quad_vertex ][0],
                    tex_coord[ flip_type ][ quad_vertex ][1]
                );

                Coord3f coords = coord_at( geometry.shape_type, face, Vertex( quad_vertex ));
                Coord2f tex_coords = tex_coord_at( face, coords );

     //           glTexCoord2f( tex_coords.x, tex_coords.y );
                glVertex3f( coords.x, coords.y, coords.z );
            }
        glEnd();
    }

    glPopMatrix();
}

/*void face_draw( const block_binary_info& block, const block_binary_info::Block_Face& face, const Style& style )
{
    static GLfloat div2 = 0.5f;
    static GLfloat tex[16] = { 0,0, 1,0, 1,1, 0,1, 0,0, 1,0, 1,1, 0,1 };
    static GLfloat tex_xflip[16] = { 0,0, -1,0, -1,1, 0,1, 0,0, -1,0, -1,1, 0,1 };
    static GLfloat vtx[60] = {
        div2, div2, -div2,    div2, -div2, -div2,     div2, -div2, div2,      div2, div2, div2,
        -div2, -div2, -div2,  -div2, div2, -div2,     -div2, div2, div2,      -div2, -div2, div2,
        -div2, div2, -div2,   div2, div2, -div2,      div2, div2, div2,       -div2, div2, div2,
        div2, -div2, -div2,   -div2, -div2, -div2,    -div2, -div2, div2,     div2, -div2, div2,
        div2, -div2, div2,    -div2, -div2, div2,     -div2, div2, div2,      div2, div2, div2
    };

    uint16_t texture_id = block.face_texture( face );
    block_binary_info::Face_Orientation tex_rotate = block.face_orientation( face );
    bool xflip = block.face_xflip( face );

#if true
    if ( xflip )
        glColor3f( 1.0f, 0.5f, 0.5f );
    else
        glColor3f( 1.0f, 1.0f, 1.0f );
#endif

    // zero texture is always invisible
    if ( texture_id != 0 )
    {
        glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ texture_id ] );
        glBegin( GL_QUADS );
            for ( int vertex = 0; vertex != 4; ++vertex )
            {
                if ( false )
                    glTexCoord2fv( tex_xflip + (vertex*2) + (tex_rotate*2) );
                else
                    glTexCoord2fv( tex + (vertex*2) + (tex_rotate*2) );

                glVertex3fv( vtx + face*12 + vertex*3 );
            }
        glEnd();
    }
}*/

void dmap_column_binary_info::draw( const std::vector<block_binary_info>& block_info, const Style& style )
{
    for ( int z = (int)offset; z != (int)height; ++z )
    {
        uint32_t block_id = blockd[ z - offset ];

        const block_binary_info& block = block_info[ block_id ];

        Geometry geometry(
            block.slope_type,
            block.face_texture( Lid )
        );

        glPushMatrix();

            glTranslatef( 0.0f, 0.0f, z );

            glEnable( GL_TEXTURE_2D );
            face_draw( block, geometry, Left, style );
            face_draw( block, geometry, Right, style );
            face_draw( block, geometry, Rear, style );
            face_draw( block, geometry, Front, style );
            face_draw( block, geometry, Lid, style );
            /*glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Left ) ] );
            glBegin( GL_QUADS );
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, -h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Right ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, -h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Rear ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Front ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, -h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, -h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Lid ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, -h2, d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, -h2, d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, h2, d2 );
            glEnd();*/
            glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }
}

/**void dmap_column_binary_info::draw( const std::vector<block_binary_info>& block_info, const Style& style )
{
    float w2 = 1 / 2.0f;
    float h2 = 1 / 2.0f;
    float d2 = 1 / 2.0f;

    //cout << (int)offset << " -> " << (int)height << endl;

    for ( int z = (int)offset; z != (int)height; ++z )
    {
        uint32_t block_id = blockd[ z - offset ];
        //if ( block_id == 0 ) continue;

        const block_binary_info& block = block_info[ block_id ];

        glPushMatrix();

            glTranslatef( 0.0f, 0.0f, z );

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Left ) ] );
            glBegin( GL_QUADS );
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, -h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Right ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, -h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Rear ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Front ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, -h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, -h2, d2 );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ block.face_texture( block_binary_info::Lid ) ] );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, -h2, d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, -h2, d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, h2, d2 );
            glEnd();
            glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }
}*/
