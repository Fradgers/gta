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
    Pillar, None, None
};

GLfloat vertices[60]/*[SHAPE_TYPE_COUNT]*/ = {
    /// Cube
    1, 1, 0,   1, 0, 0,   1, 0, 1,   1, 1, 1,   // left face
    0, 0, 0,   0, 1, 0,   0, 1, 1,   0, 0, 1,   // right face
    0, 1, 0,   1, 1, 0,   1, 1, 1,   0, 1, 1,   // front face
    1, 0, 0,   0, 0, 0,   0, 0, 1,   1, 0, 1,   // rear face
    0, 1, 1,   1, 1, 1,   1, 0, 1,   0, 0, 1    // lid face
    //-div2, -div2, div2,    div2, -div2, div2,     div2, div2, div2,      -div2, div2, div2 // lid face
};

/* //possibly inverted?
GLfloat vertical_offsets[64] = {
    0,
    0, 0.5f, 0, 0.5f, 0, 0.5f, 0, 0.5f,
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
};*/

class Geometry {
public:
    Geometry( uint8_t shape, uint16_t lid_texture_id )
    {
        //uint8_t ground_type = ( shape & 0x3 );
        orientation = orientations[ ( shape >> 2 ) & 0x3F ];
        shape_type = shape_types[ ( shape >> 2 ) & 0x3F ];
        //vertical_offset = vertical_offsets[ ( shape >> 2 ) & 0x3F ];

        if (( shape_type == Three_Point_Slope ) && ( lid_texture_id == 1023 ))
            shape_type = One_Point_Slope;
    }

    GLfloat vertical_offset;
    Orientation orientation;

    Shape_Type shape_type;
};

void face_draw( const block_binary_info& block, const block_binary_info::Block_Face& face, const Style& style )
{
        static GLfloat div2 = 0.5f;
        ///                                                     no flip,                   flip-rotate,          rotate-flip
        static GLfloat tex_rot0[24] = { 0,0, 1,0, 1,1, 0,1,  1,0, 0,0, 0,1, 1,1,   1,0, 0,0, 0,1, 1,1,  };
        static GLfloat tex_rot90[24] = { 1,0, 1,1, 0,1, 0,0,  1,1, 1,0, 0,0, 0,1,    0,0, 0,1, 1,1, 1,0 };
        static GLfloat tex_rot180[24] = { 1,1, 0,1, 0,0, 1,0,  0,1, 1,1, 1,0, 0,0,   0,1, 1,1, 1,0, 0,0 };
        static GLfloat tex_rot270[24] = { 0,1, 0,0, 1,0, 1,1,  0,0, 0,1, 1,1, 1,0,   1,1, 1,0, 0,0, 0,1 };

        static GLfloat vtx[60] = {
              div2, div2, -div2,     div2, -div2, -div2, div2, -div2, div2,     div2, div2, div2,    // left face
            -div2, -div2, -div2,  -div2, div2, -div2,   -div2, div2, div2,     -div2, -div2, div2,   // right face
               -div2, div2, -div2,   div2, div2, -div2, div2, div2, div2,      -div2, div2, div2,      // front face
              div2, -div2, -div2,   -div2, -div2, -div2, -div2, -div2, div2,    div2, -div2, div2,  // rear face
              -div2, div2, div2,     div2, div2, div2,  div2, -div2, div2,    -div2, -div2, div2// lid face
            //-div2, -div2, div2,    div2, -div2, div2,     div2, div2, div2,      -div2, div2, div2 // lid face
        };
        /*    div2, -div2, div2,     div2, div2, div2,     div2, div2, -div2,     div2, -div2, -div2,  // left face
            -div2, div2, div2,     -div2, -div2, div2,  -div2, -div2, -div2,  -div2, div2, -div2,    // right face
            div2, div2, div2,      -div2, div2, div2,    -div2, div2, -div2,   div2, div2, -div2,      // front face
            -div2, -div2, div2,    div2, -div2, div2,   div2, -div2, -div2,   -div2, -div2, -div2,  // rear face
            div2, -div2, div2,    -div2, -div2, div2,     -div2, div2, div2,     div2, div2, div2 // lid face
            //-div2, -div2, div2,    div2, -div2, div2,     div2, div2, div2,      -div2, div2, div2 // lid face
        };*/

    enum Flip { No_Flip = 0, Flip_Rotate, Rotate_Flip };

    uint16_t texture_id = block.face_texture( face );
    Orientation tex_rotate = block.face_orientation( face );
    bool xflip = block.face_xflip( face );
    Flip flip_type = ( xflip ) ? Flip_Rotate : No_Flip;
/*
#if false
    if ( xflip )
        glColor3f( 1.0f, 0.5f, 0.5f );
    else
        glColor3f( 1.0f, 1.0f, 1.0f );
#else
    switch ( tex_rotate )
    {
        case 0 : glColor3f( 1.0f, 1.0f, 1.0f ); break;
        case 1 : glColor3f( 1.0f, 0.5f, 0.5f ); break;
        case 2 : glColor3f( 0.5f, 1.0f, 0.5f ); break;
        case 3 : glColor3f( 0.5f, 0.5f, 1.0f ); break;
        default : glColor3f( 0.2f, 0.2f, 0.2f ); break;
    }
#endif
*/

    /*block_binary_info::Face_Lighting ambient_light = block.face_ambient_light( face );
    switch ( ambient_light )
    {
        case block_binary_info::None : glColor3f( 0.5f, 0.5f, 0.5f ); break;
        case block_binary_info::Low : glColor3f( 0.7f, 0.7f, 0.7f ); break;
        case block_binary_info::Medium : glColor3f( 0.85f, 0.85f, 0.85f ); break;
        case block_binary_info::High : glColor3f( 1.0f, 1.0f, 1.0f ); break;
        default : glColor3f( 0.2f, 0.2f, 0.2f ); break;
    }*/

    // zero texture is always invisible
    if ( texture_id != 0 )
    {
        glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ texture_id ] );
        glBegin( GL_QUADS );
            for ( int vertex = 0; vertex != 4; ++vertex )
            {
                switch ( tex_rotate )
                {
                    case 0 : glTexCoord2fv( tex_rot0 + (flip_type*8) + (vertex*2)); break;
                    case 1 : glTexCoord2fv( tex_rot90 + (flip_type*8) + (vertex*2)); break;
                    case 2 : glTexCoord2fv( tex_rot180 + (flip_type*8) + (vertex*2)); break;
                    case 3 : glTexCoord2fv( tex_rot270 + (flip_type*8) + (vertex*2)); break;
                    default : glColor3f( 0.0f,0.0f,0.0f );
                }

                glVertex3fv( /*vtx*/vertices + face*12 + vertex*3 );
            }
        glEnd();
    }
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

        glPushMatrix();

            glTranslatef( 0.0f, 0.0f, z );

            glEnable( GL_TEXTURE_2D );
            face_draw( block, block_binary_info::Left, style );
            face_draw( block, block_binary_info::Right, style );
            face_draw( block, block_binary_info::Rear, style );
            face_draw( block, block_binary_info::Front, style );
            face_draw( block, block_binary_info::Lid, style );
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
