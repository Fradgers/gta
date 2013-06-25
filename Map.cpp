#include "Map.h"
#include "Style.h"

#include "geometry_data.h"


GLfloat clamp( GLfloat value, const GLfloat& min, const GLfloat& max )
{
    // modifying and returning a copy
    value = std::max( value, min );
    value = std::min( value, max );

    return value;
}


void shift_side_into_position( const GLfloat& facing_angle, const GLfloat& offset )
{
    glRotatef( facing_angle, 0,0,1 );
    glTranslatef( 0, -offset-0.5f, 0 );
    glRotatef( 90.0f, 1,0,0 );
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


void face_draw( const block_binary_info& block, const Block_Face& side, const Style& style, const Vec3& coords )
{
    uint8_t block_type = ( block.slope_type >> 2 ) & 0x3F;

    Face face = blocks[ block_type ][ side ];

    uint16_t texture_id = block.face_texture( side );

    // zero texture is always invisible
    if ( texture_id != 0 && face.type != No_Face )
    {
        glColor3f( 1,1,1 );

        glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ texture_id ] );
        ///glBindTexture( GL_TEXTURE_2D, style.tiles.placeholder_texture );
        glPushMatrix();

        // rotate and flip texture prior to attaching to quad vertices
        glMatrixMode( GL_TEXTURE );
            GLfloat angle = block.face_orientation( side ) * 90.0f;

            glLoadIdentity();
            glTranslatef( 0.5f, 0.5f, 0.0f );
            glRotatef( angle, 0,0,1 );

            if ( block.face_xflip( side ))
                glRotatef( 180.0f, 0,1,0 );

            glTranslatef( -0.5f, -0.5f, 0.0f );
        glMatrixMode( GL_MODELVIEW );

        // move the frame of reference to the x,y,z coords of the block being drawn
        glTranslatef( coords.x, coords.y, coords.z );

        switch ( side )
        {
            case Rear  : shift_side_into_position( 180.0f + face.rotation, face.offset ); break;
            case Right : shift_side_into_position( 90.0f + face.rotation, face.offset ); break;
            case Front : shift_side_into_position( 0.0f + face.rotation, face.offset ); break;
            case Left  : shift_side_into_position( 270.0f + face.rotation, face.offset ); break;

            case Lid : shift_lid_into_position( face.rotation, face.axis, face.offset ); break;
            default : break;
        }

        // move block such that origin is in centre
        glTranslatef( -0.5f, -0.5f, 0 );

        /// draw the textured quad
        glBegin( GL_QUADS );

            for ( unsigned int vertex = 0; vertex != 4; ++vertex )
            {
                // tex coord is same as x,y of vertex coord to prevent stretching of texture
                // clamped to prevent tiling when quad is bigger than texture
                glTexCoord2f(
                    clamp( faces[ face.type ][ vertex*3 ], 0.0f, 1.0f ),
                    clamp( faces[ face.type ][ vertex*3 + 1 ], 0.0f, 1.0f )
                );

                glVertex3fv( faces[ face.type ] + vertex*3 );
            }
        glEnd();

        glPopMatrix();
    }
}


void dmap_column_binary_info::draw( const std::vector<block_binary_info>& block_info, const Style& style, Vec3& coords )
{
    if ( coords.z >= offset && coords.z < height )
    {
        uint32_t block_id = blockd[ coords.z - offset ];

        const block_binary_info& block = block_info[ block_id ];

        glPushMatrix();
            glEnable( GL_TEXTURE_2D );

                face_draw( block, Rear, style, coords );
                face_draw( block, Right, style, coords );
                face_draw( block, Front, style, coords );
                face_draw( block, Left, style, coords );
                face_draw( block, Lid, style, coords );

            glDisable( GL_TEXTURE_2D );
        glPopMatrix();


        /// !!!
        ///break;
    }
}
