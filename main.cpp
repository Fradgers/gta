#include <iostream>
#include <stdlib.h>

#include "Sprite.h"

using namespace std;

/*
    void draw()
    {
        glPushMatrix();
        // reset existing translations/rotations, and apply new ones
            glLoadIdentity();
            glTranslatef( centre.x, centre.y, centre.z );
            glRotatef( angle_degrees, 0.0f, 0.0f, 1.0f );

            // do drawing
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin( GL_QUADS );
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -0.5f, -0.5f, 0.0f );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f(  0.5f, -0.5f, 0.0f );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f(  0.5f,  0.5f, 0.0f );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -0.5f,  0.5f, 0.0f );
            glEnd();
            glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }*/




/*class Block_Archetype {
public:
    Texture_Collection::Texture_Name left, right, front, rear, lid;
};*/


class Textured_Cube {
public:
    Textured_Cube( const Vec3& position, const Vec3& dim, const Texture_Collection::Texture_Name& tex )
    :   centre( position ),
        dimensions( dim ),
        texture( tex )
    { ; }

    enum Side { Left, Right, Front, Rear, Lid };
    enum Orientation { zero_degrees = 0, ninety_degrees, one_eighty_degrees, two_seventy_degrees };

    void draw();
/*
    void draw()
    {
        float w2 = dimensions.x / 2.0f;
        float h2 = dimensions.y / 2.0f;
        float d2 = dimensions.z / 2.0f;

        glPushMatrix();
            glTranslatef( centre.x, centre.y, centre.z );

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin( GL_QUADS );
                //glBindTexture( GL_TEXTURE_2D, textures[Left] );
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, -h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, h2, d2 );

                //glBindTexture( GL_TEXTURE_2D, textures[Right] );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, -h2, d2 );

                //glBindTexture( GL_TEXTURE_2D, textures[Top] );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, -h2, d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, -h2, d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, h2, d2 );

                //glBindTexture( GL_TEXTURE_2D, textures[Bottom] );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, h2, -d2 );

                //glBindTexture( GL_TEXTURE_2D, textures[Front] );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( w2, -h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( -w2, -h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( -w2, -h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( w2, -h2, d2 );

                //glBindTexture( GL_TEXTURE_2D, textures[Back] );
            glEnd();
            glBindTexture( GL_TEXTURE_2D, texture );
            glBegin(GL_QUADS);
                glTexCoord2f( -0.0f, -0.0f ); glVertex3f( -w2, h2, -d2 );
                glTexCoord2f(  1.0f, -0.0f ); glVertex3f( w2, h2, -d2 );
                glTexCoord2f(  1.0f,  1.0f ); glVertex3f( w2, h2, d2 );
                glTexCoord2f( -0.0f,  1.0f ); glVertex3f( -w2, h2, d2 );
            glEnd();
            glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }*/

    Texture_Collection::Texture_Name textures[6];
    Vec3 centre;
    Vec3 dimensions;
    Texture_Collection::Texture_Name texture;
};


    void draw_face( const Textured_Cube::Side& face, const Texture_Collection::Texture_Name& texture, const Textured_Cube::Orientation& tex_rotate, bool xflip )
    {
        static GLfloat div2 = 0.5f;
        ///                                                     no flip,                   flip-rotate,          rotate-flip
        static GLfloat tex_rot0[24] = { 0,0, 1,0, 1,1, 0,1,  1,0, 0,0, 0,1, 1,1,   1,0, 0,0, 0,1, 1,1,  };
        //static GLfloat tex_rot90[16] = { 0,1, 0,0, 1,0, 1,1,  0,0, 0,1, 1,1, 1,0 };
        static GLfloat tex_rot90[24] = { 1,0, 1,1, 0,1, 0,0,  1,1, 1,0, 0,0, 0,1,    0,0, 0,1, 1,1, 1,0 };
        static GLfloat tex_rot180[24] = { 1,1, 0,1, 0,0, 1,0,  0,1, 1,1, 1,0, 0,0,   0,1, 1,1, 1,0, 0,0 };
        //static GLfloat tex_rot270[16] = { 1,0, 1,1, 0,1, 0,0,  1,1, 1,0, 0,0, 0,1 };
        static GLfloat tex_rot270[24] = { 0,1, 0,0, 1,0, 1,1,  0,0, 0,1, 1,1, 1,0,   1,1, 1,0, 0,0, 0,1 };

        static GLfloat vtx[60] = {
            div2, div2, -div2,    div2, -div2, -div2,     div2, -div2, div2,      div2, div2, div2, // left face
            -div2, -div2, -div2,  -div2, div2, -div2,     -div2, div2, div2,      -div2, -div2, div2, // right face
            -div2, div2, -div2,   div2, div2, -div2,      div2, div2, div2,       -div2, div2, div2, // front face
            div2, -div2, -div2,   -div2, -div2, -div2,    -div2, -div2, div2,     div2, -div2, div2, // rear face
            div2, -div2, div2,    -div2, -div2, div2,     -div2, div2, div2,      div2, div2, div2 // lid face
            //-div2, -div2, div2,    div2, -div2, div2,     div2, div2, div2,      -div2, div2, div2 // lid face
        };

        enum Flip { No_Flip = 0, Rotate_Flip, Flip_Rotate };

        ///block_binary_info::Face_Orientation tex_rotate = block_binary_info::zero_degrees;
        ///Flip flip_type = No_Flip;
        Flip flip_type = ( xflip ) ? Rotate_Flip : No_Flip;

    #if true
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

        glBindTexture( GL_TEXTURE_2D, texture );
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

                glVertex3fv( vtx + face*12 + vertex*3 );
            }
        glEnd();

    }


void Textured_Cube::draw()
{
    glPushMatrix();
        glTranslatef( centre.x, centre.y, centre.z );

        glEnable( GL_TEXTURE_2D );
            Textured_Cube::Orientation orientation = zero_degrees;
            bool xflip = false;

            draw_face( Left, texture, orientation, xflip );
            draw_face( Right, texture, orientation, xflip );
            draw_face( Rear, texture, orientation, xflip );
            draw_face( Front, texture, orientation, xflip );
            draw_face( Lid, texture, orientation, xflip );
        glDisable( GL_TEXTURE_2D );
    glPopMatrix();
}


int main( int argc, char** argv )
{
    OpenGL opengl;
    if ( ! opengl.good() ) return -1;

    Vec3 camera( 0.0f, 0.0f, 7.0f );

    Texture_Collection textures;
    Texture_Collection::Texture_Name tex = textures.load_texture( "text_test.tga" );

    Textured_Cube cube( Vec3( 1.0f, -2.0f, -1.0f ), Vec3( 1,1,1 ), tex );

    while ( ! glfwGetKey( GLFW_KEY_ESC ) )
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        if ( glfwGetKey( GLFW_KEY_LEFT ))  camera += Vec3(  0.02f, 0.0f, 0.0f );
        if ( glfwGetKey( GLFW_KEY_RIGHT )) camera += Vec3( -0.02f, 0.0f, 0.0f );
        if ( glfwGetKey( GLFW_KEY_UP ))  camera += Vec3(  0.0f, 0.02f, 0.0f );
        if ( glfwGetKey( GLFW_KEY_DOWN )) camera += Vec3( 0.0f, -0.02f, 0.0f );

        glTranslatef( -camera.x, -camera.y, -camera.z );

        /// draw here
        cube.draw();

        glfwSwapBuffers();
    }

    return 0;
}

