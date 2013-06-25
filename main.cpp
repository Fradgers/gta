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
/*
class Textured_Cube {
public:
    Textured_Cube( const Vec3& position, const Vec3& dim, const Texture_Collection::Texture_Name& tex )
    :   centre( position ),
        dimensions( dim ),
        texture( tex )
    { ; }

    enum Side { Left, Right, Front, Rear, Top, Bottom };

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
    }

    Vec3 centre;
    Vec3 dimensions;
    Texture_Collection::Texture_Name texture;
};
*/


#include <stdint.h>
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

#include <cassert>


#include <sstream>


#include "Map.h"
#include "Style.h"







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
                case LGHT :
                case EDIT :
                case RGEN :
                ///case MOBJ :
                {
                    map_chunk_reader.body( stream );
                    cout << "<OTHER>" << endl;
                    break;
                }
                default :
                {
                    //cout << "<UNRECOGNISED>" << endl;
                    break;
                }
            }
        }
    }

    void draw( const Style& style )
    {
        if ( dmap ) dmap->draw( style );
    }

    ~Map() { delete dmap; }

    dmap_reader* dmap;
};


int main( int argc, char** argv )
{
    OpenGL opengl;
    if ( ! opengl.good() ) return -1;

    std::ifstream stream( "MP1-comp.gmp", ios::in | ios::binary );
    ///std::ifstream stream( "wil.gmp", ios::in | ios::binary );
    if ( ! stream.is_open() )
        return -1;

    Map map( stream );
    stream.close();

    stream.open( "bil.sty", ios::in | ios::binary );
    ///stream.open( "wil.sty", ios::in | ios::binary );
    if ( ! stream.is_open() )
        return -1;

    Style style( stream );
    stream.close();

    Vec3 camera( 93.0f, 196.0f, 20.0f );
    enum Cameras { Map = 0, Aerial_NW, Aerial_SE, Rooftop_NW, Rooftop_SE, Street, CAMERA_COUNT }
        camera_angle = Aerial_SE;
    //Vec3 camera( 0.0f, 0.0f, 130.0f );

 //   Texture_Collection textures;
//    Texture_Collection::Texture_Name tex1 = textures.load_texture( "test.tga" );
   // Texture_Collection::Texture_Name tex2 = textures.load_texture( "text_test.tga" );

    //Textured_Cube cube1( Vec3( 1.0f, -2.0f, -1.0f ), Vec3( 1,1,1 ), tex1 );
    //Textured_Cube cube2( Vec3( 1.0f, -2.0f, 0.0f ), Vec3( 1,1,1 ), tex2 ) ;

    while ( ! glfwGetKey( GLFW_KEY_ESC ) )
    {
        //glClearColor( 0.8f, 0.5f, 0.5f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        if ( glfwGetKey( GLFW_KEY_LEFT )) {  camera += Vec3(  0.8f, 0.0f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_RIGHT )){ camera += Vec3( -0.8f, 0.0f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_UP ))  { camera += Vec3(  0.0f, 0.8f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}
        if ( glfwGetKey( GLFW_KEY_DOWN )){ camera += Vec3( 0.0f, -0.8f, 0.0f ); std::cout << "(" << camera.x << "," << camera.y << "," << camera.z << ")" << endl;}



        GLfloat camera_angles[CAMERA_COUNT][4] = {
            -135.0f, -0.5f, -0.5f, -0.95f,
            180.0f, 0.0f, 0.0f, 0.1f,
            180.0f, 0.0f, 0.0f, 0.1f,
            -135, 1.0f, 0.0f, 1.0f,
            0, 0.0f, 0.0f, 0.0f,
            0, 0.0f, 0.0f, 0.0f
        };

        if ( glfwGetKey( GLFW_KEY_F1 )) {  camera_angle = Map; }
        if ( glfwGetKey( GLFW_KEY_F2 )) {  camera_angle = Aerial_NW; }
        if ( glfwGetKey( GLFW_KEY_F3 )) {  camera_angle = Aerial_SE; }
        if ( glfwGetKey( GLFW_KEY_F4 )) {  camera_angle = Rooftop_NW; }
        if ( glfwGetKey( GLFW_KEY_F5 )) {  camera_angle = Rooftop_SE; }
        if ( glfwGetKey( GLFW_KEY_F6 )) {  camera_angle = Street; }

        //glRotatef( 180, 0.0f, 0.0f, 1.0f );
        glRotatef( camera_angles[camera_angle][ 0 ], camera_angles[camera_angle][ 1 ], camera_angles[camera_angle][2], camera_angles[camera_angle][3] );
        //glRotatef(-135, 0.5f, 0.5f, 1.0f );
        glTranslatef( -camera.x, -camera.y, -camera.z );

        /*glBegin(GL_QUADS);
            glColor3f( 0.2f,0.5f,0.2f );
            glVertex3f( -700, -700, 7 );
            glVertex3f( 700, -700, 7 );
            glVertex3f( 700, 700, 7 );
            glVertex3f( -700, 700, 7 );
        glEnd();*/

        /// draw here
        map.draw( style );
        //style.draw();

        glfwSwapBuffers();
    }

    return 0;
}

