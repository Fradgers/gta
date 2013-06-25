#include "Sprites.h"
#include "Vec3.h"

#include "Collision_Volume.h"


class Object : public ICollidable {
public:
    Object()
    :   ICollidable( Vec3( 76.5f, 83.0f, 2.0f ) )
    {
        sprite_number = 10;
        w = 0.5f;
        h = 1.0f;
        wdiv2 = w / 2.0f;
        hdiv2 = h / 2.0f;
        angle = 35.0f;
        velocity = 0.25f;
    }

    Vec3 movement_vector()
    {
        return Vec3(
            velocity * -sin( deg_to_rad * angle ),
            velocity *  cos( deg_to_rad * angle ),
            0
        );
    }

    virtual void collision_response( const Collision::Physics_Data& ) { std::cout << "COLLISION!" << std::endl; velocity = 0; }

    Collision::Collision_Volume collision_volume() const
    {
        std::vector<Vec3> vertices;

        GLfloat radians = deg_to_rad * angle;

        Vec3 X(  cos( radians ), sin( radians ), 0 );
        Vec3 Y( -sin( radians ), cos( radians ), 0 );

        X *= wdiv2;
        Y *= hdiv2;

        vertices.push_back( pos - X - Y );
        vertices.push_back( pos + X - Y );
        vertices.push_back( pos + X + Y );
        vertices.push_back( pos - X + Y );

        std::vector<Collision::Axis> axes;

        axes.push_back( Collision::Axis( vertices[1] - vertices[0], vertices ));
        axes.push_back( Collision::Axis( vertices[3] - vertices[0], vertices ));

        return Collision::Collision_Volume( axes, vertices );
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
            glTranslatef( pos.x, pos.y, pos.z );
            glRotatef( angle, 0, 0, 1 );

            /// draw the textured quad
            glBegin( GL_QUADS );
                glTexCoord2f( 0,1 );
                glVertex3f( -wdiv2, hdiv2, 0 );
                glTexCoord2f( 0,0 );
                glVertex3f( -wdiv2, -hdiv2, 0 );
                glTexCoord2f( 1,0 );
                glVertex3f( wdiv2, -hdiv2, 0 );
                glTexCoord2f( 1,1 );
                glVertex3f( wdiv2, hdiv2, 0 );
            glEnd();
        glPopMatrix();
    }

    uint8_t sprite_number;
    GLfloat w;
    GLfloat h;
    GLfloat wdiv2;
    GLfloat hdiv2;
    GLfloat angle;
    GLfloat velocity;
};


class Objects {
public:
    void spawn()
    {

    }

private:
    std::vector<Object> objects;
};
