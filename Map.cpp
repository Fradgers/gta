#include "Map.h"
#include "Style.h"


void face_draw( const block_binary_info& block, const block_binary_info::Block_Face& face, const Style& style )
{
    static GLfloat div2 = 0.5f;
    static GLfloat tex_rot0[16] = { 0,0, 1,0, 1,1, 0,1,  1,0, 0,0, 0,1, 1,1 };
    static GLfloat tex_rot90[16] = { 0,1, 0,0, 1,0, 1,1,  0,0, 0,1, 1,1, 1,0 };
    static GLfloat tex_rot180[16] = { 1,1, 0,1, 0,0, 1,0,  0,1, 1,1, 1,0, 0,0 };
    static GLfloat tex_rot270[16] = { 1,0, 1,1, 0,1, 0,0,  1,1, 1,0, 0,0, 0,1 };

    static GLfloat vtx[60] = {
        div2, div2, -div2,    div2, -div2, -div2,     div2, -div2, div2,      div2, div2, div2, // left face
        -div2, -div2, -div2,  -div2, div2, -div2,     -div2, div2, div2,      -div2, -div2, div2, // right face
        -div2, div2, -div2,   div2, div2, -div2,      div2, div2, div2,       -div2, div2, div2, // front face
        div2, -div2, -div2,   -div2, -div2, -div2,    -div2, -div2, div2,     div2, -div2, div2, // rear face
        div2, -div2, div2,    -div2, -div2, div2,     -div2, div2, div2,      div2, div2, div2 // lid face
    };

    uint16_t texture_id = block.face_texture( face );
    int tex_rotate = block.face_orientation( face );
    bool xflip = block.face_xflip( face );

    // zero texture is always invisible
    if ( texture_id != 0 )
    {
        glBindTexture( GL_TEXTURE_2D, style.tiles.texture_lookup[ texture_id ] );
        glBegin( GL_QUADS );
            for ( int vertex = 0; vertex != 4; ++vertex )
            {
                switch ( tex_rotate )
                {
                    case 0 : glTexCoord2fv( tex_rot0 + (vertex*2) + xflip*8 ); break;
                    case 1 : glTexCoord2fv( tex_rot90 + (vertex*2) + xflip*8 ); break;
                    case 2 : glTexCoord2fv( tex_rot180 + (vertex*2) + xflip*8 ); break;
                    case 3 : glTexCoord2fv( tex_rot270 + (vertex*2) + xflip*8 ); break;
                    default : glColor3f( 0.0f,0.0f,0.0f );
                }

                glVertex3fv( vtx + face*12 + vertex*3 );
            }
        glEnd();
    }
}

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
