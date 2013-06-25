/*
#include <vector>
#include "Vec3.h"
#include "Texture_Collection.h"

class Sprite {
public:
    Sprite( Texture_Collection& textures, const std::string& texture_filename, const Vec3& position, float angle )
    :   centre( position ),
        angle_degrees( angle )
    {
        texture = textures.load_texture( texture_filename );
    }

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
//            glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }

private:
    Texture_Collection::Texture_Name texture;
    Vec3 centre;
    float angle_degrees;
};*/
